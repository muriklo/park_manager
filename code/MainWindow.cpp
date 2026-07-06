/**
 * @file MainWindow.cpp
 * @brief Implementação da lógica de interface gráfica e interação com o sistema.
 */
#include "MainWindow.h"
#include <QApplication>
#include <QStyle>
#include <QTimer>

// Normaliza a placa: uppercase e adiciona hífen se necessário (ex: qwe1234 → QWE-1234)
static QString normalizarPlaca(const QString& texto) {
    QString placa = texto.trimmed().toUpper().remove('-');
    if (placa.length() == 7)
        placa.insert(3, '-');
    return placa;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUI();
  atualizarMapa();
  atualizarStatus();
}

void MainWindow::setupUI() {
  setWindowTitle(
      QString::fromUtf8("\xF0\x9F\x85\xBF\xEF\xB8\x8F ParkManager \xe2\x80\x94 "
                        "Gerenciador de Estacionamento"));
  setMinimumSize(950, 650);
  setStyleSheet(
      "QMainWindow { background-color: #1e1e2e; }"
      "QLabel { color: #cdd6f4; font-size: 13px; }"
      "QLineEdit { background-color: #313244; color: #cdd6f4; border: 2px "
      "solid #45475a;"
      "  border-radius: 8px; padding: 8px; font-size: 14px; }"
      "QLineEdit:focus { border-color: #89b4fa; }"
      "QComboBox { background-color: #313244; color: #cdd6f4; border: 2px "
      "solid #45475a;"
      "  border-radius: 8px; padding: 8px; font-size: 14px; }"
      "QComboBox:focus { border-color: #89b4fa; }"
      "QComboBox QAbstractItemView { background-color: #313244; color: #cdd6f4;"
      "  selection-background-color: #45475a; }"
      "QGroupBox { color: #cdd6f4; font-size: 14px; font-weight: bold;"
      "  border: 2px solid #45475a; border-radius: 10px; margin-top: 10px; "
      "padding-top: 15px; }"
      "QGroupBox::title { subcontrol-origin: margin; left: 15px; padding: 0 "
      "8px; }"
      "QTableWidget { background-color: #313244; color: #cdd6f4; border: 2px "
      "solid #45475a;"
      "  border-radius: 8px; gridline-color: #45475a; font-size: 12px; }"
      "QTableWidget::item { padding: 4px; }"
      "QHeaderView::section { background-color: #45475a; color: #cdd6f4;"
      "  font-weight: bold; border: none; padding: 6px; }"
      "QStatusBar { background-color: #181825; color: #a6adc8; font-size: "
      "12px; }");

  QWidget *central = new QWidget(this);
  setCentralWidget(central);

  QHBoxLayout *mainLayout = new QHBoxLayout(central);
  mainLayout->setSpacing(15);
  mainLayout->setContentsMargins(15, 15, 15, 15);

  // Painel Esquerdo: Controles
  QVBoxLayout *leftLayout = new QVBoxLayout();
  leftLayout->setSpacing(10);

  QLabel *titulo = new QLabel("PARK MANAGER");
  titulo->setStyleSheet("font-size: 22px; font-weight: bold; color: #89b4fa;"
                        " padding: 10px; letter-spacing: 2px;");
  titulo->setAlignment(Qt::AlignCenter);
  leftLayout->addWidget(titulo);

  QGroupBox *grupoControle = new QGroupBox(" Controle de Veiculos");
  QVBoxLayout *controleLayout = new QVBoxLayout(grupoControle);
  controleLayout->setSpacing(10);

  QLabel *lblPlaca = new QLabel("Placa do Veiculo:");
  inputPlaca = new QLineEdit();
  inputPlaca->setPlaceholderText("Ex: ABC-1234");
  inputPlaca->setMaxLength(8);
  controleLayout->addWidget(lblPlaca);
  controleLayout->addWidget(inputPlaca);

  QLabel *lblTipo = new QLabel("Tipo:");
  comboTipo = new QComboBox();
  comboTipo->addItem("Carro (R$ 5,00/h)", 0);
  comboTipo->addItem("Moto  (R$ 2,00/h)", 1);
  controleLayout->addWidget(lblTipo);
  controleLayout->addWidget(comboTipo);

  btnEntrada = new QPushButton("  Registrar Entrada");
  btnEntrada->setStyleSheet(
      "QPushButton { background-color: #a6e3a1; color: #1e1e2e; font-size: "
      "14px;"
      "  font-weight: bold; border-radius: 10px; padding: 12px; }"
      "QPushButton:hover { background-color: #94e2d5; }"
      "QPushButton:pressed { background-color: #74c7ec; }");
  btnSaida = new QPushButton("  Registrar Saida");
  btnSaida->setStyleSheet(
      "QPushButton { background-color: #f38ba8; color: #1e1e2e; font-size: "
      "14px;"
      "  font-weight: bold; border-radius: 10px; padding: 12px; }"
      "QPushButton:hover { background-color: #eba0ac; }"
      "QPushButton:pressed { background-color: #f5c2e7; }");

  controleLayout->addWidget(btnEntrada);
  controleLayout->addWidget(btnSaida);

  leftLayout->addWidget(grupoControle);

  QGroupBox *grupoInfo = new QGroupBox(" Informacoes");
  QVBoxLayout *infoLayout = new QVBoxLayout(grupoInfo);

  lblVagasLivres = new QLabel("Vagas Livres: 50 / 50");
  lblVagasLivres->setStyleSheet(
      "font-size: 16px; font-weight: bold; color: #a6e3a1;");
  lblEstrategia = new QLabel("Estrategia: Primeira vaga livre");
  lblEstrategia->setStyleSheet("font-size: 12px; color: #a6adc8;");
  lblStatus = new QLabel("Pronto.");
  lblStatus->setStyleSheet("font-size: 13px; color: #f9e2af; padding: 5px;"
                           " background-color: #313244; border-radius: 6px;");
  lblStatus->setWordWrap(true);

  infoLayout->addWidget(lblVagasLivres);
  infoLayout->addWidget(lblEstrategia);
  infoLayout->addWidget(lblStatus);

  leftLayout->addWidget(grupoInfo);
  leftLayout->addStretch();

  mainLayout->addLayout(leftLayout, 1);

  // Painel Direito: Abas (Mapa, Histórico e Clientes)
  tabs = new QTabWidget();
  tabs->setStyleSheet(
      "QTabWidget::pane { border: 2px solid #45475a; border-radius: 8px;"
      "  background-color: #181825; }"
      "QTabBar::tab { background-color: #313244; color: #cdd6f4; padding: 10px "
      "20px;"
      "  border-top-left-radius: 8px; border-top-right-radius: 8px;"
      "  margin-right: 3px; font-weight: bold; }"
      "QTabBar::tab:selected { background-color: #45475a; color: #89b4fa; }"
      "QTabBar::tab:hover { background-color: #585b70; }");

  // Aba 1: Mapa de Vagas
  QWidget *abaMapaWidget = new QWidget();
  QVBoxLayout *mapaLayout = new QVBoxLayout(abaMapaWidget);

  QLabel *mapaTitle = new QLabel("Mapa do Estacionamento");
  mapaTitle->setStyleSheet(
      "font-size: 16px; font-weight: bold; color: #89b4fa; padding: 5px;");
  mapaTitle->setAlignment(Qt::AlignCenter);
  mapaLayout->addWidget(mapaTitle);

  QHBoxLayout *legendaLayout = new QHBoxLayout();
  QLabel *legLivre = new QLabel("  Livre  ");
  legLivre->setStyleSheet(
      "color: #a6e3a1; font-weight: bold; background-color: #a6e3a1;"
      " color: #1e1e2e; padding: 4px 8px; border-radius: 4px;");
  QLabel *legOcupado = new QLabel("  Ocupado  ");
  legOcupado->setStyleSheet(
      "color: #f38ba8; font-weight: bold; background-color: #f38ba8;"
      " color: #1e1e2e; padding: 4px 8px; border-radius: 4px;");
  legendaLayout->addStretch();
  legendaLayout->addWidget(legLivre);
  legendaLayout->addWidget(legOcupado);
  legendaLayout->addStretch();
  mapaLayout->addLayout(legendaLayout);

  QGridLayout *gridLayout = new QGridLayout();
  gridLayout->setSpacing(5);
  botoesVagas.resize(CAPACIDADE_ESTACIONAMENTO);

  for (int i = 0; i < CAPACIDADE_ESTACIONAMENTO; ++i) {
    QPushButton *btn = new QPushButton(QString::number(i + 1));
    btn->setProperty("vagaIndex", i);
    btn->setStyleSheet(
        "QPushButton { background-color: #a6e3a1; color: #1e1e2e;"
        "  font-weight: bold; font-size: 12px; border-radius: 6px; border: "
        "none; padding: 0px; }"
        "QPushButton:hover { border: 2px solid #89b4fa; }");
    connect(btn, &QPushButton::clicked, this, &MainWindow::onVagaClicada);
    
    if (i < 50) { // Vagas de Carro
      int row = i / 10;
      int col = i % 10;
      btn->setFixedSize(61, 46);
      gridLayout->addWidget(btn, row, col * 2, 1, 2);
    } else { // Vagas de Moto
      int motoIdx = i - 50;
      int row = 5 + (motoIdx / 20);
      int col = motoIdx % 20;
      btn->setFixedSize(28, 46);
      gridLayout->addWidget(btn, row, col, 1, 1);
    }
    botoesVagas[i] = btn;
  }
  mapaLayout->addLayout(gridLayout);
  mapaLayout->addStretch();

  tabs->addTab(abaMapaWidget, "Mapa de Vagas");

  // Aba 2: Histórico
  QWidget *abaHistWidget = new QWidget();
  QVBoxLayout *histLayout = new QVBoxLayout(abaHistWidget);

  tabelaHistorico = new QTableWidget(0, 5);
  tabelaHistorico->setHorizontalHeaderLabels(
      {"Placa", "Tipo", "Tempo (min)", "Valor (R$)", "Data/Hora"});
  tabelaHistorico->horizontalHeader()->setStretchLastSection(true);
  tabelaHistorico->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  tabelaHistorico->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tabelaHistorico->setSelectionBehavior(QAbstractItemView::SelectRows);
  tabelaHistorico->setAlternatingRowColors(true);
  tabelaHistorico->setStyleSheet(
      "QTableWidget { alternate-background-color: #2a2a3c; }");
  histLayout->addWidget(tabelaHistorico);

  tabs->addTab(abaHistWidget, "Historico de Transacoes");

  // Aba 3: Clientes Mensalistas
  QWidget *abaClientesWidget = new QWidget();
  QVBoxLayout *clientesLayout = new QVBoxLayout(abaClientesWidget);

  QGroupBox *grupoCadastro = new QGroupBox("Cadastrar Mensalista");
  QGridLayout *formLayout = new QGridLayout(grupoCadastro);

  formLayout->addWidget(new QLabel("Nome:"), 0, 0);
  inputNomeCliente = new QLineEdit();
  formLayout->addWidget(inputNomeCliente, 0, 1);

  formLayout->addWidget(new QLabel("Placa:"), 1, 0);
  inputPlacaCliente = new QLineEdit();
  formLayout->addWidget(inputPlacaCliente, 1, 1);

  formLayout->addWidget(new QLabel("Plano:"), 2, 0);
  comboPlano = new QComboBox();
  comboPlano->addItem("Plano 30 Horas", 30);
  comboPlano->addItem("Plano 50 Horas", 50);
  comboPlano->addItem("Plano 100 Horas", 100);
  comboPlano->addItem("Plano 250 Horas", 250);
  formLayout->addWidget(comboPlano, 2, 1);

  btnCadastrarCliente = new QPushButton("Cadastrar");
  btnCadastrarCliente->setStyleSheet(
      "QPushButton { background-color: #cba6f7; color: #1e1e2e; font-weight: "
      "bold; padding: 8px; border-radius: 6px; }"
      "QPushButton:hover { background-color: #b4befe; }");
  formLayout->addWidget(btnCadastrarCliente, 3, 0, 1, 2);

  clientesLayout->addWidget(grupoCadastro);

  tabelaClientes = new QTableWidget(0, 5);
  tabelaClientes->setHorizontalHeaderLabels(
      {"Placa", "Nome", "Plano (h)", "Horas Usadas", "Saldo (h)"});
  tabelaClientes->horizontalHeader()->setStretchLastSection(true);
  tabelaClientes->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  tabelaClientes->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tabelaClientes->setSelectionBehavior(QAbstractItemView::SelectRows);
  tabelaClientes->setAlternatingRowColors(true);
  tabelaClientes->setStyleSheet(
      "QTableWidget { alternate-background-color: #2a2a3c; }");
  clientesLayout->addWidget(tabelaClientes);

  tabs->addTab(abaClientesWidget, "Gerenciar Clientes");

  mainLayout->addWidget(tabs, 2);

  connect(btnEntrada, &QPushButton::clicked, this,
          &MainWindow::onRegistrarEntrada);
  connect(btnSaida, &QPushButton::clicked, this, &MainWindow::onRegistrarSaida);
  connect(inputPlaca, &QLineEdit::returnPressed, this,
          &MainWindow::onRegistrarEntrada);
  connect(btnCadastrarCliente, &QPushButton::clicked, this,
          &MainWindow::onCadastrarCliente);

  statusBar()->showMessage("Sistema pronto - ParkManager v1.0");
}

