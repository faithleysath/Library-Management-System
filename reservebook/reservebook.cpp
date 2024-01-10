#include "reservebook.h"
#include "ui_reservebook.h"
#include "database/database.h"
#include "ui_library_worker.h"
#include "library_worker/library_worker.h"
#include <QPushButton>

ReserveBook::ReserveBook(QWidget *parent) : QDialog(parent), ui(new Ui::ReserveBook) {ui->setupUi(this);}

ReserveBook::ReserveBook(QModelIndex &index,QWidget *parent) : QDialog(parent), ui(new Ui::ReserveBook) {
    ui->setupUi(this);
    title = index.sibling(index.row(), 0).data().toString();
    author = index.sibling(index.row(), 1).data().toString();
    publisher = index.sibling(index.row(), 2).data().toString();
    year = index.sibling(index.row(), 3).data().toInt();
    quantity = index.sibling(index.row(), 4).data().toInt();
    isbn = index.sibling(index.row(), 5).data().toString();
    idbook = index.sibling(index.row(), 6).data().toInt();
    this->setWindowTitle("保留书籍");
    ui->lineEdit_reason->setText("");
    ui->spinBox_quantity->setValue(index.sibling(index.row(), 4).data().toInt());
}

ReserveBook::~ReserveBook() {delete ui;}

void ReserveBook::on_buttonBox_accepted() {
    if (ui->lineEdit_reason->text() == "" ) QMessageBox::about(this, "失败！", " 请填入保留原因！");
    else if (ui->spinBox_quantity->text().toInt() == 0 ) QMessageBox::about(this, "失败！", " 请至少保留一本书！");
    else if(ui->spinBox_quantity->text().toInt() > quantity ) QMessageBox::about(this, "失败！", " 保留数量超出存量！");
    else {
        Database connection;
        connection.openConnection();
        QSqlQuery query;
        query.prepare("INSERT INTO Reserved_books (Book_ID,Title,Author,ISBN,Publisher,Year,Quantity,Reason) "
                      "VALUES(:Book_ID, :Title, :Author, :ISBN, :Publisher, :Year, :Quantity, :Reason);");
        query.bindValue(":Book_ID", idbook);
        query.bindValue(":Title", title);
        query.bindValue(":Author", author);
        query.bindValue(":ISBN", isbn);
        query.bindValue(":Publisher", publisher);
        query.bindValue(":Year", year);
        query.bindValue(":Quantity", ui->spinBox_quantity->text().toInt());
        query.bindValue(":Reason", ui->lineEdit_reason->text());
        query.exec();
        query.prepare("Update Books SET Quantity = :sum Where Book_ID = :id");
        query.bindValue(":id", idbook);
        query.bindValue(":sum", quantity-ui->spinBox_quantity->text().toInt());
        if (query.exec()) QMessageBox::about(this,"成功！", "书籍保留成功！");

        this->close();
        connection.closeConnection();
    }
}

void ReserveBook::on_buttonBox_rejected() {this->close();}
