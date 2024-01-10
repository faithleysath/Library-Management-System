#include "signup.h"
#include "ui_signup.h"
#include "database/database.h"
#include <QMessageBox>
#include <QSql>
#include <QDateTime>

SignUp::SignUp(QWidget *parent) : QDialog(parent), ui(new Ui::SignUp) {
    ui->setupUi(this);
    this->setFixedSize(435,543);
    QString rules = "南信大图书馆会员注册及借阅规则\n\n"
                    "1. 注册\n\n"
                    "1.1 南信大图书馆会员注册对于居住在南信大的教职工子女是免费的。\n\n"
                    "1.2 年龄不满14岁的申请人必须由其父母或监护人签署会员申请表。\n\n"
                    "1.3 如有需要，年龄不满14岁的申请人也可以由其老师签署会员申请表。\n\n"
                    "2. 借阅\n\n"
                    "2.1 每位会员最多可借阅10本图书和4本杂志。\n\n"
                    "2.2 借阅期限为4周。\n\n"
                    "2.3 除了\"新书\"外，借阅的材料可以亲自续借一次。\n\n"
                    "3. 归还\n\n"
                    "3.1 所借阅的材料必须在规定的期限内归还到图书馆。\n\n"
                    "4. 逾期罚款\n\n"
                    "4.1 每逾期一天和每个逾期文件的罚款标准如下：\n\n"
                    "   - 青少年（0至13岁）和老年人（65岁及以上）：每逾期一天0.70元人民币。\n"
                    "   - 成年人（14至64岁）：每逾期一天1.75元人民币。\n\n"
                    "5. 丢失或损坏\n\n"
                    "5.1 会员必须赔偿图书馆丢失或损坏文件的价格。\n\n"
                    "5.2 如适用，会员还需支付逾期罚款以及35.00元人民币的行政费用。\n\n"
                    "6. 会员卡更换\n\n"
                    "6.1 青少年和老年人会员卡更换费用为14.00元人民币。\n"
                    "6.2 成年人会员卡更换费用为21.00元人民币.";

    ui->plainTextEdit_rules->setPlainText(rules);
    ui->plainTextEdit_rules->setTextInteractionFlags(Qt::TextSelectableByKeyboard);
}

SignUp::~SignUp() {delete ui;}

void SignUp::on_pushButton_cancel_clicked() {this->close();}

void SignUp::on_pushButton_ok_clicked() {
    Database connection;
    if(!connection.openConnection()){
        QMessageBox::critical(this,"Database Error!","Error Connecting to Database! Please try again or Check Database.");
        return;
    }
    QSqlQuery query;

    QString accountName =  ui->lineEdit_accountname->text();
    QString accountPassword = ui->lineEdit_password -> text();
    QString confirmPassword = ui->lineEdit_confirm_password -> text();
    QString email = ui->lineEdit_email->text() + '@' + ui->lineEdit_email2->text() + ui->comboBox_domen->currentText();
    QString fullName =  ui->lineEdit_fullname -> text();
    QString dayOfBirth = ui->spinBox_day->text() + '/' + ui->spinBox_month->text() +'/'+ ui->spinBox_year->text() ;
    if (accountName == "" || accountPassword  == "" || confirmPassword == "" || email == "" || fullName == "") {
        QMessageBox :: warning (this, "错误", "必须填写所有内容！");
    } else if(query.exec("SELECT User_name FROM Accounts WHERE User_name ='" + accountName +"';") && query.next())
        QMessageBox :: warning (this, "错误"," 用户名已被占用！");
    else if (confirmPassword != accountPassword) {
        QMessageBox :: warning (this, "错误", "两次输入的密码不一致！");
        ui->lineEdit_password ->setText ("");
        ui->lineEdit_confirm_password ->setText ("");
    } else if (!(ui->check_box_read->isChecked())) QMessageBox::warning(this,"错误"," 请阅读并同意图书馆规则！");
    else {
        query.prepare("INSERT INTO Users VALUES(:Full_name, :User_name, :Email, :DOB);");
        query.bindValue(":Full_name", fullName);
        query.bindValue(":Email", email);
        query.bindValue(":DOB", dayOfBirth);
        query.bindValue(":User_name", accountName);
        query.exec();

        query.prepare("INSERT INTO Accounts VALUES(:ID_card, :User_name, :Password, :Role_ID, :Active);");
        query.bindValue(":User_name", accountName);
        query.bindValue(":Password", accountPassword);
        query.bindValue(":Role_ID", 1);
        query.bindValue(":Active", "false");
        query.exec();
        QMessageBox :: about (this, "恭喜！","您已成功注册！\n"
                                                        "请联系图书馆工作人员来激活您的账户！");
        connection.closeConnection();
        this->close();
    }
}
