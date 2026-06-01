<img src="img/ifsc-logo.png"
     width="30%"
     style="padding: 10px">

# Sistema de Gerenciamento de Estacionamento

## Controle em Tempo Real de Vagas e Tarifas

> Projeto final da disciplina PRG22107 - Programação Orientada a Objeto  
> **Aluno:** Murillo Ribeiro | **Professor:** Hugo Marcondes

---

## Visão Geral

Um sistema que gerencia automaticamente entradas, saídas e cobranças em um estacionamento. O operador registra a placa do veículo, o sistema aloca a primeira vaga disponível e, ao sair, calcula a tarifa de forma polimórfica conforme o tipo (carro ou moto).

### Fluxo Básico

```
Veículo Chega → Busca Vaga Livre → Aloca Automaticamente → Mapa Atualiza
                                                     ↓
                               Calcula Tarifa ← Veículo Sai 
                                                     ↓
                                              Registra Transação
```

## Principais Características

| Aspecto | Descrição |
|---------|-----------|
| **Funcionalidade** | Entrada/saída automática com tarifas diferenciadas |
| **Tecnologia** | C++, STL (vector, unordered_map), Qt (GUI) |
| **Performance** | Busca de veículos em O(1) via hash map |
| **Capacidade** | 50 vagas com mapa visual em tempo real |

## Componentes Principais

- **Veiculo** (abstrato) → **Carro** e **Moto** (especializações com tarifas próprias)
- **Vaga** → Representa cada espaço físico (livre/ocupado)
- **Estacionamento** → Gerenciador central (entradas, saídas, buscas)
- **Transação** → Histórico de cobranças

---

<p align=center><strong>DOCUMENTAÇÃO</strong></p>

[**1. ANÁLISE ORIENTADA A OBJETO**](./analise.md)<br>
[**2. PROJETO ORIENTADO A OBJETO**](./projeto.md)<br>
[**3. IMPLEMENTAÇÃO (C++)**](./implementacao.md)<br>
[**4. TESTES**](./testes.md)<br>