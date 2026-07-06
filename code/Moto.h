/**
 * @file Moto.h
 * @brief Definição da classe Moto, derivada de Veiculo, com tarifa específica e fallback para carro.
 */
#ifndef MOTO_H
#define MOTO_H

#include "Veiculo.h"

// Moto: R$ 2.00/h (suporta tarifa de carro como fallback)
class Moto : public Veiculo {
private:
    bool tarifaDeCarro;

public:
    Moto(const std::string& p, bool tarifaDeCarro = false) 
        : Veiculo(p), tarifaDeCarro(tarifaDeCarro) {}

    float calcularTarifa(int tempoMinutos) const override;
    std::string obterTipo() const override;
};

#endif // MOTO_H
