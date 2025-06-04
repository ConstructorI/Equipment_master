//add_io.cpp
#include "add_io.h"
#include "ui_add_io.h"
#include "database.h"

add_io::add_io(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::add_io)
{
    ui->setupUi(this);
    this->is_edit = false;

    ui->checkBox->setChecked(false);
    ui->dateEdit_vyvod->setHidden(true);
    ui->label_26->setHidden(true);
    ui->pushButton_edit->setHidden(true);
    ui->pushButton_add->setHidden(false);
    ui->pushButton_delete->setHidden(true);
}

add_io::~add_io()
{
    delete ui;
}

void add_io::set_edit(){
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

void add_io::set_data(){
    QString srt_izg = izg;
    QStringList izg_list = srt_izg.split("@");

    ui->lineEdit_izg_org->setText(izg_list.at(0));
    ui->lineEdit_izg_country->setText(izg_list.at(1));
    ui->lineEdit_izg_year->setText(izg_list.at(2));

    QString srt_att = att;
    QStringList att_list = srt_att.split("@");
    QString one_att;
    one_att = att_list.at(0);
    QStringList one_param_att = one_att.split(";");
    ui->lineEdit_att_ot->setText(one_param_att.at(0));
    ui->lineEdit_att_org->setText(one_param_att.at(1));
    ui->lineEdit_att_prot->setText(one_param_att.at(2));
    ui->lineEdit_att_do->setText(one_param_att.at(3));

    for (int x = 0; x < att_list.size() - 1; ++x){
        QString one_att;
        one_att = att_list.at(x + 1);
        QStringList one_param_att = one_att.split(";");
        QHBoxLayout *hLayout = new QHBoxLayout();

        for (int i = 0; i < 4; i++) {
            QLineEdit *lineEdit = new QLineEdit(this);
            if (i == 0 || i == 3){
                lineEdit->setMaximumWidth(80);
            }
            lineEdit->setText(one_param_att.at(i));
            hLayout->addWidget(lineEdit);
        }
        if (!layouts_att_List.isEmpty()) {
            int size_of_att_list;
            size_of_att_list = layouts_att_List.size() + 3;
            ui->verticalLayout_6->insertLayout(size_of_att_list, hLayout);
        } else {
            ui->verticalLayout_6->insertLayout(3, hLayout);
        }
        layouts_att_List.append(hLayout);
    }
    ui->lineEdit_naim->setText(naim);
    ui->lineEdit_krnaim->setText(krnaim);
    ui->lineEdit_inv->setText(inv);
    ui->lineEdit_zav->setText(zav);
    ui->lineEdit_pom->setText(pom);
    ui->lineEdit_pravo->setText(pravo);

    QStringList dates_list = date.split("@");
    if (dates_list.size() < 2) {
        ui->dateEdit_vvod->setDate(QDate::fromString(dates_list.at(0), "dd.MM.yyyy"));
    } else {
        ui->checkBox->setChecked(true);
        ui->dateEdit_vvod->setDate(QDate::fromString(dates_list.at(0), "dd.MM.yyyy"));
        ui->dateEdit_vyvod->setDate(QDate::fromString(dates_list.at(1), "dd.MM.yyyy"));
    }

    ui->lineEdit_pasp->setText(pasp);
    ui->plainTextEdit_obj->setPlainText(obj);
    ui->plainTextEdit_type->setPlainText(type);
    ui->plainTextEdit_tecn_char->setPlainText(khar);
    ui->lineEdit_temp->setText(temp);
    ui->lineEdit_vlag->setText(vlag);
    ui->lineEdit_davl->setText(davl);
    ui->lineEdit_napr->setText(napr);
    ui->lineEdit_chast->setText(chast);
    ui->spinBox_TO->setValue(tecn.toInt());
    ui->spinBox_clean->setValue(clean.toInt());

    if (remont != ""){
        QString srt_rem = remont;
        QStringList rem_list = srt_rem.split("@");
        QString one_rem;
        one_rem = rem_list.at(0);
        QStringList one_param_rem = one_rem.split(";");
        ui->lineEdit_rem_date_4->setText(one_param_rem.at(0));
        ui->lineEdit_rem_nessotv_4->setText(one_param_rem.at(1));
        ui->lineEdit_rem_mer_4->setText(one_param_rem.at(2));
        ui->lineEdit_rem_srok_4->setText(one_param_rem.at(3));
        ui->lineEdit_rem_isp_4->setText(one_param_rem.at(4));

        for (int x = 0; x < rem_list.size() - 1; ++x){
            QString one_rem;
            one_rem = rem_list.at(x + 1);
            QStringList one_param_rem = one_rem.split(";");
            QHBoxLayout *hLayout = new QHBoxLayout();

            for (int i = 0; i < 5; i++) {
                QLineEdit *lineEdit = new QLineEdit(this);
                lineEdit->setText(one_param_rem.at(i));
                hLayout->addWidget(lineEdit);
            }
            if (!layouts_rem_List.isEmpty()) {
                int size_of_rem_list;
                size_of_rem_list = layouts_rem_List.size() + 3;
                ui->verticalLayout_7->insertLayout(size_of_rem_list, hLayout);
            } else {
                ui->verticalLayout_7->insertLayout(3, hLayout);
            }
            layouts_rem_List.append(hLayout);
        }
    }
}

QString add_io::get_att_Text(QHBoxLayout *hLayout) const
{
    QString result;
    if (!hLayout) {
        return result;
    }
    for (int i = 0; i < hLayout->count(); ++i) {
        QLayoutItem *item = hLayout->itemAt(i);
        if (item) {
            QWidget *widget = item->widget();
            if (widget) {
                QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget);
                if (lineEdit) {
                    result += lineEdit->text();
                    if (i < hLayout->count() - 1) {
                        result += ";";
                    }
                }
            }
        }
    }
    return result;
}