void MainWindow::onRegistrarEntrada() {
  QString placa = normalizarPlaca(inputPlaca->text());
  if (placa.isEmpty()) {
    lblStatus->setText("Informe a placa do veiculo.");
    lblStatus->setStyleSheet("font-size: 13px; color: #f38ba8; padding: 5px;"
                             " background-color: #313244; border-radius: 6px;");
    return;
  }

  int tipo = comboTipo->currentData().toInt();

  if (tipo == 1 && estacionamento.vagasLivresMoto() == 0 && estacionamento.vagasLivresCarro() > 0) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Vagas de Moto Lotadas",
        "Todas as vagas de moto estao ocupadas.\nDeseja alocar o veiculo em uma vaga de carro e pagar a tarifa de carro (R$ 5,00/h)?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
      lblStatus->setText("Entrada de moto cancelada pelo usuario.");
      lblStatus->setStyleSheet("font-size: 13px; color: #f9e2af; padding: 5px;"
                               " background-color: #313244; border-radius: 6px;");
      return;
    }
  }

  int resultado = estacionamento.registrarEntrada(placa.toStdString(), tipo);

  if (resultado == -2) {
    lblStatus->setText("Veiculo " + placa + " ja esta estacionado!");
    lblStatus->setStyleSheet("font-size: 13px; color: #f38ba8; padding: 5px;"
                             " background-color: #313244; border-radius: 6px;");
  } else if (resultado == -1) {
    lblStatus->setText("Estacionamento lotado! Sem vagas disponiveis.");
    lblStatus->setStyleSheet("font-size: 13px; color: #f38ba8; padding: 5px;"
                             " background-color: #313244; border-radius: 6px;");
  } else {
    QString tipoStr = (tipo == 0) ? "Carro" : "Moto";
    if (tipo == 1 && resultado <= 50) {
      lblStatus->setText("ATENCAO: " + tipoStr + " [" + placa +
                         "] na vaga de CARRO " + QString::number(resultado) +
                         ". Tarifa de carro (R$ 5/h) sera aplicada!");
      lblStatus->setStyleSheet(
          "font-size: 13px; color: #1e1e2e; font-weight: bold; padding: 5px;"
          " background-color: #f9e2af; border-radius: 6px;");
    } else {
      lblStatus->setText(tipoStr + " [" + placa + "] estacionado na vaga " +
                         QString::number(resultado));
      lblStatus->setStyleSheet(
          "font-size: 13px; color: #a6e3a1; padding: 5px;"
          " background-color: #313244; border-radius: 6px;");
    }
  }

  atualizarMapa();
  atualizarStatus();
  limparCampos();
}

