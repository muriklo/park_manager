/**
 * @file Cliente.cpp
 * @brief Implementação da classe Cliente.
 */
#include "Cliente.h"

Cliente::Cliente(const std::string& placa, const std::string& nome, Assinatura::Plano plano)
    : placa(placa), nome(nome), assinatura(plano), isMensalista(true) {}

std::string Cliente::obterPlaca() const {
    return placa;
}

std::string Cliente::obterNome() const {
    return nome;
}

bool Cliente::ehMensalista() const {
    return isMensalista;
}

Assinatura& Cliente::obterAssinatura() {
    return assinatura;
}
