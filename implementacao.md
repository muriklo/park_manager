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

A escolha das estruturas de dados corretas foi fundamental para garantir que o sistema não apenas funcione, mas seja rápido e seguro. 

### 1. O Pátio Físico (`std::array`)

```cpp
std::array<Vaga, 80> vagas;  // 50 vagas para Carros e 30 para Motos
```

**Por que escolhemos o array estático?** 
Sabemos que o estacionamento tem um limite físico exato de 80 vagas que não mudará durante a execução. O uso de um `std::array` garante que os endereços de memória de cada vaga sejam fixos. Isso evita problemas graves, como ponteiros apontando para locais vazios (dangling pointers), e permite acesso imediato a qualquer vaga de forma previsível.

### 2. Busca Instantânea de Veículos (`std::unordered_map`)

```cpp
std::unordered_map<std::string, Vaga*> placaParaVaga;
```

**O problema resolvido:**
Buscar um carro estacionado olhando vaga por vaga resultaria em uma operação lenta (O(n)), o que seria prejudicial em momentos de alto fluxo.

**A solução:**
O `unordered_map` funciona como uma tabela de espalhamento (hash). Ao registrar uma placa, o sistema guarda a referência exata de onde aquele veículo está parado. Quando o cliente vai embora, a busca pela placa leva um tempo constante de O(1), ou seja, é instantânea.

### 3. Histórico de Transações (`std::list`)

```cpp
std::list<Transacao> transacoes;
```

**Benefício:** Inserção O(1) nas extremidades, ideal para histórico de cobranças.

### 4. Gestão de Clientes Mensalistas (`std::unordered_map`)

```cpp
std::unordered_map<std::string, Cliente*> clientesMensalistas;
```

**A vantagem do isolamento:**
Separamos o controle de assinaturas do controle físico das vagas. Assim, pelo simples fato de informar a placa, o sistema consegue consultar este mapa em tempo O(1) e descobrir instantaneamente se o veículo pertence a um mensalista e quanto saldo de horas ele ainda possui.

---

## Arquitetura de Classes

Para lidar com diferentes tipos de veículos e formas de cobrança sem criar um código complexo e difícil de manter, aplicamos conceitos clássicos de Orientação a Objetos.

### Hierarquia de Veículos e Polimorfismo

Criamos uma classe base abstrata chamada `Veiculo`. Dela, derivamos `Carro` e `Moto`.

```cpp
// Classe base abstrata
class Veiculo {
protected:
    std::string placa;
    std::chrono::system_clock::time_point horaEntrada;
    
public:
    virtual ~Veiculo() = default;
    virtual float calcularTarifa(int tempoMinutos) = 0;
};

// Especialização: Carro
class Carro : public Veiculo {
public:
    float calcularTarifa(int tempoMinutos) override {
        return (tempoMinutos / 60.0f) * 5.0f; // R$ 5,00 por hora
    }
};

// Especialização: Moto
class Moto : public Veiculo {
private:
    bool paradaNaVagaDeCarro; // Controle de fallback
public:
    Moto(const std::string& p, bool fallbackTarifa = false) : 
        Veiculo(p), paradaNaVagaDeCarro(fallbackTarifa) {}

    float calcularTarifa(int tempoMinutos) override {
        float horas = tempoMinutos / 60.0f;
        return paradaNaVagaDeCarro ? horas * 5.0f : horas * 2.0f;
    }
};
```

**A beleza do Polimorfismo:** 
O sistema principal, ao registrar a saída, não precisa testar se o veículo é um carro ou uma moto. Ele simplesmente chama o método `calcularTarifa()` e a própria instância do veículo calcula e devolve o valor correto. Além disso, a classe `Moto` sabe se deve cobrar o valor normal ou o valor estendido, caso tenha ocupado uma vaga de carro por falta de espaço.

---

## O Gerenciador: Classe Estacionamento

A classe `Estacionamento` é o núcleo que orquestra todas as partes do sistema. Abaixo, descrevemos o ciclo de vida de uma operação padrão.

### Entrada de Veículos

1. **Busca Inteligente:** Utilizamos o padrão Strategy (`EstrategiaAlocacao`) para procurar no array a primeira vaga livre apropriada ao tipo do veículo.
2. **Criação e Fallback:** O veículo é instanciado de acordo com seu tipo. Se uma moto precisar ocupar uma vaga de carro devido à lotação (fallback), o construtor da moto é avisado para reajustar suas regras de tarifa.
3. **Sincronização:** O veículo é alocado na vaga física escolhida e também é registrado no mapa rápido (`placaParaVaga`) para garantir buscas ágeis na saída.

### Saída de Veículos

1. **Localização Rápida:** A placa digitada é consultada no mapa de veículos ativos, e a vaga é identificada de forma instantânea.
2. **Resolução de Tarifas:** O sistema verifica se a placa pertence a um mensalista. Utilizando novamente o padrão Strategy (`TarifaVeiculo`), ele decide de forma limpa se irá descontar horas do saldo do cliente ou realizar uma cobrança em dinheiro.
3. **Finalização:** A vaga é desocupada, o objeto do veículo é removido da memória (`delete` para prevenir memory leaks) e um comprovante é adicionado ao histórico de transações.

---

## Interface Gráfica com Qt

A interface do usuário foi construída para fornecer feedback visual em tempo real, facilitando a vida do operador.

### Estrutura Visual

- **Painel de Controle (Esquerda):** Agrupa os controles interativos: campos de texto para digitar a placa, seleção do tipo do veículo e os botões de execução de entrada e saída.
- **Mapa do Pátio (Direita):** Uma grade (QGridLayout) que representa o estado físico real das 80 vagas do estacionamento. Cada posição se atualiza de acordo com os seguintes estados:
  - **Verde:** Vaga disponível para uso.
  - **Vermelho:** Vaga ocupada de forma padrão.
  - **Amarelo:** Vaga ocupada com regras de fallback (ex: moto ocupando um espaço destinado a carros).

Toda vez que uma operação de entrada ou saída ocorre, a interface redesenha o mapa refletindo as modificações da memória no mesmo instante.

---

## Resumo dos Princípios Adotados

| Conceito | Implementação |
|----------|---------------|
| **Encapsulamento** | Ocultamos o estado das vagas e dos veículos em propriedades privadas, exigindo que qualquer alteração seja feita por métodos seguros e validados. |
| **Polimorfismo** | Funções chave, como `calcularTarifa()`, adaptam perfeitamente seu comportamento em tempo de execução, dependendo de qual objeto as executa. |
| **Herança** | Extraímos propriedades comuns, como placa e horário, para a classe `Veiculo`, eliminando a duplicação de código nas classes derivadas. |
| **Performance e Segurança** | O uso inteligente de Hash Maps e Arrays garante que a aplicação mantenha uma complexidade de busca ótima (O(1)), prevenindo gargalos. |

---

<div align="center">

[Retroceder](projeto.md) | [Avançar](testes.md)

</div>