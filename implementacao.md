# Implementação

> Nota Importante: Este documento relata o processo de implementação do sistema utilizando C++, STL e Qt.

## Ferramentas e Bibliotecas Utilizadas

| Ferramenta | Propósito |
|-----------|----------|
| **C++** | Linguagem base do projeto |
| **STL** | Estruturas de dados (vector, unordered_map, list) |
| **Qt** | Interface gráfica (GUI) |
| **CMake**  | Sistema de build |

## Estrutura de Dados (STL - Componentes Principais)

### 1. Vector de Vagas (Representação do Pátio Físico)
```cpp
std::vector<Vaga> vagas;  // 50 vagas do estacionamento

// Benefício: Acesso direto com complexidade O(1) por índice
for(int i = 0; i < vagas.size(); i++) {
    if(!vagas[i].estaOcupada()) {
        // Primeira vaga livre encontrada
    }
}
```

### 2. Unordered_Map para Busca Rápida (Solução de Desempenho)
```cpp
std::unordered_map<std::string, Vaga*> placaParaVaga;

// Busca com complexidade O(1), praticamente instantânea
// Sem este mapa, seria necessário fazer uma busca linear com O(n)
Vaga* vaga = placaParaVaga["ABC-1234"];  
// Encontramos o carro de forma instantânea
```

**Quando um veículo entra:**
```cpp
placaParaVaga["ABC-1234"] = &vagas[5];  // Vaga 5 está alocada
```

**Quando sai:**
```cpp
Vaga* vaga = placaParaVaga["ABC-1234"];  // O(1)
vaga->liberar();
placaParaVaga.erase("ABC-1234");  // Remove do mapa
```

### 3. List para Histórico de Transações
```cpp
std::list<Transacao> transacoes;  // Inserção/remoção O(1) nas extremidades

// Salvar cada transação de saída
transacoes.push_back(Transacao(placa, valor, dataHora));
```

## Hierarquia de Classes (Polimorfismo)

### Classe Abstrata: Veiculo
```cpp
class Veiculo {
protected:
    std::string placa;
    std::chrono::system_clock::time_point horaEntrada;
    
public:
    Veiculo(const std::string& p) : placa(p), horaEntrada(std::chrono::system_clock::now()) {}
    virtual ~Veiculo() = default;
    
    virtual float calcularTarifa(int tempoMinutos) = 0;  // Método Virtual Puro
    std::string obterPlaca() const { return placa; }
};
```

### Classe Concreta: Carro
```cpp
class Carro : public Veiculo {
public:
    Carro(const std::string& p) : Veiculo(p) {}
    
    float calcularTarifa(int tempoMinutos) override {
        // Tarifa: R$ 5.00 por hora
        float horas = tempoMinutos / 60.0f;
        return horas * 5.0f;
    }
};
```

### Classe Concreta: Moto
```cpp
class Moto : public Veiculo {
public:
    Moto(const std::string& p) : Veiculo(p) {}
    
    float calcularTarifa(int tempoMinutos) override {
        // Tarifa: R$ 2.00 por hora
        float horas = tempoMinutos / 60.0f;
        return horas * 2.0f;
    }
};
```

## Classe Vaga

```cpp
class Vaga {
private:
    int numero;
    bool ocupada;
    Veiculo* veiculo;
    
public:
    Vaga(int num) : numero(num), ocupada(false), veiculo(nullptr) {}
    
    void alocar(Veiculo* v) {
        veiculo = v;
        ocupada = true;
    }
    
    void liberar() {
        veiculo = nullptr;
        ocupada = false;
    }
    
    bool estaOcupada() const { return ocupada; }
    Veiculo* getVeiculo() const { return veiculo; }
    int getNumero() const { return numero; }
};
```

## Classe Principal: Estacionamento

