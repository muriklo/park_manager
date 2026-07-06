/**
 * @file Veiculo.h
 * @brief Classe base abstrata para representar veículos no estacionamento.
 */
#ifndef VEICULO_H
#define VEICULO_H

#include <string>
#include <chrono>

// Classe base abstrata para veículos
class Veiculo {
protected:
    std::string placa;
    std::chrono::system_clock::time_point horaEntrada;

public:
    Veiculo(const std::string& p);
    virtual ~Veiculo() = default;

    std::string obterPlaca() const;
    std::chrono::system_clock::time_point obterHoraEntrada() const;

    virtual float calcularTarifa(int tempoMinutos) const = 0;
    virtual std::string obterTipo() const = 0;
};

#endif // VEICULO_H
