/**
 * @file Estacionamento.h
 * @brief Classe principal de gerenciamento do estacionamento, vagas e transações.
 */
#ifndef ESTACIONAMENTO_H
#define ESTACIONAMENTO_H

#include "Vaga.h"
#include "Veiculo.h"
#include "Carro.h"
#include "Moto.h"
#include "Transacao.h"
#include "EstrategiaAlocacao.h"
#include "Cliente.h"
#include "TarifaVeiculo.h"

#include <array>
#include <unordered_map>
#include <list>
#include <memory>
#include <string>
#include <chrono>

class Estacionamento {
private:
    std::array<Vaga, CAPACIDADE_ESTACIONAMENTO> vagas;
    std::unordered_map<std::string, Vaga*> placaParaVaga;
    std::unordered_map<std::string, Cliente*> clientesMensalistas;
    std::list<Transacao> transacoes;
    std::unique_ptr<EstrategiaAlocacao> estrategia;

public:
    Estacionamento();
    ~Estacionamento();

    // Altera a estratégia de alocação em tempo de execução
    void definirEstrategia(std::unique_ptr<EstrategiaAlocacao> nova);
    std::string obterNomeEstrategia() const;

    void adicionarCliente(Cliente* cliente);
    const std::unordered_map<std::string, Cliente*>& obterClientes() const;

    // tipo: 0 = Carro, 1 = Moto. Retorna vaga alocada, -1 se lotado ou -2 se placa duplicada.
    int registrarEntrada(const std::string& placa, int tipo);

    struct ResultadoSaida {
        float valor = -1.0f;
        int tempoMinutos = 0;
        std::string tipo;
        int numeroVaga = 0;
    };

    ResultadoSaida registrarSaida(const std::string& placa);

    int vagasLivres() const;
    int vagasLivresCarro() const;
    int vagasLivresMoto() const;

    const std::array<Vaga, CAPACIDADE_ESTACIONAMENTO>& obterVagas() const;

    const std::list<Transacao>& obterTransacoes() const;

    int capacidade() const { return CAPACIDADE_ESTACIONAMENTO; }
};

#endif // ESTACIONAMENTO_H
