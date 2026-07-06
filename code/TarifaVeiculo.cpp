/**
 * @file TarifaVeiculo.cpp
 * @brief Implementação das lógicas de cálculo de tarifa comum e para mensalistas.
 */
#include "TarifaVeiculo.h"
#include <cmath>

TarifaComum::TarifaComum(float taxaPorHora) : taxaHora(taxaPorHora) {}

float TarifaComum::calcularCusto(int tempoMinutos) {
    float horas = tempoMinutos / 60.0f;
    if (horas < 1.0f) horas = 1.0f; // Mínimo 1 hora
    return horas * taxaHora;
}

TarifaMensalista::TarifaMensalista(Cliente* c) : cliente(c) {}

float TarifaMensalista::calcularCusto(int tempoMinutos) {
    if (!cliente) return 0.0f;
    
    // Arredonda o tempo consumido para cima em horas
    int horasConsumidas = std::ceil(tempoMinutos / 60.0f);
    if (horasConsumidas == 0) horasConsumidas = 1; // Mínimo de 1 hora
    
    Assinatura& assinatura = cliente->obterAssinatura();
    
    if (assinatura.temHorasDisponiveis(horasConsumidas)) {
        assinatura.consumirHoras(horasConsumidas);
        return 0.0f; // Coberto pelo plano
    } else {
        // Consome horas restantes da assinatura e cobra o excedente (R$ 5.00/h)
        int horasDisponiveis = assinatura.horasRestantes();
        assinatura.consumirHoras(horasDisponiveis);
        
        int horasExcedentes = horasConsumidas - horasDisponiveis;
        return horasExcedentes * 5.0f;
    }
}