void MainWindow::onRegistrarSaida() {
  QString placa = normalizarPlaca(inputPlaca->text());
  if (placa.isEmpty()) {
    lblStatus->setText("Informe a placa do veiculo para saida.");
    lblStatus->setStyleSheet("font-size: 13px; color: #f38ba8; padding: 5px;"
                             " background-color: #313244; border-radius: 6px;");
    return;
  }

  int tipoSelecionado = comboTipo->currentData().toInt();

  // Valida se o tipo selecionado bate com o tipo do veículo estacionado
  const std::array<Vaga, CAPACIDADE_ESTACIONAMENTO>& vagas = estacionamento.obterVagas();
  for (int i = 0; i < estacionamento.capacidade(); ++i) {
    if (vagas[i].estaOcupada()) {
      Veiculo *v = vagas[i].getVeiculo();
      if (v->obterPlaca() == placa.toStdString()) {
        bool isCarro = dynamic_cast<Carro *>(v) != nullptr;
        bool isMoto = dynamic_cast<Moto *>(v) != nullptr;

        if ((tipoSelecionado == 0 && !isCarro) ||
            (tipoSelecionado == 1 && !isMoto)) {
          QString tipoReal = isCarro ? "Carro" : "Moto";
          lblStatus->setText(
              QString("Tipo incorreto! O veiculo [%1] e um(a) %2.")
                  .arg(placa)
                  .arg(tipoReal));
          lblStatus->setStyleSheet(
              "font-size: 13px; color: #f38ba8; padding: 5px;"
              " background-color: #313244; border-radius: 6px;");
          return;
        }
        break;
      }
    }
  }

  Estacionamento::ResultadoSaida resultado = estacionamento.registrarSaida(placa.toStdString());

  if (resultado.valor < 0) {
    lblStatus->setText("Veiculo [" + placa +
                       "] nao encontrado no estacionamento.");
    lblStatus->setStyleSheet("font-size: 13px; color: #f38ba8; padding: 5px;"
                             " background-color: #313244; border-radius: 6px;");
  } else {
    lblStatus->setText(
        "Saida: " + QString::fromStdString(resultado.tipo) + " [" + placa +
        "] | Vaga " + QString::number(resultado.numeroVaga) +
        " | Tempo: " + QString::number(resultado.tempoMinutos) + " min" +
        " | Valor: R$ " + QString::number(resultado.valor, 'f', 2));
    lblStatus->setStyleSheet("font-size: 13px; color: #f9e2af; padding: 5px;"
                             " background-color: #313244; border-radius: 6px;");
  }

  atualizarMapa();
  atualizarStatus();
  atualizarHistorico();
  atualizarTabelaClientes();
  limparCampos();
}

