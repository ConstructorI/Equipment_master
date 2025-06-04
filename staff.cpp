//staff.cpp
#include "staff.h"
#include "ui_staff.h"
#include "database.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QDateEdit>

staff::staff(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::staff)
{
    ui->setupUi(this);
    set_data();
}

staff::~staff()
{
    delete ui;
}

void staff::on_pushButton_metr_add_clicked()
{
    QHBoxLayout *hLayout = new QHBoxLayout();
    QLabel *label = new QLabel(this);
    QDateEdit *dateEdit = new QDateEdit(this);
    QLineEdit *lineEdit = new QLineEdit(this);

    dateEdit->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    dateEdit->setMinimumWidth(80);
    dateEdit->setMaximumWidth(80);
    dateEdit->calendarPopup();
    label->setText("c ");
    lineEdit->setPlaceholderText("Фамилия И.О.");
    lineEdit->setMinimumWidth(150);
    hLayout->addWidget(label);
    hLayout->addWidget(dateEdit);
    hLayout->addWidget(lineEdit);

    if (!layouts_metr_List.isEmpty()) {
        int size_of_metr_list;
        size_of_metr_list = layouts_metr_List.size() + 2;
        ui->verticalLayout->insertLayout(size_of_metr_list, hLayout);
    } else {
        ui->verticalLayout->insertLayout(2, hLayout);
    }
    layouts_metr_List.append(hLayout);
}

