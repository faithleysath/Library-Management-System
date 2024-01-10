#include "cancelreserve.h"
#include "ui_cancelreserve.h"
#include "database/database.h"
#include "ui_library_worker.h"
#include "library_worker/library_worker.h"
#include <QPushButton>

CancelReserve::CancelReserve(QWidget *parent) : QDialog(parent), ui(new Ui::CancelReserve) {ui->setupUi(this);}
CancelReserve::CancelReserve(QModelIndex &  index,QWidget *parent) : QDialog(parent), ui(new Ui::CancelReserve) {
    ui->setupUi(this);
    idbook = index.sibling(index.row(), 7).data().toInt();
    quantity = index.sibling(index.row(), 4).data().toInt();
    reason = index.sibling(index.row(), 3).data().toString();
    this->setWindowTitle("取消保留书籍");
    ui->spinBox_quantity->setValue(index.sibling(index.row(), 4).data().toInt());
}

CancelReserve::~CancelReserve() {delete ui;}

void CancelReserve::on_buttonBox_accepted() {
    if(ui->spinBox_quantity->text().toInt() == 0 ) QMessageBox::about(this, "失败！"," 不能退回0本书！");
    else if(ui->spinBox_quantity->text().toInt() > quantity) QMessageBox::about(this, "失败！"," 退回数量超出保留数量！");
    else {
        Database connection;
        connection.openConnection();
        QSqlQuery query;

        query.prepare("SELECT Quantity FROM Books  WHERE Book_ID = :id");
        query.bindValue(":id", idbook);
        query.exec();
        query.first();
        books_quantity = query.value(0).toInt();
        query.prepare("Update Books SET Quantity = :quantity WHERE Book_ID = :id ");
        query.bindValue(":id", idbook);
        query.bindValue(":quantity", ui->spinBox_quantity->text().toInt() + books_quantity);
        query.exec();
        if (ui->spinBox_quantity->text().toInt() - quantity == 0) {
            query.prepare("Delete from Reserved_books  WHERE Book_ID = :id AND Reason = :reason ");
            query.bindValue(":id", idbook);
            query.bindValue(":reason", reason);
            query.exec();
        } else {
            query.prepare("Update Reserved_books SET Quantity = :sum Where Book_ID = :id AND Reason = :reason");
            query.bindValue(":id", idbook);
            query.bindValue(":reason", reason);
            query.bindValue(":sum", quantity-ui->spinBox_quantity->text().toInt());
            query.exec();
        }

        QMessageBox::about(this, "成功！","书籍退回成功！");
        this->close();
    }
}

void CancelReserve::on_buttonBox_rejected() {this->close();}
