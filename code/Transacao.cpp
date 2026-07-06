/**
 * @file Transacao.cpp
 * @brief Implementação da classe Transacao e formatação de dados.
 */
#include "Transacao.h"

Transacao::Transacao(const std::string& p, const std::string& tipo, float v, int tempo,
          std::chrono::system_clock::time_point dt)
    : placa(p), tipoVeiculo(tipo), valorPago(v), tempoMinutos(tempo), dataHora(dt) {}

std::string Transacao::obterPlaca() const { return placa; }
std::string Transacao::obterTipoVeiculo() const { return tipoVeiculo; }
float Transacao::obterValor() const { return valorPago; }
int Transacao::obterTempoMinutos() const { return tempoMinutos; }

std::string Transacao::obterDataHoraFormatada() const {
    std::time_t tempo = std::chrono::system_clock::to_time_t(dataHora);
    std::tm tm_local;
    #ifdef _WIN32
        localtime_s(&tm_local, &tempo);
    #else
        localtime_r(&tempo, &tm_local);
    #endif
    std::ostringstream oss;
    oss << std::put_time(&tm_local, "%d/%m/%Y %H:%M:%S");
    return oss.str();
}