void MainWindow::onCadastrarCliente() {
  QString nome = inputNomeCliente->text().trimmed();
  QString placa = normalizarPlaca(inputPlacaCliente->text());
  int horasPlano = comboPlano->currentData().toInt();

  if (nome.isEmpty() || placa.isEmpty()) {
    QMessageBox::warning(this, "Erro",
                         "Preencha todos os campos para cadastrar o cliente.");
    return;
  }

  // Easter egg: Simulação de dados
  if (placa == "SIM-0000") {
    popularDadosTeste();
    inputNomeCliente->clear();
    inputPlacaCliente->clear();
    return;
  }

  Assinatura::Plano plano = static_cast<Assinatura::Plano>(horasPlano);
  Cliente *novoCliente =
      new Cliente(placa.toStdString(), nome.toStdString(), plano);

  estacionamento.adicionarCliente(novoCliente);

  QMessageBox::information(this, "Sucesso",
                           "Cliente mensalista cadastrado com sucesso!");

  inputNomeCliente->clear();
  inputPlacaCliente->clear();

  atualizarTabelaClientes();
}

void MainWindow::onVagaClicada() {
  QPushButton *btn = qobject_cast<QPushButton *>(sender());
  if (!btn)
    return;

  int idx = btn->property("vagaIndex").toInt();
  const std::array<Vaga, CAPACIDADE_ESTACIONAMENTO>& vagas = estacionamento.obterVagas();
  const Vaga &vaga = vagas[idx];

  if (vaga.estaOcupada()) {
    Veiculo *v = vaga.getVeiculo();
    std::chrono::system_clock::time_point agora = std::chrono::system_clock::now();
    std::chrono::minutes duracao = std::chrono::duration_cast<std::chrono::minutes>(
        agora - v->obterHoraEntrada());

    QMessageBox::information(this, "Vaga " + QString::number(vaga.getNumero()),
                             "Vaga Ocupada\n\n"
                             "Placa: " +
                                 QString::fromStdString(v->obterPlaca()) +
                                 "\n"
                                 "Tipo: " +
                                 QString::fromStdString(v->obterTipo()) +
                                 "\n"
                                 "Tempo: " +
                                 QString::number(duracao.count()) + " minutos");
  } else {
    QMessageBox::information(this, "Vaga " + QString::number(vaga.getNumero()),
                             "Vaga Livre\n\nEsta vaga esta disponivel.");
  }
}

