/**
 * @file Estacionamento.cpp
 * @brief Implementação da lógica de gerenciamento de estacionamento, veículos e faturamento.
 */
#include "Estacionamento.h"

Estacionamento::Estacionamento()
    : estrategia(std::make_unique<EstrategiaVagaLivre>())
{
    for (int i = 0; i < CAPACIDADE_ESTACIONAMENTO; ++i) {
        vagas[i].setNumero(i + 1);
        if (i < 50) {
            vagas[i].setTipoVaga(0); // Carro
        } else {
            vagas[i].setTipoVaga(1); // Moto
        }
    }
}

Estacionamento::~Estacionamento() {
    for (Vaga& vaga : vagas) {
        if (vaga.estaOcupada()) {
            delete vaga.liberar();
        }
    }
    for (std::pair<const std::string, Cliente*>& pair : clientesMensalistas) {
        delete pair.second;
    }
}

void Estacionamento::definirEstrategia(std::unique_ptr<EstrategiaAlocacao> nova) {
    estrategia = std::move(nova);
}

std::string Estacionamento::obterNomeEstrategia() const {
    return estrategia ? estrategia->obterNome() : "Nenhuma";
}

void Estacionamento::adicionarCliente(Cliente* cliente) {
    if (cliente) {
        clientesMensalistas[cliente->obterPlaca()] = cliente;
    }
}

const std::unordered_map<std::string, Cliente*>& Estacionamento::obterClientes() const {
    return clientesMensalistas;
}

int Estacionamento::registrarEntrada(const std::string& placa, int tipo) {
    if (placaParaVaga.find(placa) != placaParaVaga.end()) {
        return -2;
    }

    Vaga* vagaLivre = estrategia->encontrarVaga(vagas, placa, tipo);
    if (!vagaLivre) return -1;

    Veiculo* veiculo = nullptr;
    if (tipo == 0) {
        veiculo = static_cast<Veiculo*>(new Carro(placa));
    } else {
        bool tarifaDeCarro = (vagaLivre->getTipoVaga() == 0);
        veiculo = static_cast<Veiculo*>(new Moto(placa, tarifaDeCarro));
    }

    vagaLivre->alocar(veiculo);
    placaParaVaga[placa] = vagaLivre;

    return vagaLivre->getNumero();
}

Estacionamento::ResultadoSaida Estacionamento::registrarSaida(const std::string& placa) {
    ResultadoSaida resultado;

    std::unordered_map<std::string, Vaga*>::iterator it = placaParaVaga.find(placa);
    if (it == placaParaVaga.end()) {
        return resultado;
    }

    Vaga* vaga = it->second;
    Veiculo* veiculo = vaga->getVeiculo();

    // Aceleração do tempo para testes: 1s real = 1000s simulados
    const int FATOR_ACELERACAO_TEMPO = 1000;

    std::chrono::system_clock::time_point agora = std::chrono::system_clock::now();
    
    long long duracaoSegundos = std::chrono::duration_cast<std::chrono::seconds>(
        agora - veiculo->obterHoraEntrada()).count();
        
    long long segundosSimulados = duracaoSegundos * FATOR_ACELERACAO_TEMPO;
    int tempoMin = static_cast<int>(segundosSimulados / 60);
    
    if (tempoMin < 1) tempoMin = 1;

    // Seleção polimórfica da tarifa (Strategy Pattern)
    std::unique_ptr<TarifaVeiculo> tarifa = nullptr;
    std::unordered_map<std::string, Cliente*>::iterator itCliente = clientesMensalistas.find(placa);
    
    if (itCliente != clientesMensalistas.end()) {
        tarifa = std::make_unique<TarifaMensalista>(itCliente->second);
    } else {
        // Obtém taxa horária padrão do veículo
        float taxaPorHora = veiculo->calcularTarifa(60); 
        tarifa = std::make_unique<TarifaComum>(taxaPorHora);
    }

    float valor = tarifa->calcularCusto(tempoMin);

    resultado.valor = valor;
    resultado.tempoMinutos = tempoMin;
    resultado.tipo = (itCliente != clientesMensalistas.end()) ? "Mensalista" : veiculo->obterTipo();
    resultado.numeroVaga = vaga->getNumero();

    transacoes.emplace_back(placa, resultado.tipo, valor, tempoMin, agora);

    vaga->liberar();
    delete veiculo;
    placaParaVaga.erase(it);

    return resultado;
}

int Estacionamento::vagasLivres() const {
    int count = 0;
    for (const Vaga& vaga : vagas) {
        if (!vaga.estaOcupada()) ++count;
    }
    return count;
}

int Estacionamento::vagasLivresCarro() const {
    int count = 0;
    for (const Vaga& vaga : vagas) {
        if (!vaga.estaOcupada() && vaga.getTipoVaga() == 0) ++count;
    }
    return count;
}

int Estacionamento::vagasLivresMoto() const {
    int count = 0;
    for (const Vaga& vaga : vagas) {
        if (!vaga.estaOcupada() && vaga.getTipoVaga() == 1) ++count;
    }
    return count;
}

const std::array<Vaga, CAPACIDADE_ESTACIONAMENTO>& Estacionamento::obterVagas() const {
    return vagas;
}

const std::list<Transacao>& Estacionamento::obterTransacoes() const {
    return transacoes;
}
