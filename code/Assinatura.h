/**
 * @file Assinatura.h
 * @brief Definição da classe Assinatura e os tipos de planos disponíveis para mensalistas.
 */
#ifndef ASSINATURA_H
#define ASSINATURA_H

class Assinatura {
public:
    enum Plano {
        PLANO_30H = 30,
        PLANO_50H = 50,
        PLANO_100H = 100,
        PLANO_250H = 250
    };

private:
    Plano tipo;
    int horasDisponiveis;
    int horasUsadas;

public:
    Assinatura(Plano p);

    bool temHorasDisponiveis(int horas) const;
    int horasRestantes() const;
    void consumirHoras(int horas);
    Plano obterTipo() const;
    int obterHorasUsadas() const;
};

#endif // ASSINATURA_H
