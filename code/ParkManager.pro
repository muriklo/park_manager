QT += core gui widgets

CONFIG += c++17

TARGET = ParkManager
TEMPLATE = app

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    Estacionamento.cpp \
    Transacao.cpp \
    Carro.cpp \
    Moto.cpp \
    Veiculo.cpp \
    EstrategiaAlocacao.cpp \
    Assinatura.cpp \
    Cliente.cpp \
    TarifaVeiculo.cpp

HEADERS += \
    MainWindow.h \
    Estacionamento.h \
    EstrategiaAlocacao.h \
    Veiculo.h \
    Carro.h \
    Moto.h \
    Vaga.h \
    Transacao.h \
    Assinatura.h \
    Cliente.h \
    TarifaVeiculo.h
