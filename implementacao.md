# Implementação

> Nota Importante: Este documento relata o processo de implementação do sistema utilizando C++, STL e Qt.

## Ferramentas e Bibliotecas Utilizadas

| Ferramenta | Propósito |
|-----------|----------|
| **C++** | Linguagem base |
| **STL** | Estruturas de dados otimizadas |
| **Qt** | Interface gráfica (GUI) |
| **CMake** | Build system |

---

## Estruturas de Dados (STL)

### 1. Vector de Vagas — O Pátio Físico

```cpp
std::vector<Vaga> vagas;  // 50 vagas do estacionamento
```

**Por que?** Acesso direto por índice, para encontrar primeira vaga livre.

### 2️. Unordered_Map — Busca Instantânea

```cpp
std::unordered_map<std::string, Vaga*> placaParaVaga;
```

**Problema resolvido:** Sem este mapa, buscar um carro pela placa seria O(n).  
**Solução:** Busca hash em O(1) — praticamente instantânea!

```cpp
// Entrada: ABC-1234 entra na Vaga 5
placaParaVaga["ABC-1234"] = &vagas[5];

// Saída: Encontrar ABC-1234 instantaneamente
Vaga* vaga = placaParaVaga["ABC-1234"];  // O(1)
```

### 3. List para Histórico — Transações

```cpp
std::list<Transacao> transacoes;
```

**Benefício:** Inserção O(1) nas extremidades, ideal para histórico de cobranças.

---

## Polimorfismo: A Chave do Design

### Hierarquia de Classes

```
Veiculo (abstrato)
  ├── Carro (tarifa: R$ 5.00/hora)
  └── Moto (tarifa: R$ 2.00/hora)
```

### Implementação

```cpp
// Classe base abstrata
class Veiculo {
protected:
    std::string placa;
    std::chrono::system_clock::time_point horaEntrada;
    
public:
    virtual ~Veiculo() = default;
    virtual float calcularTarifa(int tempoMinutos) = 0;  // Puro
    std::string obterPlaca() const { return placa; }
};

// Especialização: Carro
class Carro : public Veiculo {
public:
    float calcularTarifa(int tempoMinutos) override {
        float horas = tempoMinutos / 60.0f;
        return horas * 5.0f;  // R$ 5 por hora
    }
};

// Especialização: Moto
class Moto : public Veiculo {
public:
    float calcularTarifa(int tempoMinutos) override {
        float horas = tempoMinutos / 60.0f;
        return horas * 2.0f;  // R$ 2 por hora
    }
};
```

**Vantagem:** Mesmo método `calcularTarifa()` comporta-se diferente conforme o tipo.

---

## Classes Principais

### Vaga

```cpp
class Vaga {
private:
    int numero;
    bool ocupada;
    Veiculo* veiculo;
    
public:
    void alocar(Veiculo* v);
    void liberar();
    bool estaOcupada() const;
    Veiculo* getVeiculo() const;
};
```

**Responsabilidade:** Gerenciar o estado (livre/ocupada) de uma vaga.

### Transacao

```cpp
class Transacao {
private:
    std::string placa;
    float valorPago;
    std::chrono::system_clock::time_point dataHora;
    
public:
    Transacao(const std::string& p, float v, const auto& dt);
    void exibir() const;  // Placa | Valor | Data/Hora
};
```

**Responsabilidade:** Registrar cobranças.

---

## Classe Estacionamento — Manager

Gerencia todo o fluxo e mantém as estruturas de dados sincronizadas.

### Entrada de Veículo

```cpp
bool Estacionamento::registrarEntrada(const std::string& placa, int tipo) {
    // 1. Encontrar primeira vaga livre
    Vaga* vagaLivre = nullptr;
    for(auto& vaga : vagas) {
        if(!vaga.estaOcupada()) {
            vagaLivre = &vaga;
            break;
        }
    }
    
    if(!vagaLivre) return false;  // Lotado
    
    // 2. Polimorfismo: criar tipo correto
    Veiculo* veiculo = (tipo == 0) ? 
        new Carro(placa) : new Moto(placa);
    
    // 3. Alocar e registrar no mapa
    vagaLivre->alocar(veiculo);
    placaParaVaga[placa] = vagaLivre;
    
    return true;
}
```

**Fluxo:** Valida → Aloca → Sincroniza mapa → Retorna sucesso

### Saída de Veículo

```cpp
float Estacionamento::registrarSaida(const std::string& placa) {
    // 1. Busca O(1) no mapa
    auto it = placaParaVaga.find(placa);
    if(it == placaParaVaga.end()) return -1.0f;  // Não encontrado
    
    Vaga* vaga = it->second;
    Veiculo* veiculo = vaga->getVeiculo();
    
    // 2. Calcular tempo
    auto duracao = std::chrono::duration_cast<std::chrono::minutes>
                  (agora - veiculo->getHoraEntrada());
    
    // 3. Polimorfismo: tarifa automática conforme tipo!
    float valor = veiculo->calcularTarifa(duracao.count());
    
    // 4. Limpar estruturas
    vaga->liberar();
    delete veiculo;
    placaParaVaga.erase(placa);
    
    // 5. Registrar transação
    transacoes.push_back(Transacao(placa, valor, agora));
    
    return valor;
}
```

**Fluxo:** Busca → Calcula → Libera → Registra → Retorna valor

### Métodos Auxiliares

```cpp
int vagasLivres() const;              // Contagem rápida
std::vector<bool> obterMapaVagas();   // Para GUI atualizar
void gerarRelatorioTransacoes() const; // Histórico financeiro
```

---

## Interface Gráfica (Qt)

### Layout Principal

```
┌─────────────────────────────────────┐
│    GERENCIADOR DE ESTACIONAMENTO    │
├─────────────────┬───────────────────┤
│  CONTROLE       │   MAPA DE VAGAS   │
│  ┌───────────┐  │   ┌─┬─┬─┬─┬─┐     │
│  │ Placa: __ │  │   ├─┼─┼─┼─┼─┤     │ 
│  │ Tipo: []  │  │   ├─┼─┼─┼─┼─┤     │
│  │ [Entrada] │  │   └─┴─┴─┴─┴─┘     │
│  │ [Saída]   │  │                   │
│  │           │  │   Verde = Livre   │
│  │ Status:   │  │Vermelho = Ocupado |
│  │ Vagas: 25 │  │                   │
│  └───────────┘  │                   │
└─────────────────┴───────────────────┘
```

### Componentes

- **QLineEdit** — Entrada de placa
- **QPushButton** — Botões (Entrada/Saída)
- **QGridLayout** — 5×10 grid para vagas
- **QLabel** — Status e contador
- **QTableWidget** — Histórico

**Atualização:** Após cada operação, o mapa é redesenhado via `update()`.

---

## Conceitos Chave

| Conceito | Implementação |
|----------|---------------|
| **Encapsulamento** | Atributos privados, acesso via getters/setters |
| **Polimorfismo** | `calcularTarifa()` virtual — cada tipo tem seu cálculo |
| **Herança** | Carro e Moto herdam de Veiculo |
| **Abstração** | Classe Veiculo define contrato, não instanciação direta |
| **Performance** | Hash map garante buscas instantâneas mesmo com 1000s de registros |

---

<div align="center">

[Retroceder](projeto.md) | [Avançar](testes.md)

</div>