/**
 * @file MainWindow.h
 * @brief Definição da interface gráfica principal do sistema de gerenciamento de estacionamento.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QFrame>
#include <QHeaderView>
#include <QFont>
#include <QStatusBar>
#include <QTabWidget>
#include <vector>

#include "Estacionamento.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

private slots:
    void onRegistrarEntrada();
    void onRegistrarSaida();
    void onVagaClicada();
    void onCadastrarCliente();

private:
    void setupUI();
    void atualizarMapa();
    void atualizarStatus();
    void atualizarHistorico();
    void atualizarTabelaClientes();
    void limparCampos();
    void popularDadosTeste();

    Estacionamento estacionamento;

    QLineEdit* inputPlaca;
    QComboBox* comboTipo;
    QPushButton* btnEntrada;
    QPushButton* btnSaida;
    QLabel* lblStatus;
    QLabel* lblVagasLivres;
    QLabel* lblEstrategia;

    std::vector<QPushButton*> botoesVagas;

    QTableWidget* tabelaHistorico;

    QLineEdit* inputNomeCliente;
    QLineEdit* inputPlacaCliente;
    QComboBox* comboPlano;
    QPushButton* btnCadastrarCliente;
    QTableWidget* tabelaClientes;
    QTabWidget* tabs;
};

#endif // MAINWINDOW_H
