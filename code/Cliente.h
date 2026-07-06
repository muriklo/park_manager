/**
 * @file Cliente.h
 * @brief Definição da classe Cliente, gerenciando informações e assinatura do mensalista.
 */
#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>
#include "Assinatura.h"

class Cliente {
private:
    std::string placa;
    std::string nome;
    Assinatura assinatura;
    bool isMensalista;

public:
    Cliente(const std::string& placa, const std::string& nome, Assinatura::Plano plano);

    std::string obterPlaca() const;
    std::string obterNome() const;
    bool ehMensalista() const;
    Assinatura& obterAssinatura();
};

#endif // CLIENTE_H
