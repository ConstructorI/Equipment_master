#include "add_type_si.h"
#include "ui_add_type_si.h"
#include "database.h"

#include <QHBoxLayout>

add_type_si::add_type_si(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::add_type_si)
{
    ui->setupUi(this);
}

add_type_si::~add_type_si()
{
    delete ui;
}

void add_type_si::on_pushButton_add_clicked()
{
    this->collect_info();
    Database db;
    db.openDatabase("database.db");
    db.insertSI(inv, naim, mod, reestr, zav, pom, sost, PO, izg, pravo, date,
                diap, pov, fgis, temp, vlag, davl, napr, chast, tecn, clean, remont);
    for (int i = 0; i < layouts_p_List.size(); ++i) {
        QHBoxLayout *hlayout = layouts_p_List.at(i);
        QLayoutItem *item = hlayout->itemAt(1);
        if (item) {
            QWidget *widget = item->widget();
            if (widget) {
                QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget);
                if (lineEdit) {
                    inv = lineEdit->text();
                }
            }
        }
        QLayoutItem *item2 = hlayout->itemAt(3);
        if (item2) {
            QWidget *widget = item2->widget();
            if (widget) {
                QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget);
                if (lineEdit) {
                    zav = lineEdit->text();
                }
            }
        }
        db.insertSI(inv, naim, mod, reestr, zav, pom, sost, PO, izg, pravo, date,
                    diap, pov, fgis, temp, vlag, davl, napr, chast, tecn, clean, remont);
    }
    db.close_conn();
    this->close();
}

void add_type_si::on_pushButton_add_d_clicked()
{
    QHBoxLayout *hLayout = new QHBoxLayout();

    for (int i = 0; i < 5; ++i) {
        QLineEdit *lineEdit = new QLineEdit(this);
        if (i == 1 || i == 2 || i == 4){
            lineEdit->setMaximumWidth(80);
        }
        hLayout->addWidget(lineEdit);
    }
    if (!layouts_d_List.isEmpty()) {
        int size_of_d_list;
        size_of_d_list = layouts_d_List.size() + 3;
        ui->verticalLayout_4->insertLayout(size_of_d_list, hLayout);
    } else {
        ui->verticalLayout_4->insertLayout(3, hLayout);
    }
    layouts_d_List.append(hLayout);
}

void add_type_si::on_pushButton_rem_d_clicked()
{
    if (!layouts_d_List.isEmpty()) {
        QHBoxLayout *hLayout = layouts_d_List.takeLast();
        QLayoutItem *item;

        while ((item = hLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        ui->verticalLayout->removeItem(hLayout);
        delete hLayout;
    }
}

void add_type_si::on_pushButton_add_p_clicked()
{
    QHBoxLayout *hLayout = new QHBoxLayout();

    QLineEdit *lineEdit_inv = new QLineEdit(this);
    QLineEdit *lineEdit_zav = new QLineEdit(this);
    QLabel *label_inv = new QLabel(this);
    QLabel *label_zav = new QLabel(this);
    lineEdit_inv->setMaximumWidth(100);
    lineEdit_inv->setMaximumHeight(20);
    lineEdit_zav->setMinimumWidth(200);
    lineEdit_zav->setMaximumHeight(20);
    label_inv->setText("Инв. номер:");
    label_zav->setText("Зав. номер:");
    hLayout->addWidget(label_inv);
    hLayout->addWidget(lineEdit_inv);
    hLayout->addWidget(label_zav);
    hLayout->addWidget(lineEdit_zav);

    if (!layouts_p_List.isEmpty()) {
        int size_of_p_list;
        size_of_p_list = layouts_p_List.size() + 2;
        ui->verticalLayout_3->insertLayout(size_of_p_list, hLayout);
    } else {
        ui->verticalLayout_3->insertLayout(2, hLayout);
    }
    layouts_p_List.append(hLayout);
}

void add_type_si::on_pushButton_rem_p_clicked()
{
    if (!layouts_p_List.isEmpty()) {
        QHBoxLayout *hLayout = layouts_p_List.takeLast();
        QLayoutItem *item;

        while ((item = hLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        ui->verticalLayout->removeItem(hLayout);
        delete hLayout;
    }
}

void add_type_si::collect_info(){
    QStringList izg_list;
    izg_list << ui->lineEdit_izg_name->text()
             << ui->lineEdit_izg_country->text()
             << ui->lineEdit_izg_year->text();
    izg = izg_list.join("@");

    QStringList diap_1_list;
    diap_1_list << ui->lineEdit_d_param->text()
                << ui->lineEdit_d_ot->text()
                << ui->lineEdit_d_do->text()
                << ui->lineEdit_d_ed->text()
                << ui->lineEdit_d_pogr->text();
    QString diap_1;
    diap_1 = diap_1_list.join(";");
    QStringList allDiap;
    allDiap.append(diap_1);
    for (QHBoxLayout *hLayout : layouts_d_List) {
        QString diap_text = get_d_Text(hLayout);
        allDiap.append(diap_text);
    }
    diap = allDiap.join("@");

    pov = ";;;";

    naim = ui->lineEdit_naim->text();
    mod = ui->lineEdit_mod->text();
    reestr = ui->lineEdit_reestr->text();
    pom = ui->lineEdit_pom->text();
    sost = "";
    PO = ui->lineEdit_PO->text();
    pravo = ui->lineEdit_pravo->text();
    date = ui->lineEdit_date->text();
    fgis = "";
    temp = ui->lineEdit_temp->text();
    vlag = ui->lineEdit_vlag->text();
    davl = ui->lineEdit_davl->text();
    napr = ui->lineEdit_napr->text();
    chast = ui->lineEdit_chast->text();
    tecn = ui->spinBox_TO->text();
    clean = ui->spinBox_clean->text();
    remont = "";

    inv = ui->lineEdit_inv->text();
    zav = ui->lineEdit_zav->text();
}

QString add_type_si::get_d_Text(QHBoxLayout *hLayout) const
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

void add_type_si::on_pushButton_close_clicked()
{
    this->close();
}
