/**
 * @file main.cpp
 * @brief Ponto de entrada da aplicação Park Manager.
 */
#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QFont font("Segoe UI", 10);
  app.setFont(font);

  MainWindow janela;
  janela.show();

  return app.exec();
}