void add_io::on_pushButton_cancel_clicked()
{
    this->close();
}

void add_io::on_pushButton_add_clicked()
{
    this->collect_info();
    Database db;
    db.openDatabase("database.db");
    db.insertIO(inv, naim, krnaim, zav, pom, izg, pravo, date, pasp, att,
                      obj, type, khar, temp, vlag, davl, napr, chast, tecn, clean, remont);
    db.close_conn();

    this->close();
}

void add_io::on_pushButton_edit_clicked()
{
    this->collect_info();
    Database db;
    db.openDatabase("database.db");
    db.updateIO(IOId, inv, naim, krnaim, zav, pom, izg, pravo, date, pasp, att,
                      obj, type, khar, temp, vlag, davl, napr, chast, tecn, clean, remont);
    db.close_conn();

    this->close();
}

void add_io::collect_info(){
    QStringList izg_list;
    izg_list << ui->lineEdit_izg_org->text()
             << ui->lineEdit_izg_country->text()
             << ui->lineEdit_izg_year->text();
    izg = izg_list.join("@");

    QStringList att_1_list;
    att_1_list << ui->lineEdit_att_ot->text()
               << ui->lineEdit_att_org->text()
               << ui->lineEdit_att_prot->text()
               << ui->lineEdit_att_do->text();
    QString att_1;
    att_1 = att_1_list.join(";");
    QStringList allAtt;
    allAtt.append(att_1);
    for (QHBoxLayout *hLayout : layouts_att_List) {
        QString att_text = get_att_Text(hLayout);
        allAtt.append(att_text);
    }
    att = allAtt.join("@");

    obj = ui->plainTextEdit_obj->toPlainText();
    type = ui->plainTextEdit_type->toPlainText();
    khar = ui->plainTextEdit_tecn_char->toPlainText();

    naim = ui->lineEdit_naim->text();
    krnaim = ui->lineEdit_krnaim->text();
    inv = ui->lineEdit_inv->text();
    zav = ui->lineEdit_zav->text();
    pom = ui->lineEdit_pom->text();
    pravo = ui->lineEdit_pravo->text();

    if (ui->checkBox->checkState() == Qt::Checked) {
        date = ui->dateEdit_vvod->text() + "@" + ui->dateEdit_vyvod->text();
    } else {
        date = ui->dateEdit_vvod->text();
    }

    pasp = ui->lineEdit_pasp->text();
    temp = ui->lineEdit_temp->text();
    vlag = ui->lineEdit_vlag->text();
    davl = ui->lineEdit_davl->text();
    napr = ui->lineEdit_napr->text();
    chast = ui->lineEdit_chast->text();
    tecn = ui->spinBox_TO->text();
    clean = ui->spinBox_clean->text();

    QStringList rem_1_list;
    rem_1_list << ui->lineEdit_rem_date_4->text()
               << ui->lineEdit_rem_nessotv_4->text()
               << ui->lineEdit_rem_mer_4->text()
               << ui->lineEdit_rem_srok_4->text()
               << ui->lineEdit_rem_isp_4->text();
    QString rem_1;
    rem_1 = rem_1_list.join(";");
    QStringList allRem;
    allRem.append(rem_1);
    for (QHBoxLayout *hLayout : layouts_rem_List) {
        QString rem_text = get_att_Text(hLayout);
        allRem.append(rem_text);
    }
    remont = allRem.join("@");
}