```cpp
class Estacionamento {
private:
    std::vector<Vaga> vagas;
    std::unordered_map<std::string, Vaga*> placaParaVaga;  // Mapa rápido
    std::list<Transacao> transacoes;  // Histórico
    const int CAPACIDADE = 50;
    
public:
    Estacionamento() {
        for(int i = 0; i < CAPACIDADE; i++) {
            vagas.emplace_back(Vaga(i + 1));
        }
    }
    
    // Registrar entrada de veículo
    bool registrarEntrada(const std::string& placa, int tipo) {
        // tipo: 0 = Carro, 1 = Moto
        
        // Verificar se vaga livre existe
        Vaga* vagaLivre = nullptr;
        for(auto& vaga : vagas) {
            if(!vaga.estaOcupada()) {
                vagaLivre = &vaga;
                break;  // Primeira vaga livre
            }
        }
        
        if(!vagaLivre) {
            return false;  // Estacionamento lotado
        }
        
        // Criar veículo apropriado (Polimorfismo)
        Veiculo* veiculo = nullptr;
        if(tipo == 0) {
            veiculo = new Carro(placa);
        } else {
            veiculo = new Moto(placa);
        }
        
        // Alocar na vaga
        vagaLivre->alocar(veiculo);
        
        // Adicionar ao mapa para busca rápida
        placaParaVaga[placa] = vagaLivre;
        
        return true;
    }
    
    // Registrar saída e calcular tarifa
    float registrarSaida(const std::string& placa) {
        // Busca O(1) no mapa
        auto it = placaParaVaga.find(placa);
        if(it == placaParaVaga.end()) {
            return -1.0f;  // Veículo não encontrado
        }
        
        Vaga* vaga = it->second;
        Veiculo* veiculo = vaga->getVeiculo();
        
        // Calcular tempo em minutos
        auto agora = std::chrono::system_clock::now();
        auto duracao = std::chrono::duration_cast<std::chrono::minutes>
                      (agora - veiculo->getHoraEntrada());
        int tempoMinutos = duracao.count();
        
        // Polimorfismo: calcularTarifa é virtual!
        float valor = veiculo->calcularTarifa(tempoMinutos);
        
        // Liberar vaga
        vaga->liberar();
        delete veiculo;
        
        // Remover do mapa
        placaParaVaga.erase(placa);
        
        // Registrar transação
        transacoes.push_back(Transacao(placa, valor, agora));
        
        return valor;
    }
    
    // Obter mapa de vagas para visualização
    std::vector<bool> obterMapaVagas() const {
        std::vector<bool> mapa;
        for(const auto& vaga : vagas) {
            mapa.push_back(vaga.estaOcupada());
        }
        return mapa;
    }
    
    // Contar vagas livres
    int vagasLivres() const {
        int count = 0;
        for(const auto& vaga : vagas) {
            if(!vaga.estaOcupada()) count++;
        }
        return count;
    }
    
    // Gerar relatório de transações
    void gerarRelatorioTransacoes() const {
        for(const auto& t : transacoes) {
            t.exibir();  // Placa, Valor, Data/Hora
        }
    }
};
```

## Classe Transacao

```cpp
class Transacao {
private:
    std::string placa;
    float valorPago;
    std::chrono::system_clock::time_point dataHora;
    int tempoMinutos;
    
public:
    Transacao(const std::string& p, float v, 
              const std::chrono::system_clock::time_point& dt)
        : placa(p), valorPago(v), dataHora(dt) {}
    
    void exibir() const {
        std::cout << "Placa: " << placa 
                  << " | Valor: R$ " << valorPago 
                  << " | Data/Hora: " << /* formatar data */ std::endl;
    }
};
```

## Interface Qt (MainWindow)

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT
    
private:
    Estacionamento* estacionamento;
    
    // Widgets da esquerda
    QLineEdit* plateInput;
    QPushButton* btnEntrada;
    QPushButton* btnSaida;
    QLabel* labelStatus;
    QLabel* labelVagasLivres;
    
    // Widgets da direita
    QGridLayout* gridLayout;  // 5x10 grid para 50 vagas
    std::vector<QPushButton*> vagasButtons;
    
