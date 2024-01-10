#include "user.h"
#include "ui_user.h"
#include "signup/signup.h"
#include "database/database.h"
#include "signin/signin.h"
#include "searchbook/searchbook.h"

User::User(QString pAccountName,QWidget *parent) : QWidget(parent), ui(new Ui::User) {
    ui->setupUi(this);
    accountName = pAccountName;
    this->on_pushButton_refresh_clicked();
}

User::~User() {delete ui;}

void User::on_pushButton_search_book_clicked() {
    SearchBook searchBook;
    ui->tableView_books->setModel(searchBook.search(ui->comboBox->currentText(), ui->lineEdit_search_book->text()));
    ui->tableView_books->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void User::refresh(){
    on_pushButton_refresh_clicked();
}

void User::on_pushButton_refresh_clicked() {
    this->on_pushButton_search_book_clicked();

    Database connection ;
    connection.openConnection();
    QSqlQuery query;
    QSqlQueryModel *databaseModel = new QSqlQueryModel;
    query.prepare("select Book_ID as 书籍编号, Title as 标题, Quantity as 数量, Borrow_date as 借书时间, Duration as 借阅期限 from Borrowed_books where User = :User and Status = 'true'  ");
    query.bindValue(":User", accountName);
    query.exec();

    databaseModel->setQuery(query);
    ui->tableView_borrowed_books->setModel(databaseModel);
    connection.closeConnection();
}

void User::on_pushButton_borrow_book_clicked() {
    QModelIndex index = ui->tableView_books->currentIndex();
    if (!index.isValid()) {
        QMessageBox::about(this,"错误", "请先选择一本书！");
    } else {
        QString now =  QDateTime::currentDateTime().toString("yyyy/MM/dd");
        QString title = index.sibling(index.row(), 0).data().toString();
        QString status = "true";
        int id = index.sibling(index.row(), 6).data().toInt();
        int quantity = index.sibling(index.row(), 4).data().toInt();

        Database connection;
        connection.openConnection();
        QSqlQuery query;

        query.prepare("Select Book_ID FROM Borrowed_books WHERE User = :user AND Book_ID = :id");
        query.bindValue(":user", accountName);
        query.bindValue(":id", id);

        if (query.exec() && query.next() && id == query.value(0).toInt()) {
            QMessageBox::about(this,"失败！","你只能同时借阅本书的一个副本！\n备注：可能是图书管理员尚未在后台通过你的还书申请！");
            connection.closeConnection();
            return;
        } else if (quantity == 1) {
            QMessageBox::about(this,"最后一本了！","本书只能在馆内阅读！");
            connection.closeConnection();
            return;
        } else if (quantity < 1) {
            QMessageBox::about(this,"缺货！","无可供借阅的副本！\n请联系图书管理员！");
            connection.closeConnection();
            return;
        }

        QMessageBox::StandardButton confirm = QMessageBox::question(this,"","是否借阅此书？",QMessageBox::Yes|QMessageBox::No);
        if (confirm == QMessageBox::Yes) {
            query.prepare("INSERT INTO Borrowed_books (Status,User,Book_ID,Title,Quantity,Borrow_date,Duration) "
                          "VALUES(:status,:user,:id,:title,:quantity,:date,:duration);");
            query.bindValue(":status", status);
            query.bindValue(":title", title);
            query.bindValue(":user", accountName);
            query.bindValue(":id", id);
            query.bindValue(":quantity", 1);
            query.bindValue(":date", now);
            query.bindValue(":duration", 30);
            query.exec();

            query.prepare("update Books set Quantity = :quantity where Book_ID = :id");
            query.bindValue(":id", id);
            query.bindValue(":quantity", --quantity);
            query.exec();
        }
        connection.closeConnection();
    }
    this->on_pushButton_refresh_clicked();
}

void User::on_pushButton_return_book_clicked() {
    QModelIndex index = ui->tableView_borrowed_books->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this,"", "未选择任何书籍！");
    } else {
        QString title = index.sibling(index.row(), 1).data().toString();
        QString now = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");

        Database connection;
        connection.openConnection();
        QSqlQuery query;
        QMessageBox::StandardButton confirm = QMessageBox::question(this,"","是否归还此书？",QMessageBox::Yes|QMessageBox::No);
        if (confirm == QMessageBox::Yes) {
            query.prepare("update  Borrowed_books set Status = 'false' where Book_ID = :id AND User = :user");
            query.bindValue(":id",index.sibling(index.row(), 0).data().toInt());
            query.bindValue(":user", accountName);
            query.exec();
        }
        connection.closeConnection();
    }
    this->on_pushButton_refresh_clicked();
}
