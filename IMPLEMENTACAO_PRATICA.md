# Guia de Implementação Prática — Integração Passo a Passo

> Exemplos e padrões para integrar os novos recursos ao projeto

---

## 📝 Exemplo 1: Usando Tarifas Polimórficas

### Setup Inicial

```cpp
// Mapa para armazenar clientes mensalistas
std::unordered_map<std::string, Cliente*> clientesMensalistas;

// Registrar um cliente mensalista
Cliente* cliente1 = new Cliente("ABC-1234", "João Silva", Assinatura::PLANO_100H);
clientesMensalistas["ABC-1234"] = cliente1;
```

### Na Entrada

```cpp
bool Estacionamento::registrarEntrada(const std::string& placa, int tipo) {
    // ... validações ...

    Vaga* vagaLivre = estrategia->encontrarVaga(vagas, placa);
    if(!vagaLivre) return false;

    Veiculo* veiculo = (tipo == 0) ?
        new Carro(placa) : new Moto(placa);

    vagaLivre->alocar(veiculo);
    placaParaVaga[placa] = vagaLivre;

    return true;
}
```

### Na Saída (Polimorfismo em Ação!)

```cpp
float Estacionamento::registrarSaida(const std::string& placa) {
    auto it = placaParaVaga.find(placa);
    if(it == placaParaVaga.end()) return -1.0f;

    Vaga* vaga = it->second;
    Veiculo* veiculo = vaga->getVeiculo();

    // Calcular tempo
    auto duracao = std::chrono::duration_cast<std::chrono::minutes>
                  (agora - veiculo->getHoraEntrada());
    int tempoMinutos = duracao.count();

    // === POLIMORFISMO: Selecionar tarifa apropriada ===
    std::unique_ptr<TarifaVeiculo> tarifa = nullptr;
    float valor = 0.0f;

    auto itCliente = clientesMensalistas.find(placa);
    if(itCliente != clientesMensalistas.end()) {
        // Cliente mensalista
        tarifa = std::make_unique<TarifaMensalista>(itCliente->second);
        valor = tarifa->calcularCusto(tempoMinutos);

        std::cout << "Cliente mensalista " << itCliente->second->obterPlaca()
                  << " | Horas restantes: "
                  << itCliente->second->obterAssinatura().horasRestantes()
                  << std::endl;
    } else {
        // Cliente comum
        float taxa = (dynamic_cast<Carro*>(veiculo) != nullptr) ? 5.0f : 2.0f;        tarifa = std::make_unique<TarifaComum>(taxa);
        valor = tarifa->calcularCusto(tempoMinutos);

        std::cout << "Cliente comum | Valor: R$ " << valor << std::endl;
    }

    // Liberar estruturas
    vaga->liberar();
    delete veiculo;
    // delete tarifa; // não é mais necessário, unique_ptr limpa sozinho
    placaParaVaga.erase(placa);

    // Registrar transação
    transacoes.push_back(Transacao(placa, valor, agora));

    return valor;
}
```

**Resultado:**

- ✅ Cliente mensal com 100 horas: `0.0f` (horas debitadas do saldo)
- ✅ Cliente mensal fora do saldo: Cobra excedente a R$ 5/hora
- ✅ Cliente comum: Cobra R$ 5/hora (Carro) ou R$ 2/hora (Moto)

---

## 📝 Exemplo 2: Trocar Estratégia de Alocação em Runtime

### Cenário: Dia Alternado

```cpp
int main() {
    Estacionamento estacionamento;

    // De manhã: primeira vaga livre
    estacionamento.definirEstrategia(std::make_unique<EstrategiaVagaLivre>());
    estacionamento.registrarEntrada("ABC-1111", 0);  // Vaga 1
    estacionamento.registrarEntrada("ABC-2222", 0);  // Vaga 2

    // À tarde: mudar para vaga alternada
    estacionamento.definirEstrategia(std::make_unique<EstrategiaAlternada>());
    estacionamento.registrarEntrada("ABC-3333", 0);  // Vaga par ou ímpar?

    return 0;
}
```

---

## 📝 Exemplo 3: Priorizar Vagas para Clientes Mensalistas

```cpp
// Criar estratégia que separa vagas
auto cliente_map = estacionamento.obterClientesMapa();
estacionamento.definirEstrategia(
    std::make_unique<EstrategiaClienteEspecifico>(cliente_map)
);

// Agora:
// ABC-1234 (mensalista) → vagas 1-25
// XYZ-5678 (comum)      → vagas 26-50
estacionamento.registrarEntrada("ABC-1234", 0);  // Vaga ~10
estacionamento.registrarEntrada("XYZ-5678", 0);  // Vaga ~40
```

---

## 📝 Exemplo 4: Relatório Mensal de Clientes

```cpp
void Estacionamento::gerarRelatorioMensalista() {
    std::cout << "\n=== RELATÓRIO DE CLIENTES MENSALISTAS ===" << std::endl;

    for(auto& [placa, cliente] : clientesMensalistas) {
        Assinatura& asin = cliente->obterAssinatura();

        std::cout << "\nPlaca: " << placa << std::endl;
        std::cout << "Plano: " << static_cast<int>(asin.obterTipo())
                  << " horas" << std::endl;
        std::cout << "Horas usadas: " << asin.horasUsadas << std::endl;
        std::cout << "Horas restantes: " << asin.horasRestantes() << std::endl;

        if(asin.horasRestantes() <= 5) {
            std::cout << "⚠️  AVISO: Saldo baixo!" << std::endl;
        }
    }
}
```

**Saída:**

```
=== RELATÓRIO DE CLIENTES MENSALISTAS ===

Placa: ABC-1234
Plano: 100 horas
Horas usadas: 87
Horas restantes: 13
⚠️  AVISO: Saldo baixo!

Placa: DEF-5678
Plano: 50 horas
Horas usadas: 42
Horas restantes: 8
⚠️  AVISO: Saldo baixo!
```

---

## 📝 Exemplo 5: Sistema de Notificações (Observer Pattern)

```cpp
class ObservadorEstacionamento {
public:
    virtual ~ObservadorEstacionamento() = default;
    virtual void aoDespararEntrada(const std::string& placa) = 0;
    virtual void aoDespararSaida(const std::string& placa, float valor) = 0;
};

class NotificadorSaldo : public ObservadorEstacionamento {
public:
    void aoDespararSaida(const std::string& placa, float valor) override {
        // Se valor == 0, é mensal
        if(valor == 0.0f) {
            std::cout << "📌 Cliente mensalista " << placa
                      << " usou horas do saldo" << std::endl;
        } else {
            std::cout << "💰 Cobrança: R$ " << valor << " para " << placa
                      << std::endl;
        }
    }

    void aoDespararEntrada(const std::string& placa) override {
        std::cout << "🚗 Entrada registrada: " << placa << std::endl;
    }
};

// No Estacionamento:
class Estacionamento {
private:
    std::vector<ObservadorEstacionamento*> observadores;

public:
    void adicionarObservador(ObservadorEstacionamento* obs) {
        observadores.push_back(obs);
    }

    bool registrarEntrada(const std::string& placa, int tipo) {
        // ... lógica ...
        for(auto obs : observadores) {
            obs->aoDespararEntrada(placa);
        }
        return true;
    }

    float registrarSaida(const std::string& placa) {
        // ... lógica ...
        float valor = /* calcula */;
        for(auto obs : observadores) {
            obs->aoDespararSaida(placa, valor);
        }
        return valor;
    }
};

// Uso:
Estacionamento est;
est.adicionarObservador(new NotificadorSaldo());
est.registrarEntrada("ABC-1234", 0);
est.registrarSaida("ABC-1234");
```

**Saída:**

```
🚗 Entrada registrada: ABC-1234
📌 Cliente mensalista ABC-1234 usou horas do saldo
```

---

## 🔀 Decisão: Qual Estratégia Escolher?

### Para Clientes Mensalistas

**✅ Use:** Hierarquia `TarifaVeiculo` com `TarifaComum` e `TarifaMensalista`

- Simples de entender
- Fácil adicionar mais tipos (TarifaCorporativa, TarifaEventual)
- Não precisa de lógica complexa

---

### Para Alocação de Vagas

**Recomendação por Caso:**

| Caso                   | Estratégia                    | Razão                |
| ---------------------- | ----------------------------- | -------------------- |
| **Início simples**     | `EstrategiaVagaLivre`         | Menos código         |
| **Priorizar clientes** | `EstrategiaClienteEspecifico` | Separação clara      |
| **Otimizar uso**       | `EstrategiaProxima`           | Entrada mais perto   |
| **Teste A/B**          | `EstrategiaAlternada`         | Comparar desempenho  |
| **Combinar tudo**      | `Strategy + Decorator`        | Máxima flexibilidade |

**✅ Recomendação Final:** Comece com `EstrategiaVagaLivre`, depois evolua para `EstrategiaClienteEspecifico` quando os clientes mensalistas crescerem.

---

## 🧪 Testes Unitários

```cpp
void testarTarifaMensalista() {
    Cliente cliente("ABC-1234", "João", Assinatura::PLANO_30H);
    TarifaMensalista tarifa(&cliente);

    // 30 horas = 1800 minutos = grátis
    float custo1 = tarifa.calcularCusto(1800);
    assert(custo1 == 0.0f);  // ✓ Dentro do plano

    // 35 horas = 2100 minutos = 5h excedentes
    float custo2 = tarifa.calcularCusto(2100);
    assert(custo2 > 0.0f);  // ✓ Cobrado excedente
}

void testarEstrategias() {
    std::vector<Vaga> vagas;
    for(int i = 1; i <= 10; i++) {
        vagas.emplace_back(Vaga(i));
    }

    EstrategiaVagaLivre estrategia;
    Vaga* v = estrategia.encontrarVaga(vagas, "ABC");
    assert(v->getNumero() == 1);  // ✓ Primeira vaga
}
```

---

<div align=center>

[← Voltar](ARQUITETURA_REFINADA.md)

</div>
