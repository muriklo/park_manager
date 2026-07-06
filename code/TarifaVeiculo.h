/**
 * @file TarifaVeiculo.h
 * @brief Definição da interface TarifaVeiculo e suas implementações para cálculo de custos.
 */
#ifndef TARIFA_VEICULO_H
#define TARIFA_VEICULO_H

#include "Cliente.h"

class TarifaVeiculo {
public:
    virtual ~TarifaVeiculo() = default;
    virtual float calcularCusto(int tempoMinutos) = 0;
};

class TarifaComum : public TarifaVeiculo {
private:
    float taxaHora;

public:
    TarifaComum(float taxaPorHora);
    float calcularCusto(int tempoMinutos) override;
};

class TarifaMensalista : public TarifaVeiculo {
private:
    Cliente* cliente;

public:
    TarifaMensalista(Cliente* c);
    float calcularCusto(int tempoMinutos) override;
};

#endif // TARIFA_VEICULO_H
