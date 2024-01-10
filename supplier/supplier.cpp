#include "supplier.h"
#include "ui_supplier.h"
#include "supplybook/supplybook.h"
#include "ui_supplybook.h"
#include "database/database.h"
#include "searchbook/searchbook.h"

Supplier::Supplier(QString pAccountName,QWidget *parent) : QWidget(parent), ui(new Ui::Supplier) {
    ui->setupUi(this);
    accountName = pAccountName;
    this->on_pushButton_refresh_clicked();
}

Supplier::~Supplier(){delete ui;}

void Supplier::on_pushButton_search_book_clicked() {
    SearchBook searchBook;
    ui->tableView_books->setModel(searchBook.search(ui->comboBox->currentText(), ui->lineEdit_search_book->text()));
    ui->tableView_books->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void Supplier::refresh(){
    on_pushButton_refresh_clicked();
}

void Supplier::on_pushButton_refresh_clicked() {
    this->on_pushButton_search_book_clicked();

    Database connection ;
    connection.openConnection();
    QSqlQuery query;
    QSqlQueryModel* databaseModel = new QSqlQueryModel;
    query.exec("SELECT Title as 标题, Author as 作者, Publisher as 出版社, Year as 年份, Quantity as 数量, ISBN, Book_ID as 书籍ID FROM Supplied_books");
    databaseModel->setQuery(query);
    ui->tableView_supplied_books->setModel(databaseModel);
    connection.closeConnection();
}

void Supplier::on_pushButton_supply_book_clicked() {
    SupplyBook connection;
    connection.exec();
    this->on_pushButton_refresh_clicked();
}

void Supplier::on_pushButton_remove_supply_clicked() {
    QModelIndex index = ui->tableView_supplied_books->currentIndex();
    if (!index.isValid()) QMessageBox::about(this,"错误", "请选择要移除的供给");
    else {
        int row = index.row();
        int id = index.sibling(row, 0).data().toInt();

        Database connection;
        connection.openConnection();
        QSqlQuery query;
        query.prepare("delete from Supplied_books where Book_ID = :id");
        query.bindValue(":id", id);
        QMessageBox delete_book;
        QPushButton *ok = delete_book.addButton(tr("移除") , QMessageBox::ActionRole);
        delete_book.addButton(tr("取消"), QMessageBox::NoRole);

        delete_book.setWindowTitle("警告！");
        delete_book.setText("供给将被移除！\n是否确认移除？");
        delete_book.exec();
        if(delete_book.clickedButton()==ok){
            query.exec();
            this->on_pushButton_refresh_clicked();
        }
        connection.closeConnection();
    }
}

void Supplier::on_pushButton_modify_supply_clicked() {
    QModelIndex index = ui->tableView_supplied_books->currentIndex();
    if(!index.isValid()) QMessageBox::about(this,"错误", "请选择要编辑的供给！");
    else {
        SupplyBook connection(index, 0);
        connection.exec();
        this->on_pushButton_refresh_clicked();
    }
}
