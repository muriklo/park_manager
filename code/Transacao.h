/**
 * @file Transacao.h
 * @brief Definição da classe Transacao, armazenando registros de saída de veículos.
 */
#ifndef TRANSACAO_H
#define TRANSACAO_H

#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>

// Registro de transação de saída
class Transacao {
private:
    std::string placa;
    std::string tipoVeiculo;
    float valorPago;
    std::chrono::system_clock::time_point dataHora;
    int tempoMinutos;

public:
    Transacao(const std::string& p, const std::string& tipo, float v, int tempo,
              std::chrono::system_clock::time_point dt);

    std::string obterPlaca() const;
    std::string obterTipoVeiculo() const;
    float obterValor() const;
    int obterTempoMinutos() const;
    std::string obterDataHoraFormatada() const;
};

#endif // TRANSACAO_H