void MainWindow::atualizarMapa() {
  const std::array<Vaga, CAPACIDADE_ESTACIONAMENTO>& vagas = estacionamento.obterVagas();
  for (int i = 0; i < CAPACIDADE_ESTACIONAMENTO; ++i) {
    if (vagas[i].estaOcupada()) {
      Veiculo *v = vagas[i].getVeiculo();
      QString label = QString::number(i + 1) + "\n" +
                      QString::fromStdString(v->obterPlaca().substr(0, 4));
      botoesVagas[i]->setText(label);
      int fontSize = (i >= 50) ? 8 : 10; // Letra menor na moto
      botoesVagas[i]->setStyleSheet(
          QString("QPushButton { background-color: #f38ba8; color: #1e1e2e;"
          "  font-weight: bold; font-size: %1px; border-radius: 6px; border: "
          "none; padding: 0px; }"
          "QPushButton:hover { border: 2px solid #f5c2e7; }").arg(fontSize));
      botoesVagas[i]->setToolTip("Vaga " + QString::number(i + 1) + " - " +
                                 QString::fromStdString(v->obterTipo()) + " [" +
                                 QString::fromStdString(v->obterPlaca()) + "]");
    } else {
      botoesVagas[i]->setText(QString::number(i + 1));
      int fontSize = (i >= 50) ? 10 : 12; // Letra menor na moto
      botoesVagas[i]->setStyleSheet(
          QString("QPushButton { background-color: #a6e3a1; color: #1e1e2e;"
          "  font-weight: bold; font-size: %1px; border-radius: 6px; border: "
          "none; padding: 0px; }"
          "QPushButton:hover { border: 2px solid #89b4fa; }").arg(fontSize));
      QString tipoVagaStr = (vagas[i].getTipoVaga() == 0) ? "Carro" : "Moto";
      botoesVagas[i]->setToolTip("Vaga " + QString::number(i + 1) + " (" +
                                 tipoVagaStr + ") - Livre");
    }
  }
}

