#include "database.h"
#include <QSqlDatabase>
#include <QSqlQuery>

Database::Database() {
    if (!db.open()){
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(QCoreApplication::applicationDirPath()+"/database.db");
        this->createTables();
        this->createInitialRecords();
    }
}

bool Database::openConnection(){
    db.open();
    db.exec("PRAGMA foreign_keys = ON");
    return db.open();
}

void Database::closeConnection(){
    db.close();
}

void Database::createTables() {
    this->openConnection();
    QSqlQuery query(db);

    QString createTableSql = "CREATE TABLE sqlite_sequence(name,seq);";
    query.exec(createTableSql);

    QString createBooksTableSql = "CREATE TABLE IF NOT EXISTS Books ("
                                  "Book_ID INTEGER NOT NULL,"
                                  "Title TEXT NOT NULL,"
                                  "Author TEXT NOT NULL,"
                                  "ISBN TEXT NOT NULL,"
                                  "Publisher TEXT NOT NULL,"
                                  "Year INTEGER NOT NULL,"
                                  "Quantity INTEGER NOT NULL CHECK(Quantity>=0),"
                                  "PRIMARY KEY(Book_ID)"
                                  ");";
    query.exec(createBooksTableSql);

    QString createSuppliedBooksTableSql = "CREATE TABLE IF NOT EXISTS Supplied_books ("
                                          "Book_ID INTEGER NOT NULL,"
                                          "Title TEXT NOT NULL,"
                                          "Author TEXT NOT NULL,"
                                          "ISBN TEXT NOT NULL,"
                                          "Publisher TEXT NOT NULL,"
                                          "Year INTEGER NOT NULL,"
                                          "Quantity INTEGER NOT NULL CHECK(Quantity>=0),"
                                          "PRIMARY KEY(Book_ID)"
                                          ");";
    query.exec(createSuppliedBooksTableSql);

    QString createUsersTableSql = "CREATE TABLE IF NOT EXISTS Users ("
                                  "Full_name TEXT NOT NULL,"
                                  "User_name TEXT NOT NULL,"
                                  "Email TEXT NOT NULL,"
                                  "DOB TEXT NOT NULL,"
                                  "PRIMARY KEY(User_name),"
                                  "FOREIGN KEY(User_name) REFERENCES Accounts(User_name)"
                                  ");";
    query.exec(createUsersTableSql);

    QString createBorrowedBooksTableSql = "CREATE TABLE IF NOT EXISTS Borrowed_books ("
                                          "Status TEXT NOT NULL,"
                                          "User TEXT NOT NULL,"
                                          "Book_ID INTEGER NOT NULL,"
                                          "Title TEXT NOT NULL,"
                                          "Quantity INTEGER NOT NULL,"
                                          "Borrow_date TEXT NOT NULL,"
                                          "Duration INTEGER NOT NULL,"
                                          "FOREIGN KEY(Book_ID) REFERENCES Books(Book_ID),"
                                          "FOREIGN KEY(User) REFERENCES Accounts(User_name),"
                                          "PRIMARY KEY(User, Book_ID)"
                                          ");";
    query.exec(createBorrowedBooksTableSql);

    QString createRolesTableSql = "CREATE TABLE IF NOT EXISTS Roles ("
                                  "Role_ID INTEGER NOT NULL,"
                                  "Role_title TEXT NOT NULL,"
                                  "PRIMARY KEY(Role_ID)"
                                  ");";
    query.exec(createRolesTableSql);

    QString createAccountsTableSql = "CREATE TABLE IF NOT EXISTS Accounts ("
                                     "ID_card INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                                     "User_name TEXT NOT NULL,"
                                     "Password TEXT NOT NULL,"
                                     "Role_ID INTEGER NOT NULL,"
                                     "Active NUMERIC NOT NULL,"
                                     "FOREIGN KEY(Role_ID) REFERENCES Roles(Role_ID)"
                                     ");";
    query.exec(createAccountsTableSql);

    QString createReservedBooksTableSql = "CREATE TABLE IF NOT EXISTS Reserved_books ("
                                          "Book_ID INTEGER NOT NULL,"
                                          "Title TEXT NOT NULL,"
                                          "Author TEXT NOT NULL,"
                                          "ISBN TEXT NOT NULL,"
                                          "Publisher TEXT NOT NULL,"
                                          "Year INTEGER NOT NULL,"
                                          "Quantity INTEGER NOT NULL CHECK(Quantity>=0),"
                                          "Reason TEXT NOT NULL,"
                                          "FOREIGN KEY(Book_ID) REFERENCES Books(Book_ID),"
                                          "PRIMARY KEY(Book_ID, Reason)"
                                          ");";
    query.exec(createReservedBooksTableSql);
    this->closeConnection();
}

