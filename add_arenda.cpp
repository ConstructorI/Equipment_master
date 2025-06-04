#include "add_arenda.h"
#include "ui_add_arenda.h"
#include "database.h"

add_Arenda::add_Arenda(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::add_Arenda)
{
    ui->setupUi(this);

    ui->label_2->setText("Наименование, модификация:");
    ui->label_2->setMinimumWidth(170);
    ui->label_10->setHidden(false);
    ui->lineEdit_reestr->setHidden(false);

    ui->label_8->setHidden(true);
    ui->lineEdit_pred->setHidden(true);

    ui->frame->setHidden(false);
    ui->frame_2->setHidden(false);

    ui->radioButton_SI->setChecked(true);
    state = 1;
    ui->checkBox->setChecked(false);
    ui->dateEdit_vyvod->setHidden(true);
    ui->label_11->setHidden(true);

    ui->pushButton_edit->setHidden(true);
    ui->pushButton_save->setHidden(false);
    ui->pushButton_delete->setHidden(true);
}

add_Arenda::~add_Arenda()
{
    delete ui;
}


void add_Arenda::on_radioButton_SI_clicked()
{
    state = 1;
    ui->label_2->setText("Наименование, модификация:");
    ui->label_2->setMinimumWidth(170);
    ui->label_10->setHidden(false);
    ui->lineEdit_reestr->setHidden(false);

    ui->label_8->setHidden(true);
    ui->lineEdit_pred->setHidden(true);

    ui->frame->setHidden(false);
    ui->frame_2->setHidden(false);
}

void add_Arenda::on_radioButton_IO_clicked()
{
    state = 2;
    ui->label_2->setText("Наименование:");
    ui->label_2->setMinimumWidth(120);
    ui->label_10->setHidden(true);
    ui->lineEdit_reestr->setHidden(true);

    ui->label_8->setHidden(true);
    ui->lineEdit_pred->setHidden(true);

    ui->frame->setHidden(true);
    ui->frame_2->setHidden(false);
}

void add_Arenda::on_radioButton_VO_clicked()
{
    state = 3;
    ui->label_2->setText("Наименование:");
    ui->label_2->setMinimumWidth(120);
    ui->label_10->setHidden(true);
    ui->lineEdit_reestr->setHidden(true);

    ui->label_8->setHidden(false);
    ui->lineEdit_pred->setHidden(false);

    ui->frame->setHidden(true);
    ui->frame_2->setHidden(true);
}

void add_Arenda::on_checkBox_stateChanged(int arg1)
{
    Qt::CheckState state = static_cast<Qt::CheckState>(arg1);
    if (state == Qt::Checked){
        ui->dateEdit_vyvod->setDate(QDate::fromString("01.01.2000", "dd.MM.yyyy"));
        ui->dateEdit_vyvod->setHidden(false);
        ui->label_11->setHidden(false);
    } else {
        ui->dateEdit_vyvod->setDate(QDate::fromString("01.01.2000", "dd.MM.yyyy"));
        ui->dateEdit_vyvod->setHidden(true);
        ui->label_11->setHidden(true);
    }
}


void add_Arenda::on_pushButton_cancel_clicked()
{
    this->close();
}

void add_Arenda::on_pushButton_add_d_clicked()
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

void add_Arenda::on_pushButton_rem_d_clicked()
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

void add_Arenda::on_pushButton_add_p_clicked()
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
        ui->verticalLayout_5->insertLayout(size_of_p_list, hLayout);
    } else {
        ui->verticalLayout_5->insertLayout(3, hLayout);
    }
    layouts_p_List.append(hLayout);
}

void add_Arenda::on_pushButton_rem_p_clicked()
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

void add_Arenda::collect_info()
{
    if (state == 1) {
        inv = ui->lineEdit_inv->text();
        naim = ui->lineEdit_naim->text();
        type = "СИ";
        zav = ui->lineEdit_zav->text();
        reg_num = ui->lineEdit_reestr->text();
        param = "";
        sobst = ui->lineEdit_pravo->text();
        if (ui->checkBox->checkState() == Qt::Checked) {
            dates = ui->dateEdit_vvod->text() + "@" + ui->dateEdit_vyvod->text();
        } else {
            dates = ui->dateEdit_vvod->text();
        }

        QStringList izg_list;
        izg_list << ui->lineEdit_izg_org->text()
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
        pov_att = allPov.join("@");
    } else if (state == 2) {
        inv = ui->lineEdit_inv->text();
        naim = ui->lineEdit_naim->text();
        type = "ИО";
        zav = ui->lineEdit_zav->text();
        reg_num = "";
        param = "";
        sobst = ui->lineEdit_pravo->text();
        if (ui->checkBox->checkState() == Qt::Checked) {
            dates = ui->dateEdit_vvod->text() + "@" + ui->dateEdit_vyvod->text();
        } else {
            dates = ui->dateEdit_vvod->text();
        }

        QStringList izg_list;
        izg_list << ui->lineEdit_izg_org->text()
                 << ui->lineEdit_izg_country->text()
                 << ui->lineEdit_izg_year->text();
        izg = izg_list.join("@");

        diap = "";

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
        pov_att = allPov.join("@");
    } else if (state == 3) {
        inv = ui->lineEdit_inv->text();
        naim = ui->lineEdit_naim->text();
        type = "ВО";
        zav = ui->lineEdit_zav->text();
        reg_num = "";
        param = ui->lineEdit_pred->text();
        sobst = ui->lineEdit_pravo->text();
        if (ui->checkBox->checkState() == Qt::Checked) {
            dates = ui->dateEdit_vvod->text() + "@" + ui->dateEdit_vyvod->text();
        } else {
            dates = ui->dateEdit_vvod->text();
        }

        QStringList izg_list;
        izg_list << ui->lineEdit_izg_org->text()
                 << ui->lineEdit_izg_country->text()
                 << ui->lineEdit_izg_year->text();
        izg = izg_list.join("@");

        diap = "";
        pov_att = "";
    }
}

QString add_Arenda::get_d_Text(QHBoxLayout *hLayout) const
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

void add_Arenda::set_edit(){
    if (is_edit) {
        ui->pushButton_edit->setHidden(false);
        ui->pushButton_save->setHidden(true);
        ui->pushButton_delete->setHidden(false);
    } else {
        ui->pushButton_edit->setHidden(true);
        ui->pushButton_save->setHidden(false);
        ui->pushButton_delete->setHidden(true);
    }
}

void add_Arenda::set_data(){
    if (type == "СИ") {
        state = 1;
        ui->radioButton_SI->setChecked(true);
        ui->label_2->setText("Наименование, модификация:");
        ui->label_2->setMinimumWidth(170);
        ui->label_10->setHidden(false);
        ui->lineEdit_reestr->setHidden(false);

        ui->label_8->setHidden(true);
        ui->lineEdit_pred->setHidden(true);

        ui->frame->setHidden(false);
        ui->frame_2->setHidden(false);

        QString srt_diap = diap;
        QStringList diap_list = srt_diap.split("@");
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

        QString srt_pov = pov_att;
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
                ui->verticalLayout_5->insertLayout(size_of_p_list, hLayout);
            } else {
                ui->verticalLayout_5->insertLayout(3, hLayout);
            }
            layouts_p_List.append(hLayout);
        }
        ui->lineEdit_reestr->setText(reg_num);

    } else if (type == "ИО") {
        state = 2;
        ui->radioButton_IO->setChecked(true);
        ui->label_2->setText("Наименование:");
        ui->label_2->setMinimumWidth(120);
        ui->label_10->setHidden(true);
        ui->lineEdit_reestr->setHidden(true);

        ui->label_8->setHidden(true);
        ui->lineEdit_pred->setHidden(true);

        ui->frame->setHidden(true);
        ui->frame_2->setHidden(false);

        QString srt_pov = pov_att;
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
                ui->verticalLayout_5->insertLayout(size_of_p_list, hLayout);
            } else {
                ui->verticalLayout_5->insertLayout(3, hLayout);
            }
            layouts_p_List.append(hLayout);
        }
    } else if (type == "ВО") {
        state = 3;
        ui->radioButton_VO->setChecked(true);
        ui->label_2->setText("Наименование:");
        ui->label_2->setMinimumWidth(120);
        ui->label_10->setHidden(true);
        ui->lineEdit_reestr->setHidden(true);

        ui->label_8->setHidden(false);
        ui->lineEdit_pred->setHidden(false);

        ui->frame->setHidden(true);
        ui->frame_2->setHidden(true);

        ui->lineEdit_pred->setText(param);
    }

    ui->lineEdit_naim->setText(naim);
    ui->lineEdit_inv->setText(inv);
    ui->lineEdit_zav->setText(zav);
    ui->lineEdit_pravo->setText(sobst);

    QString srt_izg = izg;
    QStringList izg_list = srt_izg.split("@");

    ui->lineEdit_izg_org->setText(izg_list.at(0));
    ui->lineEdit_izg_country->setText(izg_list.at(1));
    ui->lineEdit_izg_year->setText(izg_list.at(2));

    QStringList dates_list = dates.split("@");
    if (dates_list.size() < 2) {
        ui->dateEdit_vvod->setDate(QDate::fromString(dates_list.at(0), "dd.MM.yyyy"));
    } else {
        ui->checkBox->setChecked(true);
        ui->dateEdit_vvod->setDate(QDate::fromString(dates_list.at(0), "dd.MM.yyyy"));
        ui->dateEdit_vyvod->setDate(QDate::fromString(dates_list.at(1), "dd.MM.yyyy"));
    }

    qDebug() << QDate::fromString(dates_list.at(0), "dd.MM.yyyy");
}

void add_Arenda::on_pushButton_save_clicked()
{
    collect_info();
    Database db;
    db.openDatabase("database.db");
    db.insertArenda(inv, naim, type, zav, param, reg_num,
                    izg, sobst, diap, pov_att, dates);
    db.close_conn();
    this->close();
}


void add_Arenda::on_pushButton_edit_clicked()
{
    collect_info();
    Database db;
    db.openDatabase("database.db");
    db.updateArenda(ArendaId, inv, naim, type, zav, param, reg_num,
                    izg, sobst, diap, pov_att, dates);
    db.close_conn();
    this->close();
}


void add_Arenda::on_pushButton_delete_clicked()
{
    Database db;
    db.openDatabase("database.db");
    db.delete_Arenda(ArendaId);
    db.close_conn();
    this->close();
}

