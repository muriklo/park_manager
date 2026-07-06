/**
 * @file Carro.cpp
 * @brief Implementação dos métodos da classe Carro e cálculo de sua tarifa.
 */
#include "Carro.h"

float Carro::calcularTarifa(int tempoMinutos) const {
    float horas = tempoMinutos / 60.0f;
    if (horas < 1.0f) horas = 1.0f; // Mínimo 1 hora
    return horas * 5.0f;
}

std::string Carro::obterTipo() const {
    return "Carro";
}
