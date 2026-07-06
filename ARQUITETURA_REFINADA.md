# Arquitetura Refinada: Clientes e Polimorfismo Avançado

> Documento de design explorando alternativas de polimorfismo para o sistema de estacionamento

---

## 📋 Parte 1: Sistema de Clientes Mensalistas

### Hierarquia de Cobranças

```
TarifaVeiculo (abstrato)
├── TarifaComum (por hora: R$ 5 carro, R$ 2 moto)
└── TarifaMensalista (desconta horas da assinatura)
```

### Assinatura Mensal

```cpp
class Assinatura {
public:
    enum Plano {
        PLANO_30H = 30,
        PLANO_50H = 50,
        PLANO_100H = 100,
        PLANO_250H = 250
    };
    
private:
    Plano tipo;
    int horasDisponiveis;
    int horasUsadas;
    
public:
    Assinatura(Plano p) : tipo(p), horasDisponiveis(p), horasUsadas(0) {}
    
    bool temHorasDisponiveis(int horas) const {
        return (horasDisponiveis - horasUsadas) >= horas;
    }
    
    int horasRestantes() const {
        return horasDisponiveis - horasUsadas;
    }
    
    void consumirHoras(int horas) {
        horasUsadas += horas;
    }
    
    void resetarMes() {
        horasUsadas = 0;
    }
};
```

### Cliente

```cpp
class Cliente {
private:
    std::string placa;
    std::string nome;
    Assinatura assinatura;
    bool isMensalista;
    
public:
    Cliente(const std::string& p, const std::string& n, 
            Assinatura::Plano plano)
        : placa(p), nome(n), assinatura(plano), isMensalista(true) {}
    
    Cliente(const std::string& p)  // Cliente não-assinante
        : placa(p), nome(""), assinatura(Assinatura::PLANO_30H), 
          isMensalista(false) {}
    
    bool ehMensalista() const { return isMensalista; }
    std::string obterPlaca() const { return placa; }
    Assinatura& obterAssinatura() { return assinatura; }
};
```

---

## 🎯 Polimorfismo de Cobrança: Opção 1 (RECOMENDADO)

### Estratégia 1A: Hierarquia de Tarifas

```cpp
class TarifaVeiculo {
public:
    virtual ~TarifaVeiculo() = default;
    virtual float calcularCusto(int tempoMinutos) = 0;
    virtual std::string obterDescricao() const = 0;
};

class TarifaComum : public TarifaVeiculo {
private:
    float valorPorHora;  // R$ 5 ou R$ 2
    
public:
    TarifaComum(float vph) : valorPorHora(vph) {}
    
    float calcularCusto(int tempoMinutos) override {
        float horas = tempoMinutos / 60.0f;
        return horas * valorPorHora;
    }
    
    std::string obterDescricao() const override {
        return "Tarifa por hora";
    }
};

class TarifaMensalista : public TarifaVeiculo {
private:
    Cliente* cliente;
    
public:
    TarifaMensalista(Cliente* c) : cliente(c) {}
    
    float calcularCusto(int tempoMinutos) override {
        int horas = tempoMinutos / 60;
        Assinatura& asin = cliente->obterAssinatura();
        
        if(asin.temHorasDisponiveis(horas)) {
            asin.consumirHoras(horas);
            return 0.0f;  // Grátis, usa do saldo
        } else {
            // Cobra o excedente
            int horasExcedentes = horas - asin.horasRestantes();
            return horasExcedentes * 5.0f;  // Preço penalidade
        }
    }
    
    std::string obterDescricao() const override {
        return "Tarifa mensal (saldo de horas)";
    }
};
```

### Uso na Saída

```cpp
float Estacionamento::registrarSaida(const std::string& placa) {
    auto it = placaParaVaga.find(placa);
    if(it == placaParaVaga.end()) return -1.0f;
    
    Vaga* vaga = it->second;
    Veiculo* veiculo = vaga->getVeiculo();
    
    // Calcular tempo
    auto duracao = std::chrono::duration_cast<std::chrono::minutes>
                  (agora - veiculo->getHoraEntrada());
    
    // POLIMORFISMO: Usa tarifa apropriada
    TarifaVeiculo* tarifa = obterTarifa(placa);  // Busca tarifa do cliente
    float valor = tarifa->calcularCusto(duracao.count());
    
    // ... resto do código
    
    return valor;
}

// Método auxiliar
TarifaVeiculo* Estacionamento::obterTarifa(const std::string& placa) {
    auto it = clientes.find(placa);
    if(it != clientes.end() && it->second->ehMensalista()) {
        return new TarifaMensalista(it->second);
    }
    return new TarifaComum(5.0f);  // Valor default para carros
}
```

### Vantagem
✅ Fácil adicionar novos tipos de tarifa (TarifaGrupoParceria, TarifaSazonal, etc)  
✅ Separação clara de responsabilidades  
✅ Cada tarifa independente

---

## 🎯 Polimorfismo de Cobrança: Opção 2

### Estratégia 2: State Pattern (Estado da Cobrança)