void Database::createInitialRecords() {
    this->openConnection();
    QSqlQuery query(db);

    // 检查Roles表的默认记录是否存在
    QString checkRolesSql = "SELECT COUNT(*) FROM Roles;";
    query.exec(checkRolesSql);
    query.next();
    int roleCount = query.value(0).toInt();

    if (roleCount == 0) {
        // 插入Roles表的默认记录
        QString insertRolesSql = "INSERT INTO Roles (Role_ID, Role_title) VALUES "
                                 "(1, 'User'), "
                                 "(2, 'Library worker'), "
                                 "(3, 'Supplier'), "
                                 "(4, 'Administrator');";
        query.exec(insertRolesSql);
        // 插入Books表的初始记录
        QString insertBooksSql = "INSERT INTO Books (Book_ID, Title, Author, ISBN, Publisher, Year, Quantity) VALUES "
                                 "(1, '红楼梦', '曹雪芹', '9787020002207', '人民文学出版社', 1996, 10), "
                                 "(2, '活着', '余华', '9787506365437', '作家出版社', 2012, 5), "
                                 "(3, '1984', '乔治·奥威尔', '9787530210291', '北京十月文艺出版社', 2010, 0), "
                                 "(4, '哈利·波特', 'J.K.罗琳', '9787020096695', '人民文学出版社', 2008, 3), "
                                 "(5, '三体全集', '刘慈欣', '9787229042066', '重庆出版社', 2012, 4), "
                                 "(6, '百年孤独', '加西亚·马尔克斯', '9787544253994', '南海出版公司', 2011, 2), "
                                 "(7, '飘', '玛格丽特·米切尔', '9787806570920', '译林出版社', 2000, 3), "
                                 "(8, '房思琪的初恋乐园', '林奕含', '9787559614636', '北京联合出版公司', 2018, 1), "
                                 "(9, '动物农场', '乔治·奥威尔', '9787532741854', '上海译文出版社', 2007, 2), "
                                 "(10, '三国演义（全二册）', '罗贯中', '9787020008728', '人民文学出版社', 1998, 8);";
        query.exec(insertBooksSql);
        // 插入Accounts表的初始记录
        QString insertAccountsSql = "INSERT INTO Accounts (User_name, Password, Role_ID, Active) VALUES "
                                    "('user', 'password', 1, 'true'), "
                                    "('worker', 'password', 2, 'true'), "
                                    "('supplier', 'password', 3, 'true'), "
                                    "('admin', 'password', 4, 'true');";
        query.exec(insertAccountsSql);
        // 插入Users表的初始记录
        QString insertUsersSql = "INSERT INTO Users (Full_name, User_name, Email, DOB) VALUES "
                                 "('用户', 'user', 'user@example.com', '1990-01-01'), "
                                 "('图书管理员', 'worker', 'worker@example.com', '1990-01-02'), "
                                 "('供应商', 'supplier', 'supplier@example.com', '1990-01-03'), "
                                 "('超级管理员', 'admin', 'admin@example.com', '1990-01-04'); ";
        query.exec(insertUsersSql);
        //插入Borrowed_books表的初始记录

        //插入Supplied_books表的初始记录

        //插入Reserved_books表的初始记录
    }
    this->closeConnection();
}