void staff::on_pushButton_metr_rem_clicked()
{
    if (!layouts_metr_List.isEmpty()) {
        QHBoxLayout *hLayout = layouts_metr_List.takeLast();
        QLayoutItem *item;

        while ((item = hLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        ui->verticalLayout->removeItem(hLayout);
        delete hLayout;
    }
}

void staff::on_pushButton_ruk_add_clicked()
{
    QHBoxLayout *hLayout = new QHBoxLayout();
    QLabel *label = new QLabel(this);
    QDateEdit *dateEdit = new QDateEdit(this);
    QLineEdit *lineEdit = new QLineEdit(this);

    dateEdit->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    dateEdit->setMinimumWidth(80);
    dateEdit->setMaximumWidth(80);
    dateEdit->calendarPopup();
    label->setText("c ");
    lineEdit->setPlaceholderText("Фамилия И.О.");
    lineEdit->setMinimumWidth(150);
    hLayout->addWidget(label);
    hLayout->addWidget(dateEdit);
    hLayout->addWidget(lineEdit);

    if (!layouts_ruk_List.isEmpty()) {
        int size_of_ruk_list;
        size_of_ruk_list = layouts_ruk_List.size() + 2;
        ui->verticalLayout_2->insertLayout(size_of_ruk_list, hLayout);
    } else {
        ui->verticalLayout_2->insertLayout(2, hLayout);
    }
    layouts_ruk_List.append(hLayout);
}

void staff::on_pushButton_ruk_rem_clicked()
{
    if (!layouts_ruk_List.isEmpty()) {
        QHBoxLayout *hLayout = layouts_ruk_List.takeLast();
        QLayoutItem *item;

        while ((item = hLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        ui->verticalLayout_2->removeItem(hLayout);
        delete hLayout;
    }
}

void staff::on_pushButton_gen_add_clicked()
{
    QHBoxLayout *hLayout = new QHBoxLayout();
    QLabel *label = new QLabel(this);
    QDateEdit *dateEdit = new QDateEdit(this);
    QLineEdit *lineEdit = new QLineEdit(this);

    dateEdit->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    dateEdit->setMinimumWidth(80);
    dateEdit->setMaximumWidth(80);
    dateEdit->calendarPopup();
    label->setText("c ");
    lineEdit->setPlaceholderText("Фамилия И.О.");
    lineEdit->setMinimumWidth(150);
    hLayout->addWidget(label);
    hLayout->addWidget(dateEdit);
    hLayout->addWidget(lineEdit);

    if (!layouts_gen_List.isEmpty()) {
        int size_of_gen_list;
        size_of_gen_list = layouts_gen_List.size() + 2;
        ui->verticalLayout_3->insertLayout(size_of_gen_list, hLayout);
    } else {
        ui->verticalLayout_3->insertLayout(2, hLayout);
    }
    layouts_gen_List.append(hLayout);
}

void staff::on_pushButton_gen_rem_clicked()
{
    if (!layouts_gen_List.isEmpty()) {
        QHBoxLayout *hLayout = layouts_gen_List.takeLast();
        QLayoutItem *item;

        while ((item = hLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        ui->verticalLayout_3->removeItem(hLayout);
        delete hLayout;
    }
}

QString staff::get_Text(QHBoxLayout *hLayout) const {
    QString result;

    if (!hLayout) {
        return result;
    }

    for (int i = 0; i < hLayout->count(); ++i) {
        QLayoutItem *item = hLayout->itemAt(i);
        if (!item) {
            continue;
        }

        QWidget *widget = item->widget();
        if (!widget) {
            continue;
        }

        QDateEdit *dateEdit = qobject_cast<QDateEdit *>(widget);
        if (dateEdit) {
            QString date = dateEdit->date().toString("dd.MM.yyyy");
            result += date;
            result += ";";
        } else {
            QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget);
            if (lineEdit) {
                result += lineEdit->text();
            }
        }
    }
    return result;
}

void staff::on_pushButton_save_clicked()
{
    QString str_name = ui->lineEdit_lab_name->text();
    QString adress = ui->lineEdit_adress->text();

    QString str_metr = ui->lineEdit_metr->text();
    for (int i = 0; i < layouts_metr_List.size(); i++){
        str_metr += "@";
        QHBoxLayout *hLayout = layouts_metr_List.at(i);
        str_metr += get_Text(hLayout);
    }
    qDebug() << str_metr;

    QString str_ruk = ui->lineEdit_ruk->text();
    for (int i = 0; i < layouts_ruk_List.size(); i++){
        str_ruk += "@";
        QHBoxLayout *hLayout = layouts_ruk_List.at(i);
        str_ruk += get_Text(hLayout);
    }
    qDebug() << str_ruk;

    QString str_gen = ui->lineEdit_gen->text();
    for (int i = 0; i < layouts_gen_List.size(); i++){
        str_gen += "@";
        QHBoxLayout *hLayout = layouts_gen_List.at(i);
        str_gen += get_Text(hLayout);
    }
    qDebug() << str_gen;

    Database db;
    if (db.openDatabase("database.db"))
    {
        QString metr = "Метролог";
        QString ruk = "Руководитель";
        QString gen = "Ген. дир.";
        db.insert_Info(str_name, adress);
        db.insert_Staff(metr, str_metr);
        db.insert_Staff_ruk(ruk, str_ruk);
        db.insert_Staff_gen(gen, str_gen);
        db.close_conn();
        this->close();
    }
}

void staff::set_data(){
    Database db;
    if (db.openDatabase("database.db"))
    {
        QStringList info;
        this->metr = db.get_data_metr();
        this->ruk = db.get_data_ruk();
        this->gen = db.get_data_gen();
        info = db.get_data_info();
        this->name = info.at(0);
        this->adress = info.at(1);
        db.close_conn();
    }
    ui->lineEdit_lab_name->setText(name);
    ui->lineEdit_adress->setText(adress);
    QStringList metr_list = this->metr.split("@");
    QStringList ruk_list = this->ruk.split("@");
    QStringList gen_list = this->gen.split("@");

    QString one_metr;
    one_metr = metr_list.at(0);
    ui->lineEdit_metr->setText(one_metr);
    QString one_ruk;
    one_ruk = ruk_list.at(0);
    ui->lineEdit_ruk->setText(one_ruk);
    QString one_gen;
    one_gen = gen_list.at(0);
    ui->lineEdit_gen->setText(one_gen);

    for (int x = 0; x < metr_list.size() - 1; ++x){
        QString one_metr;
        one_metr = metr_list.at(x + 1);
        QStringList one_param_metr = one_metr.split(";");

        QHBoxLayout *hLayout = new QHBoxLayout();
        QLabel *label = new QLabel(this);
        QDateEdit *dateEdit = new QDateEdit(this);
        QLineEdit *lineEdit = new QLineEdit(this);
        label->setText("c ");
        QDate date = QDate::fromString(one_param_metr.at(0), "dd.MM.yyyy");
        dateEdit->setDate(date);
        dateEdit->setMinimumWidth(80);
        dateEdit->setMaximumWidth(80);
        dateEdit->calendarPopup();
        lineEdit->setText(one_param_metr.at(1));
        lineEdit->setMinimumWidth(150);
        hLayout->addWidget(label);
        hLayout->addWidget(dateEdit);
        hLayout->addWidget(lineEdit);

        if (!layouts_metr_List.isEmpty()) {
            int size_of_metr_list;
            size_of_metr_list = layouts_metr_List.size() + 2;
            ui->verticalLayout->insertLayout(size_of_metr_list, hLayout);
        } else {
            ui->verticalLayout->insertLayout(2, hLayout);
        }
        layouts_metr_List.append(hLayout);
    }

    for (int x = 0; x < ruk_list.size() - 1; ++x){
        QString one_ruk;
        one_ruk = ruk_list.at(x + 1);
        QStringList one_param_ruk = one_ruk.split(";");

        QHBoxLayout *hLayout = new QHBoxLayout();
        QLabel *label = new QLabel(this);
        QDateEdit *dateEdit = new QDateEdit(this);
        QLineEdit *lineEdit = new QLineEdit(this);
        label->setText("c ");
        QDate date = QDate::fromString(one_param_ruk.at(0), "dd.MM.yyyy");
        dateEdit->setDate(date);
        dateEdit->setMinimumWidth(80);
        dateEdit->setMaximumWidth(80);
        dateEdit->calendarPopup();
        lineEdit->setText(one_param_ruk.at(1));
        lineEdit->setMinimumWidth(150);
        hLayout->addWidget(label);
        hLayout->addWidget(dateEdit);
        hLayout->addWidget(lineEdit);

        if (!layouts_ruk_List.isEmpty()) {
            int size_of_ruk_list;
            size_of_ruk_list = layouts_ruk_List.size() + 2;
            ui->verticalLayout_2->insertLayout(size_of_ruk_list, hLayout);
        } else {
            ui->verticalLayout_2->insertLayout(2, hLayout);
        }
        layouts_ruk_List.append(hLayout);
    }

    for (int x = 0; x < gen_list.size() - 1; ++x){
        QString one_gen;
        one_gen = gen_list.at(x + 1);
        QStringList one_param_gen = one_gen.split(";");

        QHBoxLayout *hLayout = new QHBoxLayout();
        QLabel *label = new QLabel(this);
        QDateEdit *dateEdit = new QDateEdit(this);
        QLineEdit *lineEdit = new QLineEdit(this);
        label->setText("c ");
        QDate date = QDate::fromString(one_param_gen.at(0), "dd.MM.yyyy");
        dateEdit->setDate(date);
        dateEdit->setMinimumWidth(80);
        dateEdit->setMaximumWidth(80);
        dateEdit->calendarPopup();
        lineEdit->setText(one_param_gen.at(1));
        lineEdit->setMinimumWidth(150);
        hLayout->addWidget(label);
        hLayout->addWidget(dateEdit);
        hLayout->addWidget(lineEdit);

        if (!layouts_gen_List.isEmpty()) {
            int size_of_gen_list;
            size_of_gen_list = layouts_gen_List.size() + 2;
            ui->verticalLayout_3->insertLayout(size_of_gen_list, hLayout);
        } else {
            ui->verticalLayout_3->insertLayout(2, hLayout);
        }
        layouts_gen_List.append(hLayout);
    }
}
