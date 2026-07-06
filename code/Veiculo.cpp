/**
 * @file Veiculo.cpp
 * @brief Implementação dos métodos comuns a todos os veículos.
 */
#include "Veiculo.h"

Veiculo::Veiculo(const std::string& p)
    : placa(p), horaEntrada(std::chrono::system_clock::now()) {}

std::string Veiculo::obterPlaca() const {
    return placa;
}

std::chrono::system_clock::time_point Veiculo::obterHoraEntrada() const {
    return horaEntrada;
}
