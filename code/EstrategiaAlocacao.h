/**
 * @file EstrategiaAlocacao.h
 * @brief Definição da interface e estratégia concreta para alocação de vagas.
 */
#ifndef ESTRATEGIA_ALOCACAO_H
#define ESTRATEGIA_ALOCACAO_H

#include "Vaga.h"
#include <array>
#include <string>

constexpr int CAPACIDADE_ESTACIONAMENTO = 80;

class EstrategiaAlocacao {
public:
    virtual ~EstrategiaAlocacao() = default;
    virtual Vaga* encontrarVaga(std::array<Vaga, CAPACIDADE_ESTACIONAMENTO>& vagas,
                                const std::string& placa, int tipoVeiculo) = 0;
    virtual std::string obterNome() const = 0;
};

class EstrategiaVagaLivre : public EstrategiaAlocacao {
public:
    Vaga* encontrarVaga(std::array<Vaga, CAPACIDADE_ESTACIONAMENTO>& vagas,
                        const std::string& placa, int tipoVeiculo) override;
    std::string obterNome() const override;
};

#endif // ESTRATEGIA_ALOCACAO_H