public:
    MainWindow() {
        estacionamento = new Estacionamento();
        setupUI();
    }
    
    void setupUI() {
        // Layout principal
        QWidget* centralWidget = new QWidget(this);
        QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
        
        // ===== PAINEL ESQUERDO (Controle) =====
        QVBoxLayout* leftPanel = new QVBoxLayout();
        
        plateInput = new QLineEdit();
        plateInput->setPlaceholderText("Digite a placa");
        
        btnEntrada = new QPushButton("Registrar Entrada");
        btnSaida = new QPushButton("Registrar Saída");
        
        labelStatus = new QLabel("Pronto");
        labelVagasLivres = new QLabel();
        
        leftPanel->addWidget(new QLabel("Placa:"));
        leftPanel->addWidget(plateInput);
        leftPanel->addWidget(btnEntrada);
        leftPanel->addWidget(btnSaida);
        leftPanel->addWidget(new QLabel("Status:"));
        leftPanel->addWidget(labelStatus);
        leftPanel->addWidget(labelVagasLivres);
        leftPanel->addStretch();
        
        // ===== PAINEL DIREITO (Mapa de Vagas) =====
        QWidget* gridWidget = new QWidget();
        gridLayout = new QGridLayout(gridWidget);
        
        // Criar 50 botões em grid 5 linhas x 10 colunas
        for(int i = 0; i < 50; i++) {
            QPushButton* btn = new QPushButton(QString::number(i + 1));
            btn->setFixedSize(60, 60);
            btn->setStyleSheet("background-color: green;");  // Verde = livre
            vagasButtons.push_back(btn);
            
            int linha = i / 10;
            int coluna = i % 10;
            gridLayout->addWidget(btn, linha, coluna);
        }
        
        // Adicionar painéis ao layout principal
        mainLayout->addLayout(leftPanel, 1);  // Proporção 1:3
        mainLayout->addWidget(gridWidget, 3);
        
        setCentralWidget(centralWidget);
        setWindowTitle("Sistema de Estacionamento");
        setGeometry(100, 100, 1200, 600);
        
        // Conectar sinais
        connect(btnEntrada, &QPushButton::clicked, this, &MainWindow::onRegistrarEntrada);
        connect(btnSaida, &QPushButton::clicked, this, &MainWindow::onRegistrarSaida);
        
        atualizarMapaVagas();
    }
    
private slots:
    void onRegistrarEntrada() {
        QString placa = plateInput->text().toUpper();
        if(placa.isEmpty()) {
            labelStatus->setText("Digite uma placa!");
            return;
        }
        
        // Para exemplo, sempre Carro. Em produção, seria um combo
        bool sucesso = estacionamento->registrarEntrada(placa.toStdString(), 0);
        
        if(sucesso) {
            labelStatus->setText("Entrada registrada: " + placa);
            plateInput->clear();
        } else {
            labelStatus->setText("ESTACIONAMENTO LOTADO!");
        }
        
        atualizarMapaVagas();
    }
    
    void onRegistrarSaida() {
        QString placa = plateInput->text().toUpper();
        if(placa.isEmpty()) {
            labelStatus->setText("Digite uma placa!");
            return;
        }
        
        float valor = estacionamento->registrarSaida(placa.toStdString());
        
        if(valor < 0) {
            labelStatus->setText("VEÍCULO NÃO ENCONTRADO!");
        } else {
            labelStatus->setText(QString::asprintf("Saída: R$ %.2f", valor));
            plateInput->clear();
        }
        
        atualizarMapaVagas();
    }
    
    void atualizarMapaVagas() {
        std::vector<bool> mapa = estacionamento->obterMapaVagas();
        
        for(int i = 0; i < 50; i++) {
            if(mapa[i]) {
                vagasButtons[i]->setStyleSheet("background-color: red;");  // Ocupada
            } else {
                vagasButtons[i]->setStyleSheet("background-color: green;");  // Livre
            }
        }
        
        int livres = estacionamento->vagasLivres();
        labelVagasLivres->setText(QString("Vagas livres: %1/50").arg(livres));
    }
};
```

## Compilação (CMakeLists.txt)

```cmake
cmake_minimum_required(VERSION 3.16)
project(EstacionamentoPOO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(estacionamento
    main.cpp
    veiculo.h
    carro.h
    moto.h
    vaga.h
    transacao.h
    estacionamento.h
    mainwindow.h
    mainwindow.cpp
)

target_link_libraries(estacionamento Qt5::Core Qt5::Gui Qt5::Widgets)
```

<div align="center">

[Retroceder](projeto.md) | [Avançar](testes.md)

</div>