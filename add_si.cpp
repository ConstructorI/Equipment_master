//add_si.cpp
#include "add_si.h"
#include "ui_add_si.h"
#include "database.h"

add_si::add_si(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::add_si)
{
    ui->setupUi(this);
    this->is_edit = false;
    this->is_pov = true;
    ui->checkBox->setChecked(false);
    ui->dateEdit_vyvod->setHidden(true);
    ui->label_38->setHidden(true);

    ui->pushButton_edit->setHidden(true);
    ui->pushButton_add->setHidden(false);
    ui->pushButton_delete->setHidden(true);
}

add_si::~add_si()
{
    delete ui;
}

void add_si::on_pushButton_add_d_clicked()
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

void add_si::on_pushButton_rem_d_clicked()
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

void add_si::on_pushButton_add_p_clicked()
{
    QHBoxLayout *hLayout = new QHBoxLayout();

    for (int i = 0; i < 4; ++i) {
        QLineEdit *lineEdit = new QLineEdit(this);
        if (i == 0 || i == 3){
            lineEdit->setMaximumWidth(80);
        }
        hLayout->addWidget(lineEdit);
    }
    if (!layouts_p_List.isEmpty()) {
        int size_of_p_list;
        size_of_p_list = layouts_p_List.size() + 3;
        ui->verticalLayout_3->insertLayout(size_of_p_list, hLayout);
    } else {
        ui->verticalLayout_3->insertLayout(3, hLayout);
    }
    layouts_p_List.append(hLayout);
}

void add_si::on_pushButton_rem_p_clicked()
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

void add_si::on_pushButton_cancel_clicked()
{
    this->close();
}

void add_si::on_pushButton_add_clicked()
{
    this->collect_info();
    Database db;
    db.openDatabase("database.db");
    db.insertSI(inv, naim, mod, reestr, zav, pom, sost, PO, izg, pravo, date,
                    diap, pov, fgis, temp, vlag, davl, napr, chast, tecn, clean, remont);
    if (dop_info != ""){
        db.insert_BirksKoeft(inv, dop_info);
    }
    db.close_conn();
    this->close();
}

void add_si::on_pushButton_edit_clicked()
{
    this->collect_info();
    Database db;
    db.openDatabase("database.db");
    db.updateSI(SIId, inv, naim, mod, reestr, zav, pom, sost, PO, izg, pravo, date,
                    diap, pov, fgis, temp, vlag, davl, napr, chast, tecn, clean, remont);
    if (db.get_data_BirksKoeft(inv) == ""){
        db.insert_BirksKoeft(inv, dop_info);
    } else {
        db.update_BirksKoeft(inv, dop_info);
    }
    db.close_conn();
    this->close();
}

void add_si::collect_info(){
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

    if (is_pov){
        QStringList pov_1_list;
        pov_1_list << ui->lineEdit_p_ot->text()
                   << ui->lineEdit_p_org->text()
                   << ui->lineEdit_p_svid->text()
                   << ui->lineEdit_p_do->text();
        QString pov_1;
        pov_1 = pov_1_list.join(";");
        QStringList allPov;
        allPov.append(pov_1);
        for (QHBoxLayout *hLayout : layouts_p_List) {
            QString pov_text = get_d_Text(hLayout);
            allPov.append(pov_text);
        }
        pov = allPov.join("@");
    } else {
        pov = "-";
    }

    naim = ui->lineEdit_naim->text();
    inv = ui->lineEdit_inv->text();
    mod = ui->lineEdit_mod->text();
    reestr = ui->lineEdit_reestr->text();
    zav = ui->lineEdit_zav->text();
    pom = ui->lineEdit_pom->text();
    sost = ui->lineEdit_sost->text();
    PO = ui->lineEdit_PO->text();
    pravo = ui->lineEdit_pravo->text();


    if (ui->checkBox->checkState() == Qt::Checked) {
        date = ui->dateEdit_vvod->text() + "@" + ui->dateEdit_vyvod->text();
    } else {
        date = ui->dateEdit_vvod->text();
    }

    fgis = ui->lineEdit_fgis->text();
    temp = ui->lineEdit_temp->text();
    vlag = ui->lineEdit_vlag->text();
    davl = ui->lineEdit_davl->text();
    napr = ui->lineEdit_napr->text();
    chast = ui->lineEdit_chast->text();
    tecn = ui->spinBox_TO->text();
    clean = ui->spinBox_clean->text();

    QStringList rem_1_list;
    rem_1_list << ui->lineEdit_rem_date->text()
               << ui->lineEdit_rem_nessotv->text()
               << ui->lineEdit_rem_mer->text()
               << ui->lineEdit_rem_srok->text()
               << ui->lineEdit_rem_isp->text();
    QString rem_1;
    rem_1 = rem_1_list.join(";");
    QStringList allRem;
    allRem.append(rem_1);
    for (QHBoxLayout *hLayout : layouts_rem_List) {
        QString rem_text = get_d_Text(hLayout);
        allRem.append(rem_text);
    }
    remont = allRem.join("@");
    dop_info = ui->lineEdit_dopinfo->text();
}

void add_si::on_pushButton_delete_clicked()
{
    Database db;
    db.openDatabase("database.db");
    db.delete_SI(SIId);
    db.close_conn();
    this->close();
}

void add_si::set_edit(){
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

void add_si::set_data(){

    QString srt_izg = izg;
    QStringList izg_list = srt_izg.split("@");

    ui->lineEdit_izg_name->setText(izg_list.at(0));
    ui->lineEdit_izg_country->setText(izg_list.at(1));
    ui->lineEdit_izg_year->setText(izg_list.at(2));

    QString srt_diap = diap; //Все данные: rgragr;rgarga;agaegr;agrarg;agrag@gagag;argag;agragr;agra;argag@rgthbt;aghaa;baergg;ahbareg;agraega
    QStringList diap_list = srt_diap.split("@"); //Список данных: "rgragr;rgarga;agaegr;agrarg;agrag" "gagag;argag;agragr;agra;argag" "rgthbt;aghaa;baergg;ahbareg;agraega"
    QString one_diap;
    one_diap = diap_list.at(0);
    QStringList one_param_diap = one_diap.split(";");
    ui->lineEdit_d_param->setText(one_param_diap.at(0));
    ui->lineEdit_d_ot->setText(one_param_diap.at(1));
    ui->lineEdit_d_do->setText(one_param_diap.at(2));
    ui->lineEdit_d_ed->setText(one_param_diap.at(3));
    ui->lineEdit_d_pogr->setText(one_param_diap.at(4));

    for (int x = 0; x < diap_list.size() - 1; ++x){
        QString one_diap;
        one_diap = diap_list.at(x + 1);
        QStringList one_param_diap = one_diap.split(";");
        QHBoxLayout *hLayout = new QHBoxLayout();

        for (int i = 0; i < 5; i++) {
            QLineEdit *lineEdit = new QLineEdit(this);
            if (i == 1 || i == 2 || i == 4){
                lineEdit->setMaximumWidth(80);
            }
            lineEdit->setText(one_param_diap.at(i));
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

    if (pov != "-"){
        QString srt_pov = pov;
        QStringList pov_list = srt_pov.split("@");
        QString one_pov;
        one_pov = pov_list.at(0);
        QStringList one_param_pov = one_pov.split(";");
        ui->lineEdit_p_ot->setText(one_param_pov.at(0));
        ui->lineEdit_p_org->setText(one_param_pov.at(1));
        ui->lineEdit_p_svid->setText(one_param_pov.at(2));
        ui->lineEdit_p_do->setText(one_param_pov.at(3));
        for (int x = 0; x < pov_list.size() - 1; ++x){
            QString one_pov;
            one_pov = pov_list.at(x + 1);
            QStringList one_param_pov = one_pov.split(";");
            QHBoxLayout *hLayout = new QHBoxLayout();
            for (int i = 0; i < 4; ++i) {
                QLineEdit *lineEdit = new QLineEdit(this);
                if (i == 0 || i == 3){
                    lineEdit->setMaximumWidth(80);
                }
                lineEdit->setText(one_param_pov.at(i));
                hLayout->addWidget(lineEdit);
            }
            if (!layouts_p_List.isEmpty()) {
                int size_of_p_list;
                size_of_p_list = layouts_p_List.size() + 3;
                ui->verticalLayout_3->insertLayout(size_of_p_list, hLayout);
            } else {
                ui->verticalLayout_3->insertLayout(3, hLayout);
            }
            layouts_p_List.append(hLayout);
        }
    } else {
        this->is_pov = false;
        ui->checkBox_pov->setChecked(true);
        this->set_pov(is_pov);
    }

    ui->lineEdit_naim->setText(naim);
    ui->lineEdit_inv->setText(inv);
    ui->lineEdit_mod->setText(mod);
    ui->lineEdit_reestr->setText(reestr);
    ui->lineEdit_zav->setText(zav);
    ui->lineEdit_pom->setText(pom);
    ui->lineEdit_sost->setText(sost);
    ui->lineEdit_PO->setText(PO);
    ui->lineEdit_pravo->setText(pravo);

    QStringList dates_list = date.split("@");
    if (dates_list.size() < 2) {
        ui->dateEdit_vvod->setDate(QDate::fromString(dates_list.at(0), "dd.MM.yyyy"));
    } else {
        ui->checkBox->setChecked(true);
        ui->dateEdit_vvod->setDate(QDate::fromString(dates_list.at(0), "dd.MM.yyyy"));
        ui->dateEdit_vyvod->setDate(QDate::fromString(dates_list.at(1), "dd.MM.yyyy"));
    }

    ui->lineEdit_fgis->setText(fgis);
    ui->lineEdit_temp->setText(temp);
    ui->lineEdit_vlag->setText(vlag);
    ui->lineEdit_davl->setText(davl);
    ui->lineEdit_napr->setText(napr);
    ui->lineEdit_chast->setText(chast);
    ui->spinBox_TO->setValue(tecn.toInt());
    ui->spinBox_clean->setValue(clean.toInt());

    Database db;
    db.openDatabase("database.db");
    if (db.get_data_BirksKoeft(inv) == ""){
        ui->lineEdit_dopinfo->setText("");
    } else {
        ui->lineEdit_dopinfo->setText(db.get_data_BirksKoeft(inv));
    }
    db.close_conn();

    if (remont != ""){
        QString srt_rem = remont;
        QStringList rem_list = srt_rem.split("@");
        QString one_rem;
        one_rem = rem_list.at(0);
        QStringList one_param_rem = one_rem.split(";");
        ui->lineEdit_rem_date->setText(one_param_rem.at(0));
        ui->lineEdit_rem_nessotv->setText(one_param_rem.at(1));
        ui->lineEdit_rem_mer->setText(one_param_rem.at(2));
        ui->lineEdit_rem_srok->setText(one_param_rem.at(3));
        ui->lineEdit_rem_isp->setText(one_param_rem.at(4));

        for (int x = 0; x < rem_list.size() - 1; ++x){
            QString one_rem;
            one_rem = rem_list.at(x + 1);
            QStringList one_param_rem = one_rem.split(";");
            QHBoxLayout *hLayout = new QHBoxLayout();
            for (int i = 0; i < 5; ++i) {
                QLineEdit *lineEdit = new QLineEdit(this);
                lineEdit->setText(one_param_rem.at(i));
                hLayout->addWidget(lineEdit);
            }
            if (!layouts_rem_List.isEmpty()) {
                int size_of_rem_list;
                size_of_rem_list = layouts_rem_List.size() + 3;
                ui->verticalLayout_5->insertLayout(size_of_rem_list, hLayout);
            } else {
                ui->verticalLayout_5->insertLayout(3, hLayout);
            }
            layouts_rem_List.append(hLayout);
        }
    }
}

QString add_si::get_d_Text(QHBoxLayout *hLayout) const
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

void add_si::set_pov(bool is_pov)
{
    ui->label_26->setHidden(!is_pov);
    ui->label_27->setHidden(!is_pov);
    ui->label_28->setHidden(!is_pov);
    ui->label_29->setHidden(!is_pov);
    ui->pushButton_add_p->setHidden(!is_pov);
    ui->pushButton_rem_p->setHidden(!is_pov);
    ui->lineEdit_p_ot->setHidden(!is_pov);
    ui->lineEdit_p_org->setHidden(!is_pov);
    ui->lineEdit_p_svid->setHidden(!is_pov);
    ui->lineEdit_p_do->setHidden(!is_pov);
}

void add_si::on_checkBox_pov_stateChanged(int arg1)
{
    Qt::CheckState state = static_cast<Qt::CheckState>(arg1);
    if (state == Qt::Checked){
        this->is_pov = false;
        set_pov(is_pov);
    } else {
        this->is_pov = true;
        set_pov(is_pov);
    }
}

void add_si::on_pushButton_add_to_clicked()
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
        ui->verticalLayout_5->insertLayout(size_of_rem_list, hLayout);
    } else {
        ui->verticalLayout_5->insertLayout(3, hLayout);
    }
    layouts_rem_List.append(hLayout);
}

void add_si::on_pushButton_rem_to_clicked()
{
    if (!layouts_rem_List.isEmpty()) {
        QHBoxLayout *hLayout = layouts_rem_List.takeLast();
        QLayoutItem *item;

        while ((item = hLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        ui->verticalLayout_5->removeItem(hLayout);
        delete hLayout;
    }
}


void add_si::on_checkBox_stateChanged(int arg1)
{
    Qt::CheckState state = static_cast<Qt::CheckState>(arg1);
    if (state == Qt::Checked){
        ui->dateEdit_vyvod->setDate(QDate::fromString("01.01.2000", "dd.MM.yyyy"));
        ui->dateEdit_vyvod->setHidden(false);
        ui->label_38->setHidden(false);
    } else {
        ui->dateEdit_vyvod->setDate(QDate::fromString("01.01.2000", "dd.MM.yyyy"));
        ui->dateEdit_vyvod->setHidden(true);
        ui->label_38->setHidden(true);
    }
}

