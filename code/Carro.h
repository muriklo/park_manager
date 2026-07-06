/**
 * @file Carro.h
 * @brief Definição da classe Carro, derivada de Veiculo, com tarifa específica.
 */
#ifndef CARRO_H
#define CARRO_H

#include "Veiculo.h"

// Carro: R$ 5.00/h
class Carro : public Veiculo {
public:
    Carro(const std::string& p) : Veiculo(p) {}

    float calcularTarifa(int tempoMinutos) const override;
    std::string obterTipo() const override;
};

#endif // CARRO_H
