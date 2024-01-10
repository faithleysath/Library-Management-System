#include "mainwindow/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {

    QApplication application(argc, argv);
    MainWindow *mainWindow = new MainWindow();
    mainWindow->setWindowTitle("图书管理系统");
    mainWindow->show();
    return application.exec();
}