void MainWindow::atualizarStatus() {
  int livres = estacionamento.vagasLivres();
  int livresCarro = estacionamento.vagasLivresCarro();
  int livresMoto = estacionamento.vagasLivresMoto();
  int total = estacionamento.capacidade();

  lblVagasLivres->setText(
      QString("Vagas Livres: %1 / %2  (Carros: %3/50 | Motos: %4/30)")
          .arg(livres)
          .arg(total)
          .arg(livresCarro)
          .arg(livresMoto));

  // Altera a cor conforme a lotação
  if (livres == 0) {
    lblVagasLivres->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #f38ba8;");
  } else if (livres <= 10) {
    lblVagasLivres->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #f9e2af;");
  } else {
    lblVagasLivres->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #a6e3a1;");
  }

  lblEstrategia->setText(
      "Estrategia: " +
      QString::fromStdString(estacionamento.obterNomeEstrategia()));
}

void MainWindow::atualizarHistorico() {
  const std::list<Transacao>& transacoes = estacionamento.obterTransacoes();
  tabelaHistorico->setRowCount(0);

  // Exibe transações em ordem decrescente (mais recentes no topo)
  int row = 0;
  for (std::list<Transacao>::const_reverse_iterator it = transacoes.rbegin(); it != transacoes.rend(); ++it) {
    tabelaHistorico->insertRow(row);
    tabelaHistorico->setItem(
        row, 0, new QTableWidgetItem(QString::fromStdString(it->obterPlaca())));
    tabelaHistorico->setItem(
        row, 1,
        new QTableWidgetItem(QString::fromStdString(it->obterTipoVeiculo())));
    tabelaHistorico->setItem(
        row, 2, new QTableWidgetItem(QString::number(it->obterTempoMinutos())));
    tabelaHistorico->setItem(
        row, 3,
        new QTableWidgetItem("R$ " +
                             QString::number(it->obterValor(), 'f', 2)));
    tabelaHistorico->setItem(row, 4,
                             new QTableWidgetItem(QString::fromStdString(
                                 it->obterDataHoraFormatada())));
    ++row;
  }
}