void add_io::on_pushButton_att_add_clicked()
{
    QHBoxLayout *hLayout = new QHBoxLayout();

    for (int i = 0; i < 4; ++i) {
        QLineEdit *lineEdit = new QLineEdit(this);
        if (i == 0 || i == 3){
            lineEdit->setMaximumWidth(80);
        }
        hLayout->addWidget(lineEdit);
    }
    if (!layouts_att_List.isEmpty()) {
        int size_of_att_list;
        size_of_att_list = layouts_att_List.size() + 3;
        ui->verticalLayout_6->insertLayout(size_of_att_list, hLayout);
    } else {
        ui->verticalLayout_6->insertLayout(3, hLayout);
    }
    layouts_att_List.append(hLayout);
}


void add_io::on_pushButton_att_rem_clicked()
{
    if (!layouts_att_List.isEmpty()) {
        QHBoxLayout *hLayout = layouts_att_List.takeLast();
        QLayoutItem *item;

        while ((item = hLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        ui->verticalLayout_6->removeItem(hLayout);
        delete hLayout;
    }
}

void add_io::on_pushButton_delete_clicked()
{
    Database db;
    db.openDatabase("database.db");
    db.delete_IO(IOId);
    db.close_conn();
    this->close();
}

void add_io::on_pushButton_add_to_clicked()
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
        ui->verticalLayout_7->insertLayout(size_of_rem_list, hLayout);
    } else {
        ui->verticalLayout_7->insertLayout(3, hLayout);
    }
    layouts_rem_List.append(hLayout);
}


void add_io::on_pushButton_rem_to_clicked()
{
    if (!layouts_rem_List.isEmpty()) {
        QHBoxLayout *hLayout = layouts_rem_List.takeLast();
        QLayoutItem *item;

        while ((item = hLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        ui->verticalLayout_7->removeItem(hLayout);
        delete hLayout;
    }
}


void add_io::on_checkBox_stateChanged(int arg1)
{
    Qt::CheckState state = static_cast<Qt::CheckState>(arg1);
    if (state == Qt::Checked){
        ui->dateEdit_vyvod->setDate(QDate::fromString("01.01.2000", "dd.MM.yyyy"));
        ui->dateEdit_vyvod->setHidden(false);
        ui->label_26->setHidden(false);
    } else {
        ui->dateEdit_vyvod->setDate(QDate::fromString("01.01.2000", "dd.MM.yyyy"));
        ui->dateEdit_vyvod->setHidden(true);
        ui->label_26->setHidden(true);
    }
}

