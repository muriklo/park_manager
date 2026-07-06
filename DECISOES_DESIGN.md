# Decisões de Design — Resumo das Escolhas

> Documento com as principais decisões de design e suas justificativas

---

## 🎯 Problema: Como Representar Clientes Mensalistas?

### ❌ Abordagem 1: Adicionar flag ao Veiculo

```cpp
class Veiculo {
    bool isMensalista;  // ❌ Problema: tipo de dado incorreto
    float saldoHoras;   // Negócio de cliente, não de veículo!
};
```

**Problemas:**
- Veiculo fica com responsabilidades de Cliente
- Um veículo não pode ter saldo — Cliente tem saldo
- Violação do SRP (Single Responsibility Principle)

---

### ✅ Abordagem 2: Criar classe Cliente (RECOMENDADO)

```cpp
class Cliente {
    std::string placa;
    Assinatura assinatura;
    bool isMensalista;
};

class Assinatura {
    enum Plano { PLANO_30H, PLANO_50H, ... };
    int horasDisponiveis;
    int horasUsadas;
};
```

**Vantagens:**
- ✅ Separação clara: Veiculo = instância física no estacionamento
- ✅ Cliente = dados de negócio (saldo, assinatura, histórico)
- ✅ Um Cliente pode ter múltiplos Veiculos (ex: dois carros)
- ✅ Fácil adicionar: endereço, email, telefone, etc

---

## 🎯 Problema: Como Calcular Tarifas Diferentes?

### ❌ Abordagem 1: If-Else na Saída

```cpp
float registrarSaida(std::string placa) {
    if(clientesMensalistas.find(placa)) {
        if(tipo_assinatura == PLANO_100H) {
            // ...
        } else if(tipo_assinatura == PLANO_50H) {
            // ...
        }
    } else {
        if(tipo_veiculo == CARRO) {
            // ...
        } else if(tipo_veiculo == MOTO) {
            // ...
        }
    }
}  // ❌ Código cresce exponencialmente!
```

**Problemas:**
- ❌ Violação do Open/Closed Principle (aberto para extensão, fechado para modificação)
- ❌ Novo tipo de tarifa = modificar função existente
- ❌ Difícil testar cada caso

---

### ✅ Abordagem 2: Hierarquia Polimórfica (RECOMENDADO)

```cpp
class TarifaVeiculo {
public:
    virtual float calcularCusto(int tempoMinutos) = 0;
};

class TarifaComum : public TarifaVeiculo {
    float calcularCusto(int tempoMinutos) override {
        return (tempoMinutos / 60.0f) * taxa;
    }
};

class TarifaMensalista : public TarifaVeiculo {
    float calcularCusto(int tempoMinutos) override {
        // Lógica específica
    }
};

// Na saída:
TarifaVeiculo* tarifa = selecionarTarifa(placa);
float valor = tarifa->calcularCusto(tempoMinutos);  // ✅ Polimorfismo
```

**Vantagens:**
- ✅ Novo tipo de tarifa = nova classe (não modifica código existente)
- ✅ Cada tarifa encapsula sua lógica
- ✅ Fácil testar em isolamento
- ✅ Segue Open/Closed Principle

---

## 🎯 Problema: Como Alocar Vagas?

### ❌ Abordagem 1: Hardcoding no Estacionamento

```cpp
bool registrarEntrada(std::string placa, int tipo) {
    // Procura primeira vaga livre
    for(auto& vaga : vagas) {
        if(!vaga.estaOcupada()) {
            alocar(vaga, placa);
            return true;
        }
    }
    return false;
}
```

**Problema:**
- ❌ Se quiser mudar a estratégia (ex: vaga mais próxima), alterar código
- ❌ Sem flexibilidade
- ❌ Difícil testar estratégias diferentes

---

### ✅ Abordagem 2A: Strategy Pattern (RECOMENDADO para Alocação)

```cpp
class EstrategiaAlocacao {
public:
    virtual Vaga* encontrarVaga(std::array<Vaga, 50>& vagas) = 0;
};

class EstrategiaVagaLivre : public EstrategiaAlocacao {
    Vaga* encontrarVaga(std::array<Vaga, 50>& vagas) override {
        for(auto& v : vagas) {
            if(!v.estaOcupada()) return &v;
        }
        return nullptr;
    }
};

class Estacionamento {
private:
    std::unique_ptr<EstrategiaAlocacao> estrategia;
    std::array<Vaga, 50> vagas;
    
public:
    void definirEstrategia(std::unique_ptr<EstrategiaAlocacao> nova) {
        estrategia = std::move(nova);
    }
    
    bool registrarEntrada(...) {
        Vaga* vaga = estrategia->encontrarVaga(vagas);  // ✅ Delega
        // ...
    }
};
```

