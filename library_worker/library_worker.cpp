#include "library_worker.h"
#include "ui_library_worker.h"
#include "database/database.h"
#include "addbook/addbook.h"
#include "ui_addbook.h"
#include "reservebook/reservebook.h"
#include "ui_reservebook.h"
#include "cancelreserve/cancelreserve.h"
#include "ui_cancelreserve.h"
#include "searchbook/searchbook.h"

LibraryWorker::LibraryWorker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LibraryWorker) {
    ui->setupUi(this);
    this->on_pushButton_refresh_clicked();
}

LibraryWorker::~LibraryWorker() {delete ui;}

void LibraryWorker::on_pushButton_search_book_clicked() {
    SearchBook searchBook;
    ui->tableView_books->setModel(searchBook.search(ui->comboBox_2->currentText(), ui->lineEdit_search_book->text()));
    ui->tableView_books->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void LibraryWorker::refresh(){
    on_pushButton_refresh_clicked();
}

void LibraryWorker::on_pushButton_refresh_clicked() {
    Database connection;
    connection.openConnection();
    QSqlQuery query;
    QSqlQueryModel *returnBookRequestsModel = new QSqlQueryModel;
    QSqlQueryModel *membershipRequestsModel = new QSqlQueryModel;
    QSqlQueryModel *suppliedBooksModel = new QSqlQueryModel;
    QSqlQueryModel *reservedBooksModel = new QSqlQueryModel;

    query.exec("SELECT User as 用户, Book_ID as 书籍ID, Title as 标题, Quantity as 数量, Borrow_date as 借书时间, Duration as 借阅期限 FROM Borrowed_books WHERE Status = 'false'");
    returnBookRequestsModel->setQuery(query);
    ui->tableView_requests_for_return_book->setModel(returnBookRequestsModel);
    ui->tableView_requests_for_return_book->setSelectionBehavior(QAbstractItemView::SelectRows);

    query.exec("SELECT ID_Card as 实体卡, User_name as 用户名, Active as 激活状态 FROM Accounts WHERE Role_ID = 1 AND Active = 'false' ");
    membershipRequestsModel->setQuery(query);
    ui->tableView_membership->setModel(membershipRequestsModel);
    ui->tableView_membership->setSelectionBehavior(QAbstractItemView::SelectRows);

    query.exec("SELECT Title as 标题, Author as 作者, Publisher as 出版社, Year as 年份, Quantity as 数量, ISBN, Book_ID as 书籍ID FROM Supplied_books");
    suppliedBooksModel->setQuery(query);
    ui->tableView_supplied_books->setModel(suppliedBooksModel);
    ui->tableView_supplied_books->setSelectionBehavior(QAbstractItemView::SelectRows);

    query.exec("SELECT Title as 标题, Author as 作者, Publisher as 出版社, Reason as 原因, Quantity as 数量, Year as 年份, ISBN, Book_ID as 书籍ID FROM Reserved_books");
    reservedBooksModel->setQuery(query);
    ui->tableView_reserved_books->setModel(reservedBooksModel);
    ui->tableView_reserved_books->setSelectionBehavior(QAbstractItemView::SelectRows);

    connection.closeConnection();
    this->on_pushButton_search_book_clicked();
}

void LibraryWorker::on_pushButton_add_book_clicked() {
    AddBook newBook;
    newBook.exec();
    this->on_pushButton_refresh_clicked();
}

void LibraryWorker::on_pushButton_reserve_book_clicked() {
    QModelIndex index = ui->tableView_books->currentIndex();

    if (!index.isValid()) QMessageBox::about(this, "错误", "请先选择要保留的书籍！");
    else {
        ReserveBook reserveBook(index,0);
        reserveBook.exec();
        this->on_pushButton_refresh_clicked();
    }
}

void LibraryWorker::on_pushButton_cancel_reservation_clicked() {
    QModelIndex index = ui->tableView_reserved_books->currentIndex();

    if (!index.isValid()) QMessageBox::about(this, "错误", "请先选择要退回的书籍！");
    else {
        CancelReserve cancelReserve(index,0);
        cancelReserve.exec();
        this->on_pushButton_refresh_clicked();
    }
}

void LibraryWorker::on_pushButton_delete_book_clicked(){
    QModelIndex index = ui->tableView_books->currentIndex();

    if (!index.isValid()) QMessageBox::about(this, "错误", "请选择要删除的书籍！");
    else {
        int row = index.row();
        int id = index.sibling(row, 6).data().toInt();
        Database connection;
        connection.openConnection();
        QSqlQuery query;
        query.prepare("DELETE FROM Books WHERE Book_ID = :id");
        query.bindValue(":id", id);

        QMessageBox deleteBookMessageBox;
        QPushButton *ok = deleteBookMessageBox.addButton(tr("删除") ,QMessageBox::ActionRole);
        deleteBookMessageBox.addButton(tr("取消"),QMessageBox::NoRole);
        deleteBookMessageBox.setWindowTitle("警告！");
        deleteBookMessageBox.setText("书籍将从系统中删除\n您确定要删除吗？");
        deleteBookMessageBox.exec();
        if(deleteBookMessageBox.clickedButton() == ok) {
            query.exec();
        }
        connection.closeConnection();
        this->on_pushButton_refresh_clicked();
    }
}

void LibraryWorker::on_pushButton_modify_book_clicked() {
    QModelIndex index = ui->tableView_books->currentIndex();

    if (!index.isValid()) QMessageBox::about(this,"错误", "请选择要修改的书籍！");
    else {
        AddBook modifyBook(index, 0);
        modifyBook.exec();
        this->on_pushButton_refresh_clicked();
    }
}

void LibraryWorker::on_pushButton_confirm_book_return_clicked() {
    QModelIndex index = ui->tableView_requests_for_return_book->currentIndex();

    if (!index.isValid()) QMessageBox::about(this,"错误","未选择任何请求！");
    else {
        QString user = index.sibling(index.row(), 0).data().toString();
        Database connection;
        connection.openConnection();
        QSqlQuery query;

        query.prepare("SELECT Quantity FROM Books WHERE Book_ID = :id");
        query.bindValue(":id", index.sibling(index.row(), 1).data().toInt());
        query.exec();
        int quantity = 1;
        if (query.next()) quantity = query.value(0).toInt() + 1;

        QMessageBox::StandardButton confirm = QMessageBox::question(this,"","是否确定该用户已归还此书？",QMessageBox::Yes|QMessageBox::No);
        if (confirm == QMessageBox::Yes) {
            query.prepare("DELETE FROM Borrowed_books WHERE Book_ID = :id AND User = :user");
            query.bindValue(":id",index.sibling(index.row(), 1).data().toInt());
            query.bindValue(":user",user);
            query.exec();
            query.prepare("UPDATE Books SET Quantity = :quantity WHERE Book_ID = :id");
            query.bindValue(":id",index.sibling(index.row(), 1).data().toInt());
            query.bindValue(":quantity", quantity);
            query.exec();
        }
        connection.closeConnection();
        this->on_pushButton_refresh_clicked();
    }
}

void LibraryWorker::on_pushButton_cancel_book_return_clicked() {
    QModelIndex index = ui->tableView_requests_for_return_book->currentIndex();
    if (!index.isValid()) QMessageBox::about(this, "错误", "未选择任何请求！");
    else {
        int id = index.sibling(index.row(), 1).data().toInt();
        QString user = index.sibling(index.row(), 0).data().toString();

        Database connection;
        connection.openConnection();
        QSqlQuery query;
        query.prepare("UPDATE Borrowed_books SET Status = 'true' WHERE Book_ID = :id AND User = :user");
        query.bindValue(":id", id);
        query.bindValue(":user", user);

        QMessageBox::StandardButton confirm = QMessageBox::question(this, "再次确认！！", "你确定要驳回请求吗？", QMessageBox::Yes | QMessageBox::No);
        if (confirm == QMessageBox::Yes) query.exec();

        connection.closeConnection();
        this->on_pushButton_refresh_clicked();
    }
}

void LibraryWorker::on_pushButton_confirm_membership_clicked() {
    QModelIndex index = ui->tableView_membership->currentIndex();
    if (!index.isValid()) QMessageBox::warning(this,"","未选择任何项！");
    else {
        Database connection;
        connection.openConnection();
        QSqlQuery query;
        query.prepare("UPDATE Accounts SET Active = 'true' WHERE User_name = :user");
        query.bindValue(":user", index.sibling(index.row(), 1).data().toString());

        QMessageBox::StandardButton confirm = QMessageBox::question(this, "", "是否认可此用户的会员身份？", QMessageBox::Yes | QMessageBox::No);
        if(confirm == QMessageBox::Yes) query.exec();

        connection.closeConnection();
        this->on_pushButton_refresh_clicked();
    }
}

void LibraryWorker::on_pushButton_receive_book_clicked() {
    QModelIndex index = ui->tableView_supplied_books->currentIndex();
    if (!index.isValid()) QMessageBox::warning(this,"","未选择任何项！");
    else {
        Database connection;
        connection.openConnection();
        QSqlQuery query;

        query.exec("SELECT * FROM Books;");
        int bookID = 1;
        if (query.last()) bookID = query.value(0).toInt() + 1;

        int supplyID = index.sibling(index.row(), 6).data().toInt();
        QString title = index.sibling(index.row(), 0).data().toString();
        QString author = index.sibling(index.row(), 1).data().toString();
        QString isbn = index.sibling(index.row(), 5).data().toString();
        QString publisher = index.sibling(index.row(), 2).data().toString();
        int year = index.sibling(index.row(), 3).data().toInt();
        int quantity = index.sibling(index.row(), 4).data().toInt();

        if ( query.exec("SELECT Quantity, ISBN, Title, Author, Publisher, Year FROM Books WHERE ISBN = '" + isbn + "';")
             && query.first()) {
            books_quantity = query.value(0).toInt();
            book_isbn = query.value(1).toString();
            book_title = query.value(2).toString();
            book_author = query.value(3).toString();
            book_publisher = query.value(4).toString();
            books_year = query.value(5).toInt();
        }

        QMessageBox::StandardButton confirm = QMessageBox::question(this, "再次确认！！", "确定要接收这本书吗？", QMessageBox::Yes | QMessageBox::No);
        if (confirm == QMessageBox::Yes) {
            if (book_isbn == isbn && books_year == year && book_title == title && book_author == author && book_publisher == publisher) {
                query.prepare("UPDATE Books SET Quantity = :quantity WHERE ISBN = :isbn AND Title = :title");
                query.bindValue(":quantity", quantity + books_quantity);
                query.bindValue(":isbn", index.sibling(index.row(), 5).data().toString());
                query.bindValue(":title", index.sibling(index.row(), 0).data().toString());
            } else {
                query.prepare("INSERT INTO Books (Book_ID, Title, Author, ISBN, Publisher, Year, Quantity) "
                              "VALUES(:id, :title, :author, :isbn, :publisher, :year, :quantity);");
                query.bindValue(":id", bookID);
                query.bindValue(":title", title);
                query.bindValue(":author", author);
                query.bindValue(":isbn", isbn);
                query.bindValue(":publisher", publisher);
                query.bindValue(":year", year);
                query.bindValue(":quantity", quantity);
            }
            query.exec();
            query.prepare("DELETE FROM Supplied_books WHERE Book_ID = :id");
            query.bindValue(":id", supplyID);
            query.exec();
        }
        connection.closeConnection();
        this->on_pushButton_refresh_clicked();
    }
}
