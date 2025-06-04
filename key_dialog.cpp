#include "key_dialog.h"
#include "ui_key_dialog.h"
#include "database.h"

key_dialog::key_dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::key_dialog)
{
    ui->setupUi(this);
}

key_dialog::~key_dialog()
{
    delete ui;
}

void key_dialog::on_pushButton_clicked()
{
    Database db;
    db.openDatabase("database.db");
    QString line = ui->lineEdit->text();
    db.insert_Key(line);
    db.close_conn();
    this->close();
}

