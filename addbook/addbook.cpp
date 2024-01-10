#include "addbook.h"
#include "ui_addbook.h"
#include "database/database.h"
#include "ui_library_worker.h"
#include "library_worker/library_worker.h"
#include <QPushButton>
#include <QString>

AddBook::AddBook(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddBook){ui->setupUi(this);}

AddBook::AddBook(QModelIndex &index,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddBook) {
    ui->setupUi(this);
    idBook = index.sibling(index.row(), 6).data().toInt();
    this->setWindowTitle("编辑书籍");
    ui->buttonBox->close();
    QPushButton * button = new QPushButton("更新");
    ui->formLayout->addWidget(button);
    ui->lineEdit_title->setText(index.sibling(index.row(), 0).data().toString());
    ui->lineEdit_author->setText(index.sibling(index.row(), 1).data().toString());
    ui->lineEdit_isbn->setText(index.sibling(index.row(), 5).data().toString());
    ui->lineEdit_publisher->setText(index.sibling(index.row(), 2).data().toString());
    ui->spinBox_quantity->setValue(index.sibling(index.row(), 4).data().toInt());
    ui->spinBox_year->setValue(index.sibling(index.row(), 3).data().toInt());
    connect(button, &QPushButton::clicked, this, &AddBook::on_button_modify_book_clicked);
}

AddBook::~AddBook() {delete ui;}

void AddBook::on_buttonBox_accepted() {
    if (ui->lineEdit_title->text()=="" || ui->lineEdit_author->text()=="" || ui->lineEdit_isbn->text()=="" || ui->lineEdit_publisher->text()=="")
        QMessageBox::about(this,"失败！"," 请填写所有字段！");
    else if (ui->spinBox_quantity->text().toInt() < 1) QMessageBox::about(this,"失败！","书籍数量必须为正整数！");
    else if (ui->lineEdit_isbn->text().length() > 13) QMessageBox::about(this,"失败！","ISBN码不能超过13位！");
    else {
        Database connection;
        connection.openConnection();
        QSqlQuery query;
        int id = 1;
        query.exec("select * from Books");
        if (query.last()) id = query.value(0).toInt() + 1;
        query.prepare("insert into Books values(:Book_ID, :Title, :Author, :ISBN, :Publisher, :Year, :Quantity);");
        query.bindValue(":Book_ID",id);
        query.bindValue(":Title",ui->lineEdit_title->text());
        query.bindValue(":Author",ui->lineEdit_author->text());
        query.bindValue(":ISBN",ui->lineEdit_isbn->text());
        query.bindValue(":Publisher",ui->lineEdit_publisher->text());
        query.bindValue(":Year",ui->spinBox_year->text().toInt());
        query.bindValue(":Quantity",ui->spinBox_quantity->text().toInt());
        if (query.exec()) {
            QMessageBox::about(this,"成功！","书籍添加成功！");
            this->close();
        }
        connection.closeConnection();
    }
}

void AddBook::on_buttonBox_rejected() {this->close();}

void AddBook::on_button_modify_book_clicked() {
    if (ui->lineEdit_author->text() == "" || ui->lineEdit_isbn->text() == "" || ui->lineEdit_publisher->text() == "" || ui->lineEdit_title->text() == "")
        QMessageBox::about(this,"失败！","请确保字段非空！");
    else if (ui->spinBox_quantity->text().toInt() < 1) QMessageBox::about(this,"失败！","书籍数量必须为正整数！");
    else if (ui->lineEdit_isbn->text().length() > 13) QMessageBox::about(this,"失败！","ISBN码不能超过13位！");
    else {
        Database connection;
        connection.openConnection();
        QSqlQuery query;
        query.prepare("update Books set Title = :newTitle, Author = :newAuthor, ISBN = :newISBN, Publisher = :newPublisher, Year = :newYear, Quantity = :newQuantity"
                      " where Book_ID = :id");
        query.bindValue(":newTitle",ui->lineEdit_title->text());
        query.bindValue(":newAuthor",ui->lineEdit_author->text());
        query.bindValue(":newISBN",ui->lineEdit_isbn->text());
        query.bindValue(":newPublisher",ui->lineEdit_publisher->text());
        query.bindValue(":newYear",ui->spinBox_year->text().toInt());
        query.bindValue(":newQuantity",ui->spinBox_quantity->text().toInt());
        query.bindValue(":id",idBook);
        if (query.exec()) {
            QMessageBox::about(this,"成功！","书籍已被更新！");
            this->close();
        }
    }
}
