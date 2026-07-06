/**
 * @file Vaga.h
 * @brief Definição da classe Vaga, representando um espaço no estacionamento.
 */
#ifndef VAGA_H
#define VAGA_H

#include "Veiculo.h"
#include <memory>

class Vaga {
private:
    int numero;
    int tipoVaga; // 0 = Carro, 1 = Moto
    bool ocupada;
    Veiculo* veiculo; // Ponteiro fraco (Estacionamento gerencia o ciclo de vida)

public:
    Vaga() : numero(0), tipoVaga(0), ocupada(false), veiculo(nullptr) {}
    explicit Vaga(int num, int tipo = 0) : numero(num), tipoVaga(tipo), ocupada(false), veiculo(nullptr) {}

    void alocar(Veiculo* v) {
        veiculo = v;
        ocupada = true;
    }

    Veiculo* liberar() {
        Veiculo* v = veiculo;
        veiculo = nullptr;
        ocupada = false;
        return v;
    }

    bool estaOcupada() const { return ocupada; }
    Veiculo* getVeiculo() const { return veiculo; }
    int getNumero() const { return numero; }
    void setNumero(int num) { numero = num; }
    int getTipoVaga() const { return tipoVaga; }
    void setTipoVaga(int tipo) { tipoVaga = tipo; }
};

#endif // VAGA_H