void MainWindow::limparCampos() {
  inputPlaca->clear();
  inputPlaca->setFocus();
}

void MainWindow::atualizarTabelaClientes() {
  const std::unordered_map<std::string, Cliente*>& clientes = estacionamento.obterClientes();
  tabelaClientes->setRowCount(0);

  int row = 0;
  for (const std::pair<const std::string, Cliente*>& pair : clientes) {
    Cliente *c = pair.second;
    Assinatura &a = c->obterAssinatura();

    tabelaClientes->insertRow(row);
    tabelaClientes->setItem(
        row, 0, new QTableWidgetItem(QString::fromStdString(c->obterPlaca())));
    tabelaClientes->setItem(
        row, 1, new QTableWidgetItem(QString::fromStdString(c->obterNome())));
    tabelaClientes->setItem(
        row, 2,
        new QTableWidgetItem(QString::number(static_cast<int>(a.obterTipo()))));
    tabelaClientes->setItem(
        row, 3, new QTableWidgetItem(QString::number(a.obterHorasUsadas())));

    QTableWidgetItem *itemSaldo =
        new QTableWidgetItem(QString::number(a.horasRestantes()));
    if (a.horasRestantes() <= 5) {
      itemSaldo->setForeground(
          QBrush(QColor("#f38ba8"))); // Destaque visual para saldo baixo
    } else {
      itemSaldo->setForeground(QBrush(QColor("#a6e3a1")));
    }
    tabelaClientes->setItem(row, 4, itemSaldo);

    ++row;
  }
}

void MainWindow::popularDadosTeste() {
  // Clientes Mensalistas
  estacionamento.adicionarCliente(new Cliente("MNS-1111", "Alice Silva", Assinatura::PLANO_30H));
  estacionamento.adicionarCliente(new Cliente("MNS-2222", "Bruno Costa", Assinatura::PLANO_50H));
  estacionamento.adicionarCliente(new Cliente("MNS-3333", "Carla Dias", Assinatura::PLANO_100H));

  // Histórico (gerar saída fictícia para popular a tabela de transações)
  estacionamento.registrarEntrada("TMP-9999", 0);
  estacionamento.registrarSaida("TMP-9999");

  // Entradas de Mensalistas
  estacionamento.registrarEntrada("MNS-1111", 0); // Ocupa 1 vaga de carro
  estacionamento.registrarEntrada("MNS-2222", 1); // Ocupa 1 vaga de moto

  // Entradas Carros (tipo 0) - Precisamos preencher 47 vagas normais (+1 mensalista = 48) para sobrarem 2 de 50
  for (int i = 1; i <= 47; ++i) {
    QString placa = QString("CAR-%1").arg(i, 4, 10, QChar('0'));
    estacionamento.registrarEntrada(placa.toStdString(), 0);
  }

  // Entradas Motos (tipo 1) - Precisamos preencher 27 vagas normais (+1 mensalista = 28) para sobrarem 2 de 30
  for (int i = 1; i <= 27; ++i) {
    QString placa = QString("MOT-%1").arg(i, 4, 10, QChar('0'));
    estacionamento.registrarEntrada(placa.toStdString(), 1);
  }

  atualizarMapa();
  atualizarStatus();
  atualizarHistorico();
  atualizarTabelaClientes();

  QMessageBox::information(this, "Simulação Ativada", "Dados de teste foram populados no sistema!\nRestam apenas 2 vagas de Carro e 2 vagas de Moto.");
}
