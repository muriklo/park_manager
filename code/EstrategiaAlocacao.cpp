/**
 * @file EstrategiaAlocacao.cpp
 * @brief Implementação da busca e alocação de vagas por tipo de veículo.
 */
#include "EstrategiaAlocacao.h"

Vaga* EstrategiaVagaLivre::encontrarVaga(std::array<Vaga, CAPACIDADE_ESTACIONAMENTO>& vagas,
                                         const std::string& placa, int tipoVeiculo) {
    if (tipoVeiculo == 0) { // Carro
        for (Vaga& vaga : vagas) {
            if (!vaga.estaOcupada() && vaga.getTipoVaga() == 0) return &vaga;
        }
    } else if (tipoVeiculo == 1) { // Moto
        // Tenta vaga de moto primeiro (tipo 1)
        for (Vaga& vaga : vagas) {
            if (!vaga.estaOcupada() && vaga.getTipoVaga() == 1) return &vaga;
        }
        // Fallback: Tenta vaga de carro (tipo 0)
        for (Vaga& vaga : vagas) {
            if (!vaga.estaOcupada() && vaga.getTipoVaga() == 0) return &vaga;
        }
    }
    return nullptr;
}

std::string EstrategiaVagaLivre::obterNome() const {
    return "Primeira vaga livre";
}
