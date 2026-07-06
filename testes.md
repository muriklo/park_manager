# Testes

> Nota Importante: Resumo dos principais testes de validação do sistema.

## 1. Teste de lotação e clientes (`SIM-0000`)
- **Ação:** Tentar cadastrar a placa especial `SIM-0000`.
- **Resultado:** O sistema injeta 3 mensalistas e preenche quase todas as vagas (48 carros e 28 motos) instantaneamente. O mapa visual renderiza tudo corretamente sem travar.

## 2. Teste de fallback de vagas (Motos)
- **Ação:** Lotar todas as 30 vagas de moto e tentar registrar a entrada de mais uma moto.
- **Resultado:** A moto extra é alocada em uma vaga de carro (ficando amarela no mapa) e, na saída, o sistema cobra a tarifa mais cara de carro (R$ 5,00/h).

## 3. Teste de mensalistas e banco de horas
- **Ação:** Registrar a saída de um veículo pertencente a um mensalista com saldo positivo.
- **Resultado:** A transação cobra `R$ 0,00` em dinheiro e desconta o tempo gasto diretamente do saldo de horas do pacote do cliente.

---

<div align="center">

[Retroceder](implementacao.md) | [Início](README.md)

</div>