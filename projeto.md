# Arquivos do Projeto

Para facilitar o entendimento, aqui explicamos qual é a responsabilidade de cada arquivo dentro do nosso código:

## 🚗 Veículos
- **Veiculo.h / .cpp**: Guarda informações comuns, como a placa e o horário exato que o veículo entrou.
- **Carro.h / .cpp**: Regras específicas para carros. Ensina o sistema a calcular o preço da estadia de um carro.
- **Moto.h / .cpp**: Regras específicas para motos. Ensina o sistema a calcular o preço da estadia de uma moto.

## 🅿️ Controle do Estacionamento
- **Estacionamento.h / .cpp**: Registra as entradas e saídas, gerencia todas as vagas e conecta as outras partes do código.
- **Vaga.h**: Sabe o tipo da vaga, se ela está ocupada ou livre, e qual veículo está parado nela.
- **EstrategiaAlocacao.h / .cpp**: Define as regras de como o sistema deve procurar uma vaga livre para colocar um carro ou moto que acabou de chegar.

## 👥 Mensalistas e Assinaturas
- **Cliente.h / .cpp**: Cadastra uma pessoa no sistema. Guarda o nome, a placa e se ela tem um plano mensal.
- **Assinatura.h / .cpp**: Gerencia o banco de horas de um cliente mensalista. Controla qual é o plano dele, quantas horas ele tem direito e quantas já gastou no mês.

## 💰 Cobrança e Histórico
- **TarifaVeiculo.h / .cpp**: Separa a cobrança de um cliente comum (que paga em dinheiro) da cobrança de um cliente mensalista (que desconta das horas da assinatura).
- **Transacao.h / .cpp**: Guarda o que aconteceu (placa, data, hora e valor pago).

## 🖥️ Interface Gráfica e Inicialização
- **MainWindow.h / .cpp**: Cuida do visual (telas, botões, mapa das vagas, menus).
- **main.cpp**: Inicia o sistema.

## 📊 Diagrama de Classes

De acordo com as boas práticas das aulas de UML (composição, agregação, herança e dependência), segue o Diagrama de Classes principal do projeto:

```plantuml
@startuml
skinparam classAttributeIconSize 0

abstract class Veiculo {
  # placa : string
  # horaEntrada : time_point
  + calcularTarifa(tempoMinutos: int) : float
  + obterTipo() : string
}

class Carro {
  + calcularTarifa(tempoMinutos: int) : float
  + obterTipo() : string
}

class Moto {
  - tarifaDeCarro : bool
  + calcularTarifa(tempoMinutos: int) : float
  + obterTipo() : string
}

Veiculo <|-- Carro
Veiculo <|-- Moto

class Vaga {
  - numero : int
  - tipoVaga : int
  - ocupada : bool
  + alocar(v: Veiculo)
  + liberar() : Veiculo
}

Vaga o-- "0..1" Veiculo : veiculo

abstract class EstrategiaAlocacao {
  + encontrarVaga(vagas: array, placa: string, tipo: int) : Vaga
}

class EstrategiaVagaLivre {
  + encontrarVaga(vagas: array, placa: string, tipo: int) : Vaga
}

EstrategiaAlocacao <|-- EstrategiaVagaLivre

class Assinatura {
  - tipo : Plano
  - horasDisponiveis : int
  - horasUsadas : int
  + consumirHoras(horas: int)
}

class Cliente {
  - placa : string
  - nome : string
  - isMensalista : bool
}

Cliente *-- "1" Assinatura : assinatura

abstract class TarifaVeiculo {
  + calcularCusto(tempoMinutos: int) : float
}

class TarifaComum {
  - taxaHora : float
  + calcularCusto(tempoMinutos: int) : float
}

class TarifaMensalista {
  + calcularCusto(tempoMinutos: int) : float
}

TarifaVeiculo <|-- TarifaComum
TarifaVeiculo <|-- TarifaMensalista
TarifaMensalista --> "1" Cliente : cliente

class Transacao {
  - placa : string
  - tipoVeiculo : string
  - valorPago : float
  - tempoMinutos : int
}

class Estacionamento {
  + entradaVeiculo(placa: string, tipo: int) : string
  + saidaVeiculo(placa: string) : string
}

Estacionamento *-- "80" Vaga : vagas
Estacionamento *-- "1" EstrategiaAlocacao : estrategia
Estacionamento *-- "*" Cliente : mensalistas
Estacionamento *-- "*" Transacao : transacoes

@enduml
```