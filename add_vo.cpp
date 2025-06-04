//add_vo.cpp
#include "add_vo.h"
#include "ui_add_vo.h"
#include "database.h"

add_vo::add_vo(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::add_vo)
{
    ui->setupUi(this);
    this->is_edit = false;

    ui->checkBox->setChecked(false);
    ui->dateEdit_vyvod->setHidden(true);
    ui->label_18->setHidden(true);
    ui->pushButton_edit->setHidden(true);
    ui->pushButton_add->setHidden(false);
    ui->pushButton_delete->setHidden(true);
}

add_vo::~add_vo()
{
    delete ui;
}

void add_vo::set_edit(){
    if (is_edit){
        ui->pushButton_add->setHidden(true);
        ui->pushButton_edit->setHidden(false);
        ui->pushButton_delete->setHidden(false);
    } else {
        ui->pushButton_add->setHidden(false);
        ui->pushButton_edit->setHidden(true);
        ui->pushButton_delete->setHidden(true);
    }
}

void add_vo::set_data(){
    ui->lineEdit_inv->setText(inv);
    ui->lineEdit_naim->setText(naim);
    ui->lineEdit_zav->setText(zav);
    ui->lineEdit_pom->setText(pom);

    QString srt_izg = izg;
    QStringList izg_list = srt_izg.split("@");

    ui->lineEdit_izg_org->setText(izg_list.at(0));
    ui->lineEdit_izg_country->setText(izg_list.at(1));
    ui->lineEdit_izg_year->setText(izg_list.at(2));
    ui->lineEdit_pravo->setText(pravo);

    QStringList dates_list = date.split("@");
    if (dates_list.size() < 2) {
        ui->dateEdit_vvod->setDate(QDate::fromString(dates_list.at(0), "dd.MM.yyyy"));
    } else {
        ui->checkBox->setChecked(true);
        ui->dateEdit_vvod->setDate(QDate::fromString(dates_list.at(0), "dd.MM.yyyy"));
        ui->dateEdit_vyvod->setDate(QDate::fromString(dates_list.at(1), "dd.MM.yyyy"));
    }

    ui->lineEdit_pred->setText(pred);
    ui->lineEdit_PO->setText(PO);
    ui->lineEdit_temp->setText(temp);
    ui->lineEdit_vlag->setText(vlag);
    ui->lineEdit_davl->setText(davl);
    ui->lineEdit_napr->setText(napr);
    ui->lineEdit_chast->setText(chast);
    ui->spinBox_TO->setValue(tecn.toInt());
    ui->spinBox_clean->setValue(clean.toInt());
}

void add_vo::on_pushButton_add_clicked()
{
    this->collect_info();
    Database db;
    db.openDatabase("database.db");
    db.insertVO(inv, naim, zav, pom, izg, pravo, date, pred,
                    PO, temp, vlag, davl, napr, chast, tecn, clean, remont);
    db.close_conn();
    this->close();
}

void add_vo::on_pushButton_edit_clicked()
{
    this->collect_info();
    Database db;
    db.openDatabase("database.db");
    db.updateVO(VOId, inv, naim, zav, pom, izg, pravo, date, pred,
                    PO, temp, vlag, davl, napr, chast, tecn, clean, remont);
    db.close_conn();
    this->close();
}

void add_vo::collect_info(){
    inv = ui->lineEdit_inv->text();
    naim = ui->lineEdit_naim->text();
    zav = ui->lineEdit_zav->text();
    pom = ui->lineEdit_pom->text();

    QStringList izg_list;
    izg_list << ui->lineEdit_izg_org->text()
             << ui->lineEdit_izg_country->text()
             << ui->lineEdit_izg_year->text();
    izg = izg_list.join("@");

    pravo = ui->lineEdit_pravo->text();
    if (ui->checkBox->checkState() == Qt::Checked) {
        date = ui->dateEdit_vvod->text() + "@" + ui->dateEdit_vyvod->text();
    } else {
        date = ui->dateEdit_vvod->text();
    }
    pred = ui->lineEdit_pred->text();
    PO = ui->lineEdit_PO->text();
    temp = ui->lineEdit_temp->text();
    vlag = ui->lineEdit_vlag->text();
    davl = ui->lineEdit_davl->text();
    napr = ui->lineEdit_napr->text();
    chast = ui->lineEdit_chast->text();
    tecn = ui->spinBox_TO->text();
    clean = ui->spinBox_clean->text();

}

void add_vo::on_pushButton_cancel_clicked()
{
    this->close();
}

void add_vo::on_pushButton_delete_clicked()
{
    Database db;
    db.openDatabase("database.db");
    db.delete_VO(VOId);
    db.close_conn();
    this->close();
}


void add_vo::on_pushButton_add_to_clicked()
{
    QHBoxLayout *hLayout = new QHBoxLayout();
    for (int i = 0; i < 5; ++i) {
        QLineEdit *lineEdit = new QLineEdit(this);
        if (i == 0){
            lineEdit->setMaximumWidth(80);
        } else if (i == 3) {
            lineEdit->setMaximumWidth(100);
        }
        hLayout->addWidget(lineEdit);
    }
    if (!layouts_rem_List.isEmpty()) {
        int size_of_rem_list;
        size_of_rem_list = layouts_rem_List.size() + 3;
        ui->verticalLayout_3->insertLayout(size_of_rem_list, hLayout);
    } else {
        ui->verticalLayout_3->insertLayout(3, hLayout);
    }
    layouts_rem_List.append(hLayout);
}


void add_vo::on_pushButton_rem_to_clicked()
{
    if (!layouts_rem_List.isEmpty()) {
        QHBoxLayout *hLayout = layouts_rem_List.takeLast();
        QLayoutItem *item;

        while ((item = hLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        ui->verticalLayout_3->removeItem(hLayout);
        delete hLayout;
    }
}


void add_vo::on_checkBox_stateChanged(int arg1)
{
    Qt::CheckState state = static_cast<Qt::CheckState>(arg1);
    if (state == Qt::Checked){
        ui->dateEdit_vyvod->setDate(QDate::fromString("01.01.2000", "dd.MM.yyyy"));
        ui->dateEdit_vyvod->setHidden(false);
        ui->label_18->setHidden(false);
    } else {
        ui->dateEdit_vyvod->setDate(QDate::fromString("01.01.2000", "dd.MM.yyyy"));
        ui->dateEdit_vyvod->setHidden(true);
        ui->label_18->setHidden(true);
    }
}

