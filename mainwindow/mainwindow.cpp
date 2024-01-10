#include "database/database.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "signup/signup.h"
#include "ui_supplier.h"
#include "ui_library_worker.h"
#include "ui_user.h"
#include "user/user.h"
#include "supplier/supplier.h"
#include "ui_home.h"
#include "home/home.h"
#include "library_worker/library_worker.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSql>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setFixedSize(715,650);
    ui->tabWidget->close();
    ui->toolBar->close();
    Home *home = new Home();
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(home);
    QWidget *p = new QWidget();
    p->setLayout(layout);
    setCentralWidget(p);
}

MainWindow::MainWindow(int roleId, QString name, QWidget *par) : QMainWindow(par), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->toolBar_2->close();
    accountName = name;
    if (roleId==1) {
        User *pUser = new User(accountName);
        ui->tabWidget->addTab(pUser, QString("用户"));
    }
    if(roleId==2) {
        LibraryWorker *pLibraryWorker = new LibraryWorker();
        ui->tabWidget->addTab(pLibraryWorker, QString("图书管理员"));
    }
    if(roleId==3) {
        Supplier *pSupplier = new Supplier(accountName);
        ui->tabWidget->addTab(pSupplier, QString("供应商"));
    }
    if(roleId==4) {
        User *pUser = new User(accountName);
        ui->tabWidget->addTab(pUser, QString("用户"));
        LibraryWorker *pLibraryWorker = new LibraryWorker();
        ui->tabWidget->addTab(pLibraryWorker, QString("图书管理员"));
        Supplier *pSupplier = new Supplier(accountName);
        ui->tabWidget->addTab(pSupplier, QString("供应商"));
    }
}

MainWindow::~MainWindow() {delete ui;}

void MainWindow::on_actionSignUp_triggered() {
    SignUp  signUp;
    signUp.exec();
}

void MainWindow::on_actionSignIn_triggered() {
    this->deleteLater();
    SignIn signIn;
    signIn.exec();
}

void MainWindow::on_actionSignOut_triggered() {this->deleteLater();}

void MainWindow::on_actionHome_triggered() {
    this->deleteLater();
    MainWindow * mainWindow = new MainWindow();
    mainWindow->setWindowTitle("主页");
    mainWindow->show();
}

void MainWindow::on_actionabout_triggered() {
    QMessageBox::information(this, "关于作者", "<p>吴天一</p><p>202283250010</p>");
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    QString tabname = ui->tabWidget->tabText(index);
    QWidget *currentWidget = ui->tabWidget->widget(index);
    if(tabname == "用户"){
        User *userWidget = dynamic_cast<User*>(currentWidget);
        userWidget->refresh();
    }else if(tabname == "供应商"){
        Supplier *supplierWidget = dynamic_cast<Supplier*>(currentWidget);
        supplierWidget->refresh();
    }else if(tabname == "图书管理员"){
        LibraryWorker *workerWidget = dynamic_cast<LibraryWorker*>(currentWidget);
        workerWidget->refresh();
    }
}

