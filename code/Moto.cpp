/**
 * @file Moto.cpp
 * @brief Implementação dos métodos da classe Moto e cálculo de sua tarifa.
 */
#include "Moto.h"

float Moto::calcularTarifa(int tempoMinutos) const {
    float horas = tempoMinutos / 60.0f;
    if (horas < 1.0f) horas = 1.0f; // Mínimo 1 hora
    
    if (tarifaDeCarro) {
        return horas * 5.0f;
    }
    return horas * 2.0f;
}

std::string Moto::obterTipo() const {
    return tarifaDeCarro ? "Moto (Tarifa Carro)" : "Moto";
}