```cpp
class EstadoCobranca {
public:
    virtual ~EstadoCobranca() = default;
    virtual float calcular(int tempoMinutos, void* contexto) = 0;
};

class CobrancaPorHora : public EstadoCobranca {
private:
    float taxa;
public:
    CobrancaPorHora(float t) : taxa(t) {}
    float calcular(int tempoMinutos, void* contexto) override {
        return (tempoMinutos / 60.0f) * taxa;
    }
};

class CobrancaMensalista : public EstadoCobranca {
public:
    float calcular(int tempoMinutos, void* contexto) override {
        Cliente* cliente = static_cast<Cliente*>(contexto);
        int horas = tempoMinutos / 60;
        
        if(cliente->obterAssinatura().temHorasDisponiveis(horas)) {
            cliente->obterAssinatura().consumirHoras(horas);
            return 0.0f;
        }
        return 0.0f;  // Implementar lógica de excedente
    }
};
```

### Vantagem
✅ Permite trocar estratégia em runtime  
✅ Útil se cliente mudar de plano durante o mês

---

## 🎯 Polimorfismo de Alocação de Vagas: Diferentes Estratégias

### Opção 1: Strategy Pattern — Estratégias de Busca

```cpp
class EstrategiaAlocacao {
public:
    virtual ~EstrategiaAlocacao() = default;
    virtual Vaga* encontrarVaga(std::array<Vaga, 50>& vagas, 
                                const std::string& placa) = 0;
    virtual std::string obterNome() const = 0;
};

// Estratégia 1: Primeira vaga livre (padrão)
class EstrategiaVagaLivre : public EstrategiaAlocacao {
public:
    Vaga* encontrarVaga(std::array<Vaga, 50>& vagas, 
                       const std::string& placa) override {
        for(auto& vaga : vagas) {
            if(!vaga.estaOcupada()) return &vaga;
        }
        return nullptr;
    }
    std::string obterNome() const override { return "Primeira vaga livre"; }
};

// Estratégia 2: Vaga mais próxima da entrada (números menores)
class EstrategiaProxima : public EstrategiaAlocacao {
public:
    Vaga* encontrarVaga(std::array<Vaga, 50>& vagas, 
                       const std::string& placa) override {
        for(int i = 0; i < vagas.size(); i++) {
            if(!vagas[i].estaOcupada()) return &vagas[i];
        }
        return nullptr;  // Mesma lógica, mas conceito diferente
    }
    std::string obterNome() const override { return "Vaga próxima"; }
};

// Estratégia 3: Alternado (par/ímpar por dia)
class EstrategiaAlternada : public EstrategiaAlocacao {
private:
    bool usarPares;  // true = par, false = ímpar
public:
    EstrategiaAlternada() : usarPares(verificarDia()) {}
    
    Vaga* encontrarVaga(std::array<Vaga, 50>& vagas, 
                       const std::string& placa) override {
        for(auto& vaga : vagas) {
            if(!vaga.estaOcupada()) {
                bool ehPar = (vaga.getNumero() % 2 == 0);
                if(usarPares == ehPar) return &vaga;
            }
        }
        // Fallback: pega primeira disponível
        for(auto& vaga : vagas) {
            if(!vaga.estaOcupada()) return &vaga;
        }
        return nullptr;
    }
    
    std::string obterNome() const override { return "Vaga alternada"; }
    
private:
    bool verificarDia() {
        auto tempo = std::time(nullptr);
        struct tm* timeinfo = std::localtime(&tempo);
        return (timeinfo->tm_mday % 2 == 0);
    }
};

// Estratégia 4: Vaga por tipo de cliente (Mensalista vs Comum)
class EstrategiaClienteEspecifico : public EstrategiaAlocacao {
private:
    std::unordered_map<std::string, Cliente*>& clientes;
    // Vagas 1-25: Mensalistas | Vagas 26-50: Comuns
    
public:
    EstrategiaClienteEspecifico(std::unordered_map<std::string, Cliente*>& c) 
        : clientes(c) {}
    
    Vaga* encontrarVaga(std::array<Vaga, 50>& vagas, 
                       const std::string& placa) override {
        auto it = clientes.find(placa);
        int inicio = 0, fim = vagas.size();
        
        if(it != clientes.end() && it->second->ehMensalista()) {
            // Mensalistas: vagas 0-24
            fim = 25;
        } else {
            // Comuns: vagas 25-49
            inicio = 25;
        }
        
        for(int i = inicio; i < fim; i++) {
            if(!vagas[i].estaOcupada()) return &vagas[i];
        }
        
        // Se lotado na seção preferida, procura na outra
        int inicio2 = (inicio == 0) ? 25 : 0;
        int fim2 = (inicio == 0) ? 50 : 25;
        for(int i = inicio2; i < fim2; i++) {
            if(!vagas[i].estaOcupada()) return &vagas[i];
        }
        
        return nullptr;
    }
    
    std::string obterNome() const override { 
        return "Vaga por tipo de cliente"; 
    }
};
```

### Uso na Entrada

