# Implementação

> Nota Importante: Este documento relata o processo de implementação do sistema utilizando C++, STL e Qt, priorizando a simplicidade e a aplicação correta dos padrões de projeto.

## Ferramentas e Bibliotecas Utilizadas

| Ferramenta | Propósito |
|-----------|----------|
| **C++** | Linguagem base |
| **STL** | Estruturas de dados otimizadas |
| **Qt** | Interface gráfica (GUI) |
| **CMake** | Build system |

---

## Estruturas de Dados (STL)

A escolha correta das estruturas de dados garantiu segurança e previsibilidade na alocação da memória:

- **Array Estático (`std::array`):** Utilizado para o pátio físico do estacionamento, garantindo limite exato de 80 vagas. Isso previne ponteiros perdidos e permite acesso direto a cada posição.
- **Tabelas de Espalhamento (`std::unordered_map`):** Utilizadas para busca instantânea (O(1)) de veículos estacionados através da placa e para isolar o cadastro de clientes mensalistas do controle físico de vagas.
- **Listas (`std::list`):** Utilizada para o histórico de transações, permitindo inserções rápidas nas extremidades do registro (O(1)).

---

## Arquitetura de Classes

Para suportar diferentes formas de cobrança e tipos de veículos sem aumentar a complexidade condicional, a arquitetura foi desenhada em torno do Polimorfismo.

### Hierarquia de Veículos

A classe base abstrata `Veiculo` concentra os dados comuns (placa e horário de entrada). As filhas `Carro` e `Moto` sobrescrevem o método de cálculo de tarifa, sendo que a `Moto` sabe identificar internamente se está estacionada de forma padrão ou ocupando uma vaga de carro (fallback) para aplicar a tarifa correta.

### O Gerenciador: Classe Estacionamento

A classe `Estacionamento` orquestra o ciclo de vida das operações. Suas etapas são definidas de forma simplificada:

1. **Entrada:** A regra de alocação de vaga delega a decisão para uma estratégia independente (padrão Strategy). Ao instanciar o veículo correto, ele é posicionado na vaga selecionada e indexado na tabela de espalhamento para rápida recuperação futura.
2. **Saída:** Após localizar o veículo no mapa, a orquestração resolve a tarifa (comum ou assinatura) aplicando o polimorfismo correto. Finalmente, a memória dinâmica é liberada, a vaga fica livre novamente e a transação financeira é registrada.

---

## Interface Gráfica com Qt

A interface fornece uma visão geral do pátio em tempo real:

- **Painel de Controle:** Responsável pelas entradas textuais (placa) e seleção do tipo de veículo, além de acionar as lógicas de entrada e saída.
- **Mapa Visual:** Uma grade dinâmica reflete a memória física: vagas verdes (livres), vermelhas (ocupadas regularmente) e amarelas (ocupadas por fallback). A atualização da tela é imediatamente acionada após cada transação no estacionamento.

---

## Resumo dos Princípios Adotados

| Conceito | Implementação |
|----------|---------------|
| **Encapsulamento** | Oculta e protege o estado interno das vagas e veículos, forçando alterações via métodos seguros. |
| **Polimorfismo** | Funções de cálculo de tarifas se autoajustam conforme a instância do veículo, eliminando verificações manuais de tipo. |
| **Herança** | Propriedades vitais (como a data de entrada) ficam no nível-mãe, cortando código duplicado. |
| **Performance e Segurança** | O pareamento de Arrays e Hash Maps mantém todas as operações de busca constantes, prevenindo lentidão com pátios cheios. |

---

## Justificativas de Design (SOLID)

- **Princípio da Responsabilidade Única (SRP):** A separação entre veículo e cliente garante que propriedades financeiras (saldo de assinaturas) fiquem em objetos independentes da lógica de armazenamento físico do carro.
- **Princípio Aberto/Fechado (OCP):** A tarifação ocorre por polimorfismo em vez de condicionais (`if/else`). Novas modalidades de cobrança podem ser adicionadas criando classes separadas, mantendo o sistema existente inalterado.

## Padrões de Projeto Adicionais

- **Strategy:** Permite definir ou trocar a regra para localização de vagas vazias dinamicamente (Primeira livre, Próxima à entrada, Exclusiva para mensalistas, etc).
- **Template Method e Decorator:** Aplicáveis como camadas adicionais na alocação de vagas para centralizar passos sequenciais comuns ou adicionar restrições (ex: vagas para deficientes).
- **Observer:** Estruturado para lidar com eventos colaterais de forma desacoplada, emitindo sinais de saldo baixo sem poluir a lógica de estacionamento.

## Funcionalidades Práticas

- **Relatórios:** Mecanismo padronizado de verificação que confere horas consumidas e saldo ainda ativo de clientes.
- **Testes Unitários:** Isola comportamentos específicos, testando se cálculos de redução de mensalidade e locação de índice no array funcionam de maneira robusta.

---

<div align="center">

[Retroceder](projeto.md) | [Avançar](testes.md)

</div>