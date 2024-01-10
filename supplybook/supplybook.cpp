#include "supplybook.h"
#include "ui_supplybook.h"
#include "database/database.h"
#include "ui_supplier.h"
#include "supplier/supplier.h"
#include <QPushButton>

SupplyBook::SupplyBook(QWidget *parent) : QDialog(parent), ui(new Ui::SupplyBook) {ui->setupUi(this);}

SupplyBook::SupplyBook(QModelIndex &index,QWidget *parent) : QDialog(parent), ui(new Ui::SupplyBook) {
    ui->setupUi(this);
    idbook = index.sibling(index.row(), 6).data().toInt();
    this->setWindowTitle("编辑供给");
    ui->buttonBox->close();
    QPushButton *updateButton = new QPushButton("更新");
    ui->formLayout->addWidget(updateButton);
    ui->lineEdit_title->setText(index.sibling(index.row(), 0).data().toString());
    ui->lineEdit_author->setText(index.sibling(index.row(), 1).data().toString());
    ui->lineEdit_isbn->setText(index.sibling(index.row(), 5).data().toString());
    ui->lineEdit_publisher->setText(index.sibling(index.row(), 2).data().toString());
    ui->spinBox_quantity->setValue(index.sibling(index.row(), 4).data().toInt());
    ui->spinBox_year->setValue(index.sibling(index.row(), 3).data().toInt());
    connect(updateButton, &QPushButton::clicked, this, &SupplyBook::on_button_modify_supply_clicked);
}

SupplyBook::~SupplyBook() {delete ui;}

void SupplyBook::on_buttonBox_accepted() {
    if (ui->lineEdit_title->text()=="" || ui->lineEdit_author->text()=="" || ui->lineEdit_isbn->text()=="" || ui->lineEdit_publisher->text()=="") {
        QMessageBox::about(this,"失败！","存在空白字段！");
    } else if (ui->spinBox_quantity->text().toInt() < 1) {QMessageBox::about(this,"失败！","书籍数量必须为正整数！");}
    else if (ui->lineEdit_isbn->text().length() > 13) {QMessageBox::about(this,"失败！","ISBN码不能超过13位！");}
    else {
        Database connection;
        connection.openConnection();
        QSqlQuery query;

        query.exec("SELECT * FROM Supplied_books");
        int id = 1;
        if(query.last()) id = query.value(0).toInt() + 1;

        query.prepare("insert into Supplied_books values(:Book_ID, :Title, :Author, :ISBN, :Publisher, :Year, :Quantity);");
        query.bindValue(":Book_ID",id);
        query.bindValue(":Title",ui->lineEdit_title->text());
        query.bindValue(":Author",ui->lineEdit_author->text());
        query.bindValue(":ISBN",ui->lineEdit_isbn->text());
        query.bindValue(":Publisher",ui->lineEdit_publisher->text());
        query.bindValue(":Year",ui->spinBox_year->text().toInt());
        query.bindValue(":Quantity",ui->spinBox_quantity->text().toInt());
        if (query.exec()) {
            QMessageBox::about(this,"成功！","供给已成功添加！");
            this->close();
        }
    }
}

void SupplyBook::on_buttonBox_rejected() {this->close();}

void SupplyBook::on_button_modify_supply_clicked(){
    if (ui->lineEdit_author->text() == "" || ui->lineEdit_isbn->text() == "" || ui->lineEdit_publisher->text() == "" || ui->lineEdit_title->text() == ""){
        QMessageBox::about(this,"失败！", "存在空白字段！");
    } else if (ui->spinBox_quantity->text().toInt() < 1) {QMessageBox::about(this, "失败！", "书籍数量必须为正整数！");}
    else if (ui->lineEdit_isbn->text().length() > 13) {QMessageBox::about(this, "失败！", "ISBN码不能超过13位！");}
    else {
        Database connection;
        connection.openConnection();
        QSqlQuery query;
        query.prepare("update Supplied_books set Title = :newTitle, Author = :newAuthor, ISBN = :newISBN, Publisher = :newPublisher, Year = :newYear, Quantity = :newQuantity"
                        " where Book_ID = :id");
        query.bindValue(":newTitle",ui->lineEdit_title->text());
        query.bindValue(":newAuthor",ui->lineEdit_author->text());
        query.bindValue(":newISBN",ui->lineEdit_isbn->text());
        query.bindValue(":newPublisher",ui->lineEdit_publisher->text());
        query.bindValue(":newYear",ui->spinBox_year->text().toInt());
        query.bindValue(":newQuantity",ui->spinBox_quantity->text().toInt());
        query.bindValue(":id",idbook);
        if (query.exec()) {
            QMessageBox::about(this,"成功！", "供给已被修改！");
            this->close();
        }
    }
}