**Vantagens:**
- ✅ Trocar estratégia em runtime
- ✅ Fácil testar cada estratégia
- ✅ Sem modificar Estacionamento

**Uso:**
```cpp
estacionamento.definirEstrategia(std::make_unique<EstrategiaVagaLivre>());
estacionamento.definirEstrategia(std::make_unique<EstrategiaProxima>());
estacionamento.definirEstrategia(std::make_unique<EstrategiaAlternada>());
```

---

### ✅ Abordagem 2B: Template Method (Alternativa)

```cpp
class AlocadorVaga {
public:
    Vaga* alocar(std::array<Vaga, 50>& vagas) {
        Vaga* vaga = selecionarVaga(vagas);  // Hook
        if(vaga) preparar(vaga);              // Hook
        return vaga;
    }
    
protected:
    virtual Vaga* selecionarVaga(std::array<Vaga, 50>&) = 0;
    virtual void preparar(Vaga*) {}  // Opcional
};
```

**Melhor para:** Reutilizar passos comuns

---

## 🏆 Matriz de Decisão

| Situação | Padrão | Razão |
|----------|--------|-------|
| **Múltiplas formas de calcular tarifa** | Strategy + Polimorfismo | Extensível sem modificar código |
| **Cliente com diferentes planos** | Classe Cliente + Assinatura | Separação: Veiculo ≠ Negócio |
| **Trocar estratégia em runtime** | Strategy | Flexibilidade máxima |
| **Reutilizar passos comuns** | Template Method | DRY (Don't Repeat Yourself) |
| **Adicionar comportamentos** | Decorator | Composição over Herança |
| **Notificar múltiplos observadores** | Observer | Desacoplamento |

---

## 📊 Evolução do Projeto

### Fase 1: MVP (Mínimo Viável)
```
Veiculo (abstrato)
├── Carro
└── Moto

Estacionamento
├── array<Vaga, 50>
└── map<placa, Vaga*>
```

### Fase 2: Clientes Mensalistas
```
+ Cliente
+ Assinatura
+ TarifaVeiculo (abstrato)
  ├── TarifaComum
  └── TarifaMensalista
```

### Fase 3: Alocação Flexível
```
+ EstrategiaAlocacao (abstrato)
  ├── EstrategiaVagaLivre
  ├── EstrategiaProxima
  ├── EstrategiaAlternada
  └── EstrategiaClienteEspecifico
```

### Fase 4: Notificações (Futuro)
```
+ ObservadorEstacionamento (abstrato)
  ├── NotificadorSaldo
  ├── NotificadorEmail
  └── NotificadorSMS
```

---

## ✅ Checklist de Boas Práticas

- [ ] **SRP:** Cada classe tem uma única responsabilidade
- [ ] **OCP:** Código aberto para extensão, fechado para modificação
- [ ] **LSP:** Subclasses substituem base transparentemente
- [ ] **ISP:** Clientes usam apenas interfaces que precisam
- [ ] **DIP:** Depender de abstrações, não de concretudes
- [ ] **Testes:** Cada padrão é testável isoladamente
- [ ] **Documentação:** Código é auto-explicativo
- [ ] **Performance:** Sem overhead desnecessário

---

## 🚀 Próximos Passos Recomendados

1. **Implementar Cliente + Assinatura** (impacto: ALTO, dificuldade: MÉDIA)
2. **Implementar TarifaVeiculo hierarchy** (impacto: ALTO, dificuldade: BAIXA)
3. **Adicionar EstrategiaAlocacao** (impacto: MÉDIO, dificuldade: MÉDIA)
4. **Implementar testes unitários** (impacto: MÉDIO, dificuldade: BAIXA)
5. **Adicionar persistência (BD)** (impacto: ALTO, dificuldade: ALTA)

---

<div align=center>

[← Voltar](ARQUITETURA_REFINADA.md) | [Próximo](IMPLEMENTACAO_PRATICA.md)

</div>
