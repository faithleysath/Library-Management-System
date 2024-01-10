#include "searchbook.h"


SearchBook::SearchBook() {};

QSqlQueryModel* SearchBook::search(QString searchType, QString searchWord) {
    Database connection;
    connection.openConnection();
    QSqlQueryModel* databaseModel = new QSqlQueryModel;
    QSqlQuery query;

    if (searchType == "ISBN")
        query.exec("SELECT Title as 标题, Author as 作者, Publisher as 出版社, Year as 年份, Quantity as 数量, ISBN, Book_ID as 书籍ID FROM Books WHERE ISBN LIKE '%'||'" + searchWord + "'||'%';");
    else if (searchType == "标题")
        query.exec("SELECT Title as 标题, Author as 作者, Publisher as 出版社, Year as 年份, Quantity as 数量, ISBN, Book_ID as 书籍ID FROM Books WHERE Title LIKE '%'||'" + searchWord + "'||'%';");
    else if (searchType == "作者")
        query.exec("SELECT Title as 标题, Author as 作者, Publisher as 出版社, Year as 年份, Quantity as 数量, ISBN, Book_ID as 书籍ID FROM Books WHERE Author LIKE '%'||'" + searchWord + "'||'%';");
    else if (searchType == "年份")
        query.exec("SELECT Title as 标题, Author as 作者, Publisher as 出版社, Year as 年份, Quantity as 数量, ISBN, Book_ID as 书籍ID FROM Books WHERE Year = '" + searchWord + "';");
    else if (searchType == "出版社")
        query.exec("SELECT Title as 标题, Author as 作者, Publisher as 出版社, Year as 年份, Quantity as 数量, ISBN, Book_ID as 书籍ID FROM Books WHERE Publisher LIKE '%'||'" + searchWord + "'||'%';");
    else if (searchType == "所有书籍")
        query.exec("SELECT Title as 标题, Author as 作者, Publisher as 出版社, Year as 年份, Quantity as 数量, ISBN, Book_ID as 书籍ID FROM Books;");

    databaseModel->setQuery(query);
    connection.closeConnection();
    return databaseModel;
}
