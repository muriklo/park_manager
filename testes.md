# Testes

> Relatório do processo de testes do sistema, especificando os testes realizados e seus resultados.

## Testes Implementados

O projeto inclui testes em múltiplas camadas para garantir a qualidade do sistema:

### 1. Testes em Python (teste_conceito.py)

Este arquivo valida os conceitos principais do sistema com testes automatizados:

1. **Registrar Entrada (Carro e Moto)** - Verifica se o sistema aceita corretamente novos veículos
2. **Rejeitar Placa Duplicada** - Garante que placas já estacionadas não podem entrar novamente
3. **Calcular Tarifa com Polimorfismo** - Valida o cálculo correto de tarifas diferenciadas por tipo
4. **Rejeitar Veículo Não Encontrado** - Confirma tratamento de erro para placas inexistentes
5. **Visualizar Mapa de Vagas** - Testa a exibição do estado das vagas
6. **Liberar Vaga após Saída** - Verifica a correta liberação de vagas
7. **Gerar Relatório de Transações** - Valida o histórico de transações

### 2. Testes em C++ (Console)

O arquivo main.cpp implementa os mesmos 7 testes em C++ para validação da lógica em tempo de execução do programa compilado.

### 3. Testes da Interface Gráfica (Qt)

A interface permite testes manuais da aplicação:

- Grid visual 5x10 representando as 50 vagas
- Código de cores: Verde (livre) e Vermelho (ocupada)
- Entrada/saída de veículos em tempo real
- Visualização instantânea do mapa de vagas
- Relatório de transações atualizado

## Cobertura de Testes

| Cenário | Status |
|---------|--------|
| Entrada de Carro | Implementado |
| Entrada de Moto | Implementado |
| Rejeição de Duplicata | Implementado |
| Cálculo de Tarifa | Implementado |
| Tratamento de Erro | Implementado |
| Visualização de Mapa | Implementado |
| Liberação de Vaga | Implementado |
| Relatório de Transações | Implementado |

## Resultado Final

Todos os 7 cenários de teste foram implementados e validados com sucesso, cobrindo:

- Casos de uso normais (happy path)
- Casos de erro (sad path)
- Requisitos não-funcionais de performance e usabilidade
- Integração entre componentes

<div align="center">

[Retroceder](implementacao.md) | [Início](README.md)

</div>