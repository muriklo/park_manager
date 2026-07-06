/**
 * @file Assinatura.cpp
 * @brief Implementação dos métodos da classe Assinatura para controle de horas de planos.
 */
#include "Assinatura.h"

Assinatura::Assinatura(Plano p) : tipo(p), horasDisponiveis(static_cast<int>(p)), horasUsadas(0) {}

bool Assinatura::temHorasDisponiveis(int horas) const {
    return (horasDisponiveis - horasUsadas) >= horas;
}

int Assinatura::horasRestantes() const {
    return horasDisponiveis - horasUsadas;
}

void Assinatura::consumirHoras(int horas) {
    horasUsadas += horas;
}

Assinatura::Plano Assinatura::obterTipo() const {
    return tipo;
}

int Assinatura::obterHorasUsadas() const {
    return horasUsadas;
}