```cpp
class Estacionamento {
private:
    std::unique_ptr<EstrategiaAlocacao> estrategia;
    std::array<Vaga, 50> vagas;
    std::unordered_map<std::string, Vaga*> placaParaVaga;
    
public:
    Estacionamento() : estrategia(std::make_unique<EstrategiaVagaLivre>()) {}
    
    void definirEstrategia(std::unique_ptr<EstrategiaAlocacao> nova) {
        estrategia = std::move(nova);
    }
    
    bool registrarEntrada(const std::string& placa, int tipo) {
        // Usar estratégia polimórfica
        Vaga* vagaLivre = estrategia->encontrarVaga(vagas, placa);
        
        if(!vagaLivre) return false;  // Lotado
        
        Veiculo* veiculo = (tipo == 0) ? 
            new Carro(placa) : new Moto(placa);
        
        vagaLivre->alocar(veiculo);
        placaParaVaga[placa] = vagaLivre;
        
        return true;
    }
};
```

### Vantagem
✅ Trocar estratégia em runtime: `estacionamento->definirEstrategia(new EstrategiaAlternada())`  
✅ Fácil testar cada estratégia  
✅ Sem if-else gigante

---

## 🎯 Polimorfismo de Alocação: Opção 2 — Template Method

```cpp
class AlocadorVaga {
public:
    virtual ~AlocadorVaga() = default;
    
    // Template method
    Vaga* alocar(std::array<Vaga, 50>& vagas, const std::string& placa) {
        Vaga* vaga = selecionarVaga(vagas);
        if(vaga) {
            preparar(vaga);  // Hook customizável
        }
        return vaga;
    }
    
protected:
    virtual Vaga* selecionarVaga(std::array<Vaga, 50>& vagas) = 0;
    virtual void preparar(Vaga* vaga) {}  // Hook
};

class AlocadorSimples : public AlocadorVaga {
protected:
    Vaga* selecionarVaga(std::array<Vaga, 50>& vagas) override {
        for(auto& v : vagas) {
            if(!v.estaOcupada()) return &v;
        }
        return nullptr;
    }
};

class AlocadorComLog : public AlocadorVaga {
protected:
    Vaga* selecionarVaga(std::array<Vaga, 50>& vagas) override {
        for(auto& v : vagas) {
            if(!v.estaOcupada()) return &v;
        }
        return nullptr;
    }
    
    void preparar(Vaga* vaga) override {
        // Log de auditoria
        std::cout << "Vaga " << vaga->getNumero() << " alocada" << std::endl;
    }
};
```

### Vantagem
✅ Cada subclasse define apenas o que é diferente  
✅ Reutiliza lógica comum

---

## 🎯 Polimorfismo de Alocação: Opção 3 — Decorator Pattern

```cpp
class VagaDecorator : public Vaga {
protected:
    Vaga* vagaInterna;
    
public:
    VagaDecorator(Vaga* v) : vagaInterna(v) {}
    virtual ~VagaDecorator() { delete vagaInterna; }
    
    void alocar(Veiculo* veiculo) override {
        vagaInterna->alocar(veiculo);
    }
    
    bool estaOcupada() const override {
        return vagaInterna->estaOcupada();
    }
};

class VagaComPrioridade : public VagaDecorator {
private:
    int nivelPrioridade;
    
public:
    VagaComPrioridade(Vaga* v, int nivel) 
        : VagaDecorator(v), nivelPrioridade(nivel) {}
    
    int obterPrioridade() const { return nivelPrioridade; }
};

class VagaAcessivel : public VagaDecorator {
public:
    VagaAcessivel(Vaga* v) : VagaDecorator(v) {}
    bool ehAcessivel() const { return true; }
};
```

### Vantagem
✅ Adiciona comportamentos a vagas existentes  
✅ Composição over herança

---

## 📊 Comparação de Estratégias

| Estratégia | Flexibilidade | Complexidade | Uso Ideal |
|-----------|---------------|--------------|-----------|
| **Strategy (Alocação)** | Alta | Média | Trocar em runtime |
| **Template Method** | Média | Baixa | Reutilizar lógica comum |
| **Decorator** | Alta | Alta | Combinar comportamentos |
| **Tarifa Hierarquia** | Alta | Média | Múltiplos tipos de cobrança |
| **State** | Alta | Alta | Mudanças de estado complexas |

---

## 🏗️ Estrutura Refinada Completa

```
Veiculo (abstrato)
├── Carro
└── Moto

TarifaVeiculo (abstrato)
├── TarifaComum
└── TarifaMensalista

Cliente
└── Assinatura (30h, 50h, 100h, 250h)

EstrategiaAlocacao (abstrato)
├── EstrategiaVagaLivre
├── EstrategiaProxima
├── EstrategiaAlternada
└── EstrategiaClienteEspecifico

Estacionamento (usa 1+ estratégias)
├── array<Vaga, 50>
├── map<placa, Vaga*>
├── list<Transacao>
└── map<placa, Cliente*>
```

---

<div align=center>

[← Voltar](implementacao.md)

</div>
