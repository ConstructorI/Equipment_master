//mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"
#include "file_menu.h"
#include "dates.h"
#include "add_si.h"
#include "add_type_si.h"
#include "add_io.h"
#include "add_vo.h"
#include "add_prtkl.h"
#include "add_arenda.h"
#include "staff.h"
#include "forms.h"
#include "key_dialog.h"
#include <windows.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSpacerItem>
#include <QTextEdit>
#include <QStandardItemModel>

#include <QClipboard>
#include <QDesktopServices>
#include <QProcess>
#include <QScreen>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_SI->setChecked(true);

    Database db2;
    db2.openDatabase("database.db");
    SIList = db2.get_data_SI();
    IOList = db2.get_data_IO();
    VOList = db2.get_data_VO();
    QStringList data_info = db2.get_data_info();

    sqlmodel = new QSqlQueryModel();
    QSqlQuery query(db2.db_r());
    query.prepare("SELECT * FROM SI ORDER BY CAST(inv AS INTEGER)");
    if (!query.exec()) {
        qDebug() << "Ошибка модели СИ: " << query.lastError().text();
        delete sqlmodel;
    }
    sqlmodel->setQuery(query);
    while (sqlmodel->canFetchMore()) {
        sqlmodel->fetchMore();
    }
    qDebug() << "Модель SI установлена";
    db2.close_conn();

    model = new QStandardItemModel(this);
    QStringList headers;
    for(int i = 0; i < sqlmodel->columnCount(); ++i) {
        headers << sqlmodel->headerData(i, Qt::Horizontal).toString();
    }
    model->setHorizontalHeaderLabels(headers);

    for(int row = 0; row < sqlmodel->rowCount(); ++row) {
        bool is_exist = true;
        QList<QStandardItem*> items;
        for(int col = 0; (col < sqlmodel->columnCount() - 1); ++col) {
            if (col == 2){
                continue;
            } else if (col == 3){
                QStandardItem* item = new QStandardItem(
                    sqlmodel->data(sqlmodel->index(row, 2)).toString() + " " +
                    sqlmodel->data(sqlmodel->index(row, col)).toString()
                    );
                items.append(item);
            } else if (col == 11){
                QStandardItem* item = new QStandardItem(
                    sqlmodel->data(sqlmodel->index(row, col)).toString()
                    );
                QString date = item->text();
                QStringList date_List = date.split("@");
                if (date_List.size() > 1) {
                    is_exist = false;
                }
            } else {
                QStandardItem* item = new QStandardItem(
                    sqlmodel->data(sqlmodel->index(row, col)).toString()
                    );
                items.append(item);
            }
        }
        if (!is_exist){
            for (QStandardItem* item : items) {
                item->setBackground(QBrush(QColor("#f2d7d5")));
                item->setForeground(QBrush(QColor("#943126")));
            }
        }
        model->appendRow(items);
    }

    ui->tableView->setModel(model);
    table = 1;
    set_table_columns();
    ui->tableView->show();
    ui->pushButton_prtkl->setHidden(true);

    create_directory_for_eqipment(SIList, 0);
    create_directory_for_eqipment(IOList, 1);
    create_directory_for_eqipment(VOList, 2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if (table == 1){
        add_si ui_si;
        ui_si.setModal(true);
        ui_si.exec();

        table = 1;
        set_table("SI");

    } else if (table == 2){
        add_io ui_io;
        ui_io.setModal(true);
        ui_io.exec();

        table = 2;
        set_table("IO");

    } else if (table == 3){
        add_vo ui_vo;
        ui_vo.setModal(true);
        ui_vo.exec();

        table = 3;
        set_table("VO");

    } else if (table == 4){
        add_Arenda ui_Arenda;
        ui_Arenda.setModal(true);
        ui_Arenda.exec();

        table = 4;
        set_table("Arenda");

    } else if (table == 5){
        add_Prtkl ui_Prtkl;
        ui_Prtkl.setModal(true);
        ui_Prtkl.exec();

        table = 5;
        set_table("Prtkl");

    }
}

void MainWindow::on_pushButton_SI_clicked()
{
    clean_layout();
    table = 1;
    set_table("SI");
    ui->tableView->scrollToTop();
}

void MainWindow::on_pushButton_IO_clicked()
{
    clean_layout();
    table = 2;
    set_table("IO");
    ui->tableView->scrollToTop();
}

void MainWindow::on_pushButton_VO_clicked()
{
    clean_layout();
    table = 3;
    set_table("VO");
    ui->tableView->scrollToTop();
}

void MainWindow::on_pushButton_arenda_clicked()
{
    clean_layout();
    table = 4;
    set_table("Arenda");
    ui->tableView->scrollToTop();
}

void MainWindow::on_pushButton_prtkl_clicked()
{
    clean_layout();
    table = 5;
    set_table("Prtkl");
    ui->tableView->scrollToTop();
}

void MainWindow::on_pushButton_pov_clicked()
{
    Dates dates;
    dates.setModal(true);
    dates.exec();
}

void MainWindow::on_pushButton_file_clicked()
{
    file_menu filemenu;
    filemenu.setModal(true);
    filemenu.exec();
}


void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    if (table == 1){

        int row = index.row();
        QString inv = sqlmodel->data(sqlmodel->index(row, 1)).toString();
        QString naim = sqlmodel->data(sqlmodel->index(row, 2)).toString();
        QString mod = sqlmodel->data(sqlmodel->index(row, 3)).toString();
        QString reestr = sqlmodel->data(sqlmodel->index(row, 4)).toString();
        QString zav = sqlmodel->data(sqlmodel->index(row, 5)).toString();
        QString pom = sqlmodel->data(sqlmodel->index(row, 6)).toString();
        QString sost = sqlmodel->data(sqlmodel->index(row, 7)).toString();
        QString PO = sqlmodel->data(sqlmodel->index(row, 8)).toString();
        QString izg = sqlmodel->data(sqlmodel->index(row, 9)).toString();
        QString pravo = sqlmodel->data(sqlmodel->index(row, 10)).toString();
        QString date = sqlmodel->data(sqlmodel->index(row, 11)).toString();
        QString diap = sqlmodel->data(sqlmodel->index(row, 12)).toString();
        QString pov = sqlmodel->data(sqlmodel->index(row, 13)).toString();
        QString fgis = sqlmodel->data(sqlmodel->index(row, 14)).toString();
        QString temp = sqlmodel->data(sqlmodel->index(row, 15)).toString();
        QString vlag = sqlmodel->data(sqlmodel->index(row, 16)).toString();
        QString davl = sqlmodel->data(sqlmodel->index(row, 17)).toString();
        QString napr = sqlmodel->data(sqlmodel->index(row, 18)).toString();
        QString chast = sqlmodel->data(sqlmodel->index(row, 19)).toString();
        QString tecn = sqlmodel->data(sqlmodel->index(row, 20)).toString();
        QString clean = sqlmodel->data(sqlmodel->index(row, 21)).toString();
        QStringList data_SI;
        data_SI << inv << naim << mod << reestr << zav << pom << sost << PO
                << izg << pravo << date << diap << pov << fgis << temp << vlag
                << davl << napr << chast << tecn << clean;
        clean_layout();
        fill_layout_SI(data_SI, layouts_SI_List);
        ui->frame->setHidden(false);

    } else if (table == 2){

        int row = index.row();
        QString inv = sqlmodel->data(sqlmodel->index(row, 1)).toString();
        QString naim = sqlmodel->data(sqlmodel->index(row, 2)).toString();
        QString krnaim = sqlmodel->data(sqlmodel->index(row, 3)).toString();
        QString zav = sqlmodel->data(sqlmodel->index(row, 4)).toString();
        QString pom = sqlmodel->data(sqlmodel->index(row, 5)).toString();
        QString izg = sqlmodel->data(sqlmodel->index(row, 6)).toString();
        QString pravo = sqlmodel->data(sqlmodel->index(row, 7)).toString();
        QString date = sqlmodel->data(sqlmodel->index(row, 8)).toString();
        QString pasp = sqlmodel->data(sqlmodel->index(row, 9)).toString();
        QString att = sqlmodel->data(sqlmodel->index(row, 10)).toString();
        QString obj = sqlmodel->data(sqlmodel->index(row, 11)).toString();
        QString type = sqlmodel->data(sqlmodel->index(row, 12)).toString();
        QString khar = sqlmodel->data(sqlmodel->index(row, 13)).toString();
        QString temp = sqlmodel->data(sqlmodel->index(row, 14)).toString();
        QString vlag = sqlmodel->data(sqlmodel->index(row, 15)).toString();
        QString davl = sqlmodel->data(sqlmodel->index(row, 16)).toString();
        QString napr = sqlmodel->data(sqlmodel->index(row, 17)).toString();
        QString chast = sqlmodel->data(sqlmodel->index(row, 18)).toString();
        QString tecn = sqlmodel->data(sqlmodel->index(row, 19)).toString();
        QString clean = sqlmodel->data(sqlmodel->index(row, 20)).toString();
        QStringList data_IO;
        data_IO << inv << naim << krnaim << zav << pom << izg << pravo
                << date << pasp << att << obj << type << khar << temp
                << vlag << davl << napr << chast << tecn << clean;
        clean_layout();
        fill_layout_IO(data_IO, layouts_IO_List);
        ui->frame->setHidden(false);

    } else if (table == 3){

        int row = index.row();
        QString inv = sqlmodel->data(sqlmodel->index(row, 1)).toString();
        QString naim = sqlmodel->data(sqlmodel->index(row, 2)).toString();
        QString zav = sqlmodel->data(sqlmodel->index(row, 3)).toString();
        QString pom = sqlmodel->data(sqlmodel->index(row, 4)).toString();
        QString izg = sqlmodel->data(sqlmodel->index(row, 5)).toString();
        QString pravo = sqlmodel->data(sqlmodel->index(row, 6)).toString();
        QString date = sqlmodel->data(sqlmodel->index(row, 7)).toString();
        QString pred = sqlmodel->data(sqlmodel->index(row, 8)).toString();
        QString PO = sqlmodel->data(sqlmodel->index(row, 9)).toString();
        QString temp = sqlmodel->data(sqlmodel->index(row, 10)).toString();
        QString vlag = sqlmodel->data(sqlmodel->index(row, 11)).toString();
        QString davl = sqlmodel->data(sqlmodel->index(row, 12)).toString();
        QString napr = sqlmodel->data(sqlmodel->index(row, 13)).toString();
        QString chast = sqlmodel->data(sqlmodel->index(row, 14)).toString();
        QString tecn = sqlmodel->data(sqlmodel->index(row, 15)).toString();
        QString clean = sqlmodel->data(sqlmodel->index(row, 16)).toString();
        QStringList data_VO;
        data_VO << inv << naim << zav << pom << izg << pravo
                << date << pred << PO << temp << vlag << davl
                << napr << chast << tecn << clean;
        clean_layout();
        fill_layout_VO(data_VO, layouts_VO_List);
        ui->frame->setHidden(false);

    } else if (table == 4){

        int row = index.row();
        QString inv = sqlmodel->data(sqlmodel->index(row, 1)).toString();
        QString naim = sqlmodel->data(sqlmodel->index(row, 2)).toString();
        QString type = sqlmodel->data(sqlmodel->index(row, 3)).toString();
        QString zav = sqlmodel->data(sqlmodel->index(row, 4)).toString();
        QString param = sqlmodel->data(sqlmodel->index(row, 5)).toString();
        QString reg_num = sqlmodel->data(sqlmodel->index(row, 6)).toString();
        QString izg = sqlmodel->data(sqlmodel->index(row, 7)).toString();
        QString sobst = sqlmodel->data(sqlmodel->index(row, 8)).toString();
        QString diap = sqlmodel->data(sqlmodel->index(row, 9)).toString();
        QString pov_att = sqlmodel->data(sqlmodel->index(row, 10)).toString();
        QString dates = sqlmodel->data(sqlmodel->index(row, 11)).toString();

        QStringList data_Arenda;
        data_Arenda << inv << naim << type << zav << param << reg_num
                << izg << sobst << diap << pov_att << dates;
        clean_layout();
        fill_layout_Arenda(data_Arenda, layouts_Arenda_List);
        ui->frame->setHidden(false);
    }
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (table == 1){
        add_si ui_si;

        int row = index.row();
        QVariant SIId = sqlmodel->data(sqlmodel->index(row, 0));
        QVariant inv = sqlmodel->data(sqlmodel->index(row, 1));
        QVariant naim = sqlmodel->data(sqlmodel->index(row, 2));
        QVariant mod = sqlmodel->data(sqlmodel->index(row, 3));
        QVariant reestr = sqlmodel->data(sqlmodel->index(row, 4));
        QVariant zav = sqlmodel->data(sqlmodel->index(row, 5));
        QVariant pom = sqlmodel->data(sqlmodel->index(row, 6));
        QVariant sost = sqlmodel->data(sqlmodel->index(row, 7));
        QVariant PO = sqlmodel->data(sqlmodel->index(row, 8));
        QVariant izg = sqlmodel->data(sqlmodel->index(row, 9));
        QVariant pravo = sqlmodel->data(sqlmodel->index(row, 10));
        QVariant date = sqlmodel->data(sqlmodel->index(row, 11));
        QVariant diap = sqlmodel->data(sqlmodel->index(row, 12));
        QVariant pov = sqlmodel->data(sqlmodel->index(row, 13));
        QVariant fgis = sqlmodel->data(sqlmodel->index(row, 14));
        QVariant temp = sqlmodel->data(sqlmodel->index(row, 15));
        QVariant vlag = sqlmodel->data(sqlmodel->index(row, 16));
        QVariant davl = sqlmodel->data(sqlmodel->index(row, 17));
        QVariant napr = sqlmodel->data(sqlmodel->index(row, 18));
        QVariant chast = sqlmodel->data(sqlmodel->index(row, 19));
        QVariant tecn = sqlmodel->data(sqlmodel->index(row, 20));
        QVariant clean = sqlmodel->data(sqlmodel->index(row, 21));
        QVariant remont = sqlmodel->data(sqlmodel->index(row, 22));

        ui_si.SIId = SIId.toInt();
        ui_si.inv = inv.toString();
        ui_si.naim = naim.toString();
        ui_si.mod = mod.toString();
        ui_si.reestr = reestr.toString();
        ui_si.zav = zav.toString();
        ui_si.pom = pom.toString();
        ui_si.sost = sost.toString();
        ui_si.PO = PO.toString();
        ui_si.izg = izg.toString();
        ui_si.pravo = pravo.toString();
        ui_si.date = date.toString();
        ui_si.diap = diap.toString();
        ui_si.pov = pov.toString();
        ui_si.fgis = fgis.toString();
        ui_si.temp = temp.toString();
        ui_si.vlag = vlag.toString();
        ui_si.davl = davl.toString();
        ui_si.napr = napr.toString();
        ui_si.chast = chast.toString();
        ui_si.tecn = tecn.toString();
        ui_si.clean = clean.toString();
        ui_si.remont = remont.toString();

        ui_si.is_edit = true;
        ui_si.set_edit();
        ui_si.set_data();
        ui_si.setModal(true);
        ui_si.exec();

        table = 1;
        set_table("SI");

    } else if (table == 2){
        add_io ui_io;

        int row = index.row();
        QVariant IOId = sqlmodel->data(sqlmodel->index(row, 0));
        QVariant inv = sqlmodel->data(sqlmodel->index(row, 1));
        QVariant naim = sqlmodel->data(sqlmodel->index(row, 2));
        QVariant krnaim = sqlmodel->data(sqlmodel->index(row, 3));
        QVariant zav = sqlmodel->data(sqlmodel->index(row, 4));
        QVariant pom = sqlmodel->data(sqlmodel->index(row, 5));
        QVariant izg = sqlmodel->data(sqlmodel->index(row, 6));
        QVariant pravo = sqlmodel->data(sqlmodel->index(row, 7));
        QVariant date = sqlmodel->data(sqlmodel->index(row, 8));
        QVariant pasp = sqlmodel->data(sqlmodel->index(row, 9));
        QVariant att = sqlmodel->data(sqlmodel->index(row, 10));
        QVariant obj = sqlmodel->data(sqlmodel->index(row, 11));
        QVariant type = sqlmodel->data(sqlmodel->index(row, 12));
        QVariant khar = sqlmodel->data(sqlmodel->index(row, 13));
        QVariant temp = sqlmodel->data(sqlmodel->index(row, 14));
        QVariant vlag = sqlmodel->data(sqlmodel->index(row, 15));
        QVariant davl = sqlmodel->data(sqlmodel->index(row, 16));
        QVariant napr = sqlmodel->data(sqlmodel->index(row, 17));
        QVariant chast = sqlmodel->data(sqlmodel->index(row, 18));
        QVariant tecn = sqlmodel->data(sqlmodel->index(row, 19));
        QVariant clean = sqlmodel->data(sqlmodel->index(row, 20));
        QVariant remont = sqlmodel->data(sqlmodel->index(row, 21));

        ui_io.IOId = IOId.toInt();
        ui_io.inv = inv.toString();
        ui_io.naim = naim.toString();
        ui_io.krnaim = krnaim.toString();
        ui_io.zav = zav.toString();
        ui_io.pom = pom.toString();
        ui_io.izg = izg.toString();
        ui_io.pravo = pravo.toString();
        ui_io.date = date.toString();
        ui_io.pasp = pasp.toString();
        ui_io.att = att.toString();
        ui_io.obj = obj.toString();
        ui_io.type = type.toString();
        ui_io.khar = khar.toString();
        ui_io.temp = temp.toString();
        ui_io.vlag = vlag.toString();
        ui_io.davl = davl.toString();
        ui_io.napr = napr.toString();
        ui_io.chast = chast.toString();
        ui_io.tecn = tecn.toString();
        ui_io.clean = clean.toString();
        ui_io.remont = remont.toString();

        ui_io.is_edit = true;
        ui_io.set_edit();
        ui_io.set_data();
        ui_io.setModal(true);
        ui_io.exec();

        table = 2;
        set_table("IO");

    } else if (table == 3){
        add_vo ui_vo;

        int row = index.row();
        QVariant VOId = sqlmodel->data(sqlmodel->index(row, 0));
        QVariant inv = sqlmodel->data(sqlmodel->index(row, 1));
        QVariant naim = sqlmodel->data(sqlmodel->index(row, 2));
        QVariant zav = sqlmodel->data(sqlmodel->index(row, 3));
        QVariant pom = sqlmodel->data(sqlmodel->index(row, 4));
        QVariant izg = sqlmodel->data(sqlmodel->index(row, 5));
        QVariant pravo = sqlmodel->data(sqlmodel->index(row, 6));
        QVariant date = sqlmodel->data(sqlmodel->index(row, 7));
        QVariant pred = sqlmodel->data(sqlmodel->index(row, 8));
        QVariant PO = sqlmodel->data(sqlmodel->index(row, 9));
        QVariant temp = sqlmodel->data(sqlmodel->index(row, 10));
        QVariant vlag = sqlmodel->data(sqlmodel->index(row, 11));
        QVariant davl = sqlmodel->data(sqlmodel->index(row, 12));
        QVariant napr = sqlmodel->data(sqlmodel->index(row, 13));
        QVariant chast = sqlmodel->data(sqlmodel->index(row, 14));
        QVariant tecn = sqlmodel->data(sqlmodel->index(row, 15));
        QVariant clean = sqlmodel->data(sqlmodel->index(row, 16));
        QVariant remont = sqlmodel->data(sqlmodel->index(row, 17));

        ui_vo.VOId = VOId.toInt();
        ui_vo.inv = inv.toString();
        ui_vo.naim = naim.toString();
        ui_vo.zav = zav.toString();
        ui_vo.pom = pom.toString();
        ui_vo.izg = izg.toString();
        ui_vo.pravo = pravo.toString();
        ui_vo.date = date.toString();
        ui_vo.pred = pred.toString();
        ui_vo.PO = PO.toString();
        ui_vo.temp = temp.toString();
        ui_vo.vlag = vlag.toString();
        ui_vo.davl = davl.toString();
        ui_vo.napr = napr.toString();
        ui_vo.chast = chast.toString();
        ui_vo.tecn = tecn.toString();
        ui_vo.clean = clean.toString();
        ui_vo.remont = remont.toString();

        ui_vo.is_edit = true;
        ui_vo.set_data();
        ui_vo.set_edit();
        ui_vo.setModal(true);
        ui_vo.exec();

        table = 3;
        set_table("VO");

    } else if (table == 4){
        add_Arenda ui_arenda;

        int row = index.row();
        QVariant ArendaId = sqlmodel->data(sqlmodel->index(row, 0));

        QVariant inv = sqlmodel->data(sqlmodel->index(row, 1));
        QVariant naim = sqlmodel->data(sqlmodel->index(row, 2));
        QVariant type = sqlmodel->data(sqlmodel->index(row, 3));
        QVariant zav = sqlmodel->data(sqlmodel->index(row, 4));
        QVariant param = sqlmodel->data(sqlmodel->index(row, 5));
        QVariant reg_num = sqlmodel->data(sqlmodel->index(row, 6));
        QVariant izg = sqlmodel->data(sqlmodel->index(row, 7));
        QVariant sobst = sqlmodel->data(sqlmodel->index(row, 8));
        QVariant diap = sqlmodel->data(sqlmodel->index(row, 9));
        QVariant pov_att = sqlmodel->data(sqlmodel->index(row, 10));
        QVariant dates = sqlmodel->data(sqlmodel->index(row, 11));

        ui_arenda.ArendaId = ArendaId.toInt();
        ui_arenda.inv = inv.toString();
        ui_arenda.naim = naim.toString();
        ui_arenda.type = type.toString();
        ui_arenda.zav = zav.toString();
        ui_arenda.param = param.toString();
        ui_arenda.reg_num = reg_num.toString();
        ui_arenda.izg = izg.toString();
        ui_arenda.sobst = sobst.toString();
        ui_arenda.diap = diap.toString();
        ui_arenda.pov_att = pov_att.toString();
        ui_arenda.dates = dates.toString();

        ui_arenda.is_edit = true;
        ui_arenda.set_data(); // do that
        ui_arenda.set_edit();
        ui_arenda.setModal(true);
        ui_arenda.exec();

        table = 4;
        set_table("Arenda");
    }
}



void MainWindow::fill_layout_SI(const QStringList data, QList<QHBoxLayout*> &layouts_List){

    if (layouts_List.isEmpty()){
        int numlayouts_List;
        int num_center_layouts_List;
        int num_bot_layouts_List;

        // верхняя часть
        QHBoxLayout *hLayout1 = new QHBoxLayout();
        ClickableLabel* clicklabel_inv = new ClickableLabel();
        QLabel* spacer1_3 = new QLabel();
        ClickableLabel* clicklabel_name = new ClickableLabel();

        QSpacerItem *spacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer1_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_inv = data.at(0);
        QString data_name = data.at(1);

        clicklabel_inv->setText(data_inv);
        clicklabel_name->setText(data_name);
        spacer1_3->setText("  ");

        clicklabel_inv->setStyleSheet("font-size: 16px; font-weight: bold;");
        clicklabel_name->setStyleSheet("font-size: 16px; font-weight: bold;");
        spacer1_3->setStyleSheet("font-size: 16px; font-weight: bold;");

        hLayout1->addItem(spacer1);
        hLayout1->addWidget(clicklabel_inv);
        hLayout1->addWidget(spacer1_3);
        hLayout1->addWidget(clicklabel_name);
        hLayout1->addItem(spacer1_2);
        ui->verticalLayout_2->insertLayout(0, hLayout1);
        layouts_SI_List.append(hLayout1);


        QHBoxLayout *hLayout1_1 = new QHBoxLayout();
        ClickableLabel* clicklabel_mod = new ClickableLabel();
        QSpacerItem *spacer1_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer1_2_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_mod = data.at(2);
        clicklabel_mod->setText(data_mod);
        clicklabel_mod->setStyleSheet("font-size: 16px; font-weight: bold;");
        hLayout1_1->addItem(spacer1_1);
        hLayout1_1->addWidget(clicklabel_mod);
        hLayout1_1->addItem(spacer1_2_1);
        numlayouts_List = layouts_SI_List.size();
        ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout1_1);
        layouts_SI_List.append(hLayout1_1);


        QString data_sost = data.at(6);
        if (data_sost != ""){
            QHBoxLayout *hLayout2 = new QHBoxLayout();
            QSpacerItem *spacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            QSpacerItem *spacer2_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            QLabel *label_sost = new QLabel();
            ClickableLabel *clicklabel_sost = new ClickableLabel();
            label_sost->setText("В составе: ");
            label_sost->setStyleSheet("font-size: 10px;");
            clicklabel_sost->setText(data_sost);
            clicklabel_sost->setStyleSheet("font-size: 10px; font-weight: bold;");
            hLayout2->addItem(spacer2);
            hLayout2->addWidget(label_sost);
            hLayout2->addWidget(clicklabel_sost);
            hLayout2->addItem(spacer2_2);
            numlayouts_List = layouts_List.size();
            ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout2);
            layouts_List.append(hLayout2);
        }
        QHBoxLayout *hLayout_verticalLayout_2_sp = new QHBoxLayout();
        QSpacerItem *verticalLayout_2_sp = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        hLayout_verticalLayout_2_sp->addItem(verticalLayout_2_sp);
        numlayouts_List = layouts_List.size();
        ui->verticalLayout_2->insertItem(numlayouts_List, hLayout_verticalLayout_2_sp);
        layouts_List.append(hLayout_verticalLayout_2_sp);


        // средняя часть
        QHBoxLayout *hLayout3 = new QHBoxLayout();
        QLabel *label_zav = new QLabel();
        QSpacerItem *spacer3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_zav->setText("Зав. № ");
        label_zav->setStyleSheet("font-size: 10px;");
        hLayout3->addWidget(label_zav);
        hLayout3->addItem(spacer3);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout3);
        layouts_SI_List_center.append(hLayout3);

        QHBoxLayout *hLayout4 = new QHBoxLayout();
        ClickableLabel *clicklabel_zav = new ClickableLabel();
        QString data_zav = data.at(4);
        QSpacerItem *spacer4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        clicklabel_zav->setText(data_zav);
        clicklabel_zav->setStyleSheet("font-size: 16px; font-weight: bold;");
        hLayout4->addWidget(clicklabel_zav);
        hLayout4->addItem(spacer4);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout4);
        layouts_SI_List_center.append(hLayout4);

        QHBoxLayout *hLayout_sp_4 = new QHBoxLayout();
        QLabel *label_sp_4 = new QLabel();
        label_sp_4->setMaximumHeight(5);
        hLayout_sp_4->addWidget(label_sp_4);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout_sp_4);
        layouts_SI_List_center.append(hLayout_sp_4);

        QHBoxLayout *hLayout5 = new QHBoxLayout();
        QLabel *label_reestr = new QLabel();
        QSpacerItem *spacer5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_reestr->setText("Гос. реестр № ");
        label_reestr->setStyleSheet("font-size: 10px;");
        hLayout5->addWidget(label_reestr);
        hLayout5->addItem(spacer5);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout5);
        layouts_SI_List_center.append(hLayout5);

        QHBoxLayout *hLayout6 = new QHBoxLayout();
        ClickableLabel *clicklabel_reestr = new ClickableLabel();
        QString data_reestr = data.at(3);
        QSpacerItem *spacer6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        clicklabel_reestr->setText(data_reestr);
        clicklabel_reestr->setStyleSheet("font-size: 16px; font-weight: bold;");
        hLayout6->addWidget(clicklabel_reestr);
        hLayout6->addItem(spacer6);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout6);
        layouts_SI_List_center.append(hLayout6);

        QHBoxLayout *hLayout_sp_5 = new QHBoxLayout();
        QLabel *label_sp_5 = new QLabel();
        label_sp_5->setMaximumHeight(5);
        hLayout_sp_5->addWidget(label_sp_5);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout_sp_5);
        layouts_SI_List_center.append(hLayout_sp_5);

        QHBoxLayout *hLayout7 = new QHBoxLayout();
        QLabel *label_po = new QLabel();
        QSpacerItem *spacer7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_po->setText("ПО:");
        label_po->setStyleSheet("font-size: 10px;");
        hLayout7->addWidget(label_po);
        hLayout7->addItem(spacer7);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout7);
        layouts_SI_List_center.append(hLayout7);

        QHBoxLayout *hLayout8 = new QHBoxLayout();
        ClickableLabel *clicklabel_po = new ClickableLabel();
        QString data_po = data.at(7);
        if (data.at(7) == ""){
            data_po = "отсутствует";
        }
        QSpacerItem *spacer8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        clicklabel_po->setText(data_po);
        clicklabel_po->setStyleSheet("font-size: 12px; font-weight: bold;");
        hLayout8->addWidget(clicklabel_po);
        hLayout8->addItem(spacer8);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout8);
        layouts_SI_List_center.append(hLayout8);

        QHBoxLayout *hLayout_sp_6 = new QHBoxLayout();
        QLabel *label_sp_6 = new QLabel();
        label_sp_6->setMaximumHeight(5);
        hLayout_sp_6->addWidget(label_sp_6);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout_sp_6);
        layouts_SI_List_center.append(hLayout_sp_6);

        QHBoxLayout *hLayout9 = new QHBoxLayout();
        QLabel *label_izg = new QLabel();
        QSpacerItem *spacer9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_izg->setText("Изготовитель:");
        label_izg->setStyleSheet("font-size: 10px;");
        hLayout9->addWidget(label_izg);
        hLayout9->addItem(spacer9);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout9);
        layouts_SI_List_center.append(hLayout9);

        QHBoxLayout *hLayout10 = new QHBoxLayout();
        ClickableLabel *clicklabel_country = new ClickableLabel();
        ClickableLabel *clicklabel_izg = new ClickableLabel();
        ClickableLabel *clicklabel_year = new ClickableLabel();
        QString data_izg = data.at(8);
        QStringList izg_list = data_izg.split("@");
        QSpacerItem *spacer10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        clicklabel_country->setText(izg_list.at(0));
        QStringList splitLines;
        for (int i = 0; i < izg_list.at(1).length(); i += 40) {
            splitLines << izg_list.at(1).mid(i, 40);
        }
        clicklabel_izg->setText(splitLines.join("\n"));
        clicklabel_year->setText(izg_list.at(2));
        clicklabel_country->setStyleSheet("font-size: 12px; font-weight: bold;");
        clicklabel_izg->setStyleSheet("font-size: 12px; font-weight: bold;");
        clicklabel_year->setStyleSheet("font-size: 12px; font-weight: bold;");
        hLayout10->addWidget(clicklabel_country);
        hLayout10->addItem(spacer10);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout10);
        layouts_SI_List_center.append(hLayout10);
        QHBoxLayout *hLayout10_2 = new QHBoxLayout();
        QSpacerItem *spacer10_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        hLayout10_2->addWidget(clicklabel_izg);
        hLayout10_2->addItem(spacer10_2);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout10_2);
        layouts_SI_List_center.append(hLayout10_2);
        QHBoxLayout *hLayout10_3 = new QHBoxLayout();
        QSpacerItem *spacer10_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        hLayout10_3->addWidget(clicklabel_year);
        hLayout10_3->addItem(spacer10_3);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout10_3);
        layouts_SI_List_center.append(hLayout10_3);

        QHBoxLayout *hLayout_sp_7 = new QHBoxLayout();
        QLabel *label_sp_7 = new QLabel();
        label_sp_7->setMaximumHeight(5);
        hLayout_sp_7->addWidget(label_sp_7);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout_sp_7);
        layouts_SI_List_center.append(hLayout_sp_7);

        QHBoxLayout *hLayout11 = new QHBoxLayout();
        QLabel *label_sobst = new QLabel();
        QSpacerItem *spacer11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_sobst->setText("Право собственности:");
        label_sobst->setStyleSheet("font-size: 10px;");
        hLayout11->addWidget(label_sobst);
        hLayout11->addItem(spacer11);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout11);
        layouts_SI_List_center.append(hLayout11);

        QHBoxLayout *hLayout12 = new QHBoxLayout();
        ClickableLabel *clicklabel_sobst = new ClickableLabel();
        QString data_sobst = data.at(9);
        QSpacerItem *spacer12 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        splitLines.clear();
        for (int i = 0; i < data_sobst.length(); i += 50) {
            splitLines << data_sobst.mid(i, 50);
        }
        clicklabel_sobst->setText(splitLines.join("\n"));
        clicklabel_sobst->setStyleSheet("font-size: 12px; font-weight: bold;");
        hLayout12->addWidget(clicklabel_sobst);
        hLayout12->addItem(spacer12);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout12);
        layouts_SI_List_center.append(hLayout12);

        QHBoxLayout *hLayout_sp_8 = new QHBoxLayout();
        QLabel *label_sp_8 = new QLabel();
        label_sp_8->setMaximumHeight(5);
        hLayout_sp_8->addWidget(label_sp_8);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout_sp_8);
        layouts_SI_List_center.append(hLayout_sp_8);


        QString data_vvod = data.at(10);
        if (data_vvod.split("@").size() > 1){
            QHBoxLayout *hLayout13 = new QHBoxLayout();
            QLabel *label_vvod = new QLabel();
            QLabel *label_vyvod = new QLabel();
            QSpacerItem *spacer13_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            QSpacerItem *spacer13_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            label_vvod->setText("Дата ввода:");
            label_vvod->setStyleSheet("font-size: 10px;");
            label_vyvod->setText("Дата вывода:");
            label_vyvod->setStyleSheet("font-size: 10px;");
            hLayout13->addWidget(label_vvod);
            hLayout13->addItem(spacer13_2);
            hLayout13->addWidget(label_vyvod);
            hLayout13->addItem(spacer13_3);
            num_center_layouts_List = layouts_SI_List_center.size();
            ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout13);
            layouts_SI_List_center.append(hLayout13);

            QHBoxLayout *hLayout14 = new QHBoxLayout();
            ClickableLabel *clicklabel_vvod = new ClickableLabel();
            ClickableLabel *clicklabel_vyvod = new ClickableLabel();
            QSpacerItem *spacer14_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            QSpacerItem *spacer14_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            clicklabel_vvod->setText(data_vvod.split("@").at(0));
            clicklabel_vvod->setStyleSheet("font-size: 16px; font-weight: bold;");
            clicklabel_vyvod->setText(data_vvod.split("@").at(1));
            clicklabel_vyvod->setStyleSheet("font-size: 16px; font-weight: bold;");
            hLayout14->addWidget(clicklabel_vvod);
            hLayout14->addItem(spacer14_2);
            hLayout14->addWidget(clicklabel_vyvod);
            hLayout14->addItem(spacer14_3);
            num_center_layouts_List = layouts_SI_List_center.size();
            ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout14);
            layouts_SI_List_center.append(hLayout14);
        } else {
            QHBoxLayout *hLayout13 = new QHBoxLayout();
            QLabel *label_vvod = new QLabel();
            QSpacerItem *spacer13 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            label_vvod->setText("Дата ввода:");
            label_vvod->setStyleSheet("font-size: 10px;");
            hLayout13->addWidget(label_vvod);
            hLayout13->addItem(spacer13);
            num_center_layouts_List = layouts_SI_List_center.size();
            ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout13);
            layouts_SI_List_center.append(hLayout13);

            QHBoxLayout *hLayout14 = new QHBoxLayout();
            ClickableLabel *clicklabel_vvod = new ClickableLabel();
            QString data_vvod = data.at(10);
            QSpacerItem *spacer14 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            clicklabel_vvod->setText(data_vvod);
            clicklabel_vvod->setStyleSheet("font-size: 16px; font-weight: bold;");
            hLayout14->addWidget(clicklabel_vvod);
            hLayout14->addItem(spacer14);
            num_center_layouts_List = layouts_SI_List_center.size();
            ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout14);
            layouts_SI_List_center.append(hLayout14);
        }

        QHBoxLayout *hLayout_sp_9 = new QHBoxLayout();
        QLabel *label_sp_9 = new QLabel();
        label_sp_9->setMaximumHeight(5);
        hLayout_sp_9->addWidget(label_sp_9);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout_sp_9);
        layouts_SI_List_center.append(hLayout_sp_9);

        QHBoxLayout *hLayout15 = new QHBoxLayout();
        QLabel *label_diap = new QLabel();
        QLabel *label_pogr = new QLabel();
        QSpacerItem *spacer15_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer15_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_diap->setText("Диапазоны");
        label_diap->setStyleSheet("font-size: 10px;");
        label_pogr->setText("Погрешности");
        label_pogr->setStyleSheet("font-size: 10px;");
        hLayout15->addWidget(label_diap);
        hLayout15->addItem(spacer15_2);
        hLayout15->addWidget(label_pogr);
        hLayout15->addItem(spacer15_3);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout15);
        layouts_SI_List_center.append(hLayout15);

        QString data_diap = data.at(11);
        QStringList diap_list = data_diap.split("@");
        QStringList diapazony_l;
        QStringList pogreshnosty_l;
        for (int z = 0; z < diap_list.size(); z++){
            QStringList one_diap = diap_list.at(z).split(";");
            QString ddd;
            if (one_diap.at(1) == "") {
                ddd = one_diap.at(2) + " " + one_diap.at(4);
            } else {
                ddd = "от " + one_diap.at(1) + " до " + one_diap.at(2) + " " + one_diap.at(4);
            }

            splitLines.clear();
            for (int i = 0; i < ddd.length(); i += 30) {
                splitLines << ddd.mid(i, 30);
            }
            ddd = splitLines.join("\n");

            QString pogr = one_diap.at(3);
            splitLines.clear();
            for (int i = 0; i < pogr.length(); i += 30) {
                splitLines << pogr.mid(i, 30);
            }
            pogr = splitLines.join("\n");

            diapazony_l.append(ddd);
            pogreshnosty_l.append(pogr);
        }
        QHBoxLayout *hLayout16 = new QHBoxLayout();
        QSpacerItem *spacer_d_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer_d_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        ClickableLabel *clicklabel_diap = new ClickableLabel();
        ClickableLabel *clicklabel_pogr = new ClickableLabel();
        QString diapazony_str = diapazony_l.join("\n");
        QString pogreshnosty_str = pogreshnosty_l.join("\n");

        clicklabel_diap->setText(diapazony_str);
        clicklabel_pogr->setText(pogreshnosty_str);

        clicklabel_diap->setStyleSheet("font-size: 11px; font-weight: bold;");
        clicklabel_pogr->setStyleSheet("font-size: 11px; font-weight: bold;");
        hLayout16->addWidget(clicklabel_diap);
        hLayout16->addItem(spacer_d_2);
        hLayout16->addWidget(clicklabel_pogr);
        hLayout16->addItem(spacer_d_3);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout16);
        layouts_SI_List_center.append(hLayout16);

        QHBoxLayout *hLayout_verticalLayout_3_sp_99 = new QHBoxLayout();
        QSpacerItem *verticalLayout_3_sp_99 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        hLayout_verticalLayout_3_sp_99->addItem(verticalLayout_3_sp_99);
        num_center_layouts_List = layouts_SI_List_center.size();
        ui->verticalLayout_3->insertItem(num_center_layouts_List, hLayout_verticalLayout_3_sp_99);
        layouts_SI_List_center.append(hLayout_verticalLayout_3_sp_99);

        QString data_pov = data.at(12);
        if (data_pov != "-"){
            QHBoxLayout *hLayout8 = new QHBoxLayout();
            QLabel *label_pov_ot = new QLabel();
            QLabel *label_pov_org = new QLabel();
            QLabel *label_pov_svid = new QLabel();
            QLabel *label_pov_do = new QLabel();
            label_pov_ot->setText("от");
            // label_pov_org->setText("организация");
            label_pov_svid->setText("свидетельство");
            label_pov_do->setText("до");
            label_pov_ot->setAlignment(Qt::AlignHCenter);
            label_pov_ot->setMaximumWidth(80);
            // label_pov_org->setAlignment(Qt::AlignHCenter);
            label_pov_svid->setAlignment(Qt::AlignHCenter);
            label_pov_do->setAlignment(Qt::AlignHCenter);
            label_pov_do->setMaximumWidth(80);
            hLayout8->addWidget(label_pov_ot);
            // hLayout8->addWidget(label_pov_org);
            hLayout8->addWidget(label_pov_svid);
            hLayout8->addWidget(label_pov_do);
            num_bot_layouts_List = layouts_SI_List_bottom.size();
            ui->verticalLayout_5->insertLayout(num_bot_layouts_List, hLayout8);
            layouts_SI_List_bottom.append(hLayout8);

            QStringList pov_list = data_pov.split("@");
            for (int x = 0; x < pov_list.size(); x++){
                QHBoxLayout *hLayout = new QHBoxLayout();
                QStringList one_pov = pov_list.at(x).split(";");
                ClickableLabel *label_cur_pov_ot = new ClickableLabel();
                // ClickableLabel *label_cur_pov_org = new ClickableLabel();
                ClickableLabel *label_cur_pov_svid = new ClickableLabel();
                ClickableLabel *label_cur_pov_do = new ClickableLabel();

                label_cur_pov_ot->setText(one_pov.at(0));
                label_cur_pov_ot->setMaximumWidth(80);
                // label_cur_pov_org->setText(one_pov.at(1));
                label_cur_pov_svid->setText(one_pov.at(2));
                label_cur_pov_do->setText(one_pov.at(3));
                label_cur_pov_do->setMaximumWidth(80);

                label_cur_pov_ot->setAlignment(Qt::AlignHCenter);
                // label_cur_pov_org->setAlignment(Qt::AlignHCenter);
                label_cur_pov_svid->setAlignment(Qt::AlignHCenter);
                label_cur_pov_do->setAlignment(Qt::AlignHCenter);

                label_cur_pov_ot->setStyleSheet("font-size: 14px; font-weight: bold;");
                // label_cur_pov_org->setStyleSheet("font-size: 14px; font-weight: bold;");
                label_cur_pov_svid->setStyleSheet("font-size: 14px; font-weight: bold;");
                label_cur_pov_do->setStyleSheet("font-size: 14px; font-weight: bold;");

                hLayout->addWidget(label_cur_pov_ot);
                // hLayout->addWidget(label_cur_pov_org);
                hLayout->addWidget(label_cur_pov_svid);
                hLayout->addWidget(label_cur_pov_do);

                num_bot_layouts_List = layouts_SI_List_bottom.size();
                ui->verticalLayout_5->insertLayout(num_bot_layouts_List, hLayout);
                layouts_SI_List_bottom.append(hLayout);
            }
        }

        QHBoxLayout *hLayout_verticalLayout_5_sp_99 = new QHBoxLayout();
        QSpacerItem *verticalLayout_5_sp_99 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        hLayout_verticalLayout_5_sp_99->addItem(verticalLayout_5_sp_99);
        num_bot_layouts_List = layouts_SI_List_bottom.size();
        ui->verticalLayout_5->insertItem(num_bot_layouts_List, hLayout_verticalLayout_5_sp_99);
        layouts_SI_List_bottom.append(hLayout_verticalLayout_5_sp_99);

        QHBoxLayout *hLayout_pom = new QHBoxLayout();
        ClickableLabel *label_pom = new ClickableLabel();
        QSpacerItem *spacer_pom = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_pom = data.at(5);
        label_pom->setStyleSheet("font-size: 11px; font-weight: bold;");
        label_pom->setText("Помещение: " + data_pom);
        hLayout_pom->addItem(spacer_pom);
        hLayout_pom->addWidget(label_pom);
        num_bot_layouts_List = layouts_SI_List_bottom.size();
        ui->verticalLayout_5->insertLayout(num_bot_layouts_List, hLayout_pom);
        layouts_SI_List_bottom.append(hLayout_pom);

        ui->scrollArea->setMinimumWidth(260);
        QString directoryPath = QCoreApplication::applicationDirPath() + "/FILES/1. Средства измерения/" + data.at(0);
        QDir directory(directoryPath);

        if (!directory.exists()) {
            qWarning() << "Directory does not exist:" << directoryPath;
            return;
        }

        QStringList filters;
        filters << "*.jpg" << "*.png";
        directory.setNameFilters(filters);

        QFileInfoList fileList = directory.entryInfoList(QDir::Files);

        QWidget* contentWidget = new QWidget;
        QVBoxLayout* layout = new QVBoxLayout(contentWidget);
        layout->setContentsMargins(2, 2, 2, 2);
        layout->setSpacing(4);

        for (const QFileInfo& fileInfo : fileList) {
            QPixmap pixmap(fileInfo.absoluteFilePath());

            if (!pixmap.isNull()) {
                QLabel* imageLabel = new QLabel;
                pixmap = pixmap.scaledToWidth(200, Qt::SmoothTransformation);
                imageLabel->setPixmap(pixmap);
                imageLabel->setAlignment(Qt::AlignCenter);

                QVBoxLayout* itemLayout = new QVBoxLayout;
                itemLayout->addWidget(imageLabel);

                layout->addLayout(itemLayout);
            }
        }

        layout->addStretch();

        ui->scrollArea->setWidgetResizable(true);
        ui->scrollArea->setWidget(contentWidget);
        ui->frame->setMinimumWidth(570);
    }
}

void MainWindow::fill_layout_IO(const QStringList data, QList<QHBoxLayout*> &layouts_List){

    if (layouts_List.isEmpty()){
        int numlayouts_List = 0;
        int num_center_layouts_List = 0;
        int num_bot_layouts_List = 0;

        QHBoxLayout *hLayout1 = new QHBoxLayout();
        ClickableLabel *label_inv = new ClickableLabel();
        QSpacerItem *spacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer1_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_str = data.at(0);
        label_inv->setText(data_str);
        label_inv->setStyleSheet("font-size: 16px; font-weight: bold;");
        hLayout1->addItem(spacer1_1);
        hLayout1->addWidget(label_inv);
        hLayout1->addItem(spacer1);
        numlayouts_List = layouts_IO_List.size();
        ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout1);
        layouts_IO_List.append(hLayout1);

        QHBoxLayout *hLayout2 = new QHBoxLayout();
        ClickableLabel *text_name = new ClickableLabel();
        QSpacerItem *spacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer2_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_name = data.at(1);
        QStringList splitLines;
        for (int i = 0; i < data_name.length(); i += 60) {
            splitLines << data_name.mid(i, 60);
        }
        text_name->setText(splitLines.join("\n"));
        text_name->setStyleSheet("font-size: 14px; font-weight: bold;");
        text_name->setAlignment(Qt::AlignHCenter);
        hLayout2->addItem(spacer2);
        hLayout2->addWidget(text_name);
        hLayout2->addItem(spacer2_1);
        numlayouts_List = layouts_IO_List.size();
        ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout2);
        layouts_IO_List.append(hLayout2);

        QHBoxLayout *hLayout3 = new QHBoxLayout();
        QLabel *label_zav = new QLabel();
        QSpacerItem *spacer3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_zav->setText("Зав. № ");
        label_zav->setStyleSheet("font-size: 10px;");
        hLayout3->addWidget(label_zav);
        hLayout3->addItem(spacer3);
        num_center_layouts_List = layouts_IO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout3);
        layouts_IO_List_center.append(hLayout3);

        QHBoxLayout *hLayout3_1 = new QHBoxLayout();
        ClickableLabel *label_zav_val = new ClickableLabel();
        QSpacerItem *spacer3_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_zav = data.at(3);
        label_zav_val->setText(data_zav);
        label_zav_val->setStyleSheet("font-size: 14px; font-weight: bold;");
        hLayout3_1->addWidget(label_zav_val);
        hLayout3_1->addItem(spacer3_1);
        num_center_layouts_List = layouts_IO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout3_1);
        layouts_IO_List_center.append(hLayout3_1);

        QHBoxLayout *hLayout4 = new QHBoxLayout();
        QLabel *label_izg = new QLabel();
        QSpacerItem *spacer4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_izg->setText("Изготовитель: ");
        label_izg->setStyleSheet("font-size: 10px;");
        hLayout4->addWidget(label_izg);
        hLayout4->addItem(spacer4);
        num_center_layouts_List = layouts_IO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout4);
        layouts_IO_List_center.append(hLayout4);

        ClickableLabel *label_izg_country = new ClickableLabel();
        ClickableLabel *label_izg_org = new ClickableLabel();
        ClickableLabel *label_izg_year = new ClickableLabel();
        QSpacerItem *spacer4_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer4_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer4_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_izg = data.at(5);
        QStringList izg_list = data_izg.split("@");
        label_izg_country->setText(izg_list.at(0));
        label_izg_org->setText(izg_list.at(1));
        label_izg_year->setText(izg_list.at(2));
        label_izg_country->setStyleSheet("font-size: 12px; font-weight: bold;");
        label_izg_org->setStyleSheet("font-size: 12px; font-weight: bold;");
        label_izg_year->setStyleSheet("font-size: 12px; font-weight: bold;");
        QHBoxLayout *hLayout4_2 = new QHBoxLayout();
        QHBoxLayout *hLayout4_3 = new QHBoxLayout();
        QHBoxLayout *hLayout4_4 = new QHBoxLayout();
        hLayout4_2->addWidget(label_izg_country);
        hLayout4_3->addWidget(label_izg_org);
        hLayout4_4->addWidget(label_izg_year);
        hLayout4_2->addItem(spacer4_2);
        hLayout4_3->addItem(spacer4_3);
        hLayout4_4->addItem(spacer4_4);
        num_center_layouts_List = layouts_IO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout4_2);
        layouts_IO_List_center.append(hLayout4_2);
        num_center_layouts_List = layouts_IO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout4_3);
        layouts_IO_List_center.append(hLayout4_3);
        num_center_layouts_List = layouts_IO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout4_4);
        layouts_IO_List_center.append(hLayout4_4);

        QHBoxLayout *hLayout5 = new QHBoxLayout();
        QLabel *label_pravo = new QLabel();
        QSpacerItem *spacer5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_pravo->setText("Право собственности: ");
        label_pravo->setStyleSheet("font-size: 10px;");
        hLayout5->addWidget(label_pravo);
        hLayout5->addItem(spacer5);
        num_center_layouts_List = layouts_IO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout5);
        layouts_IO_List_center.append(hLayout5);

        QHBoxLayout *hLayout5_1 = new QHBoxLayout();
        ClickableLabel *label_pravo_val = new ClickableLabel();
        QSpacerItem *spacer5_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_pravo = data.at(6);
        splitLines.clear();
        for (int i = 0; i < data_pravo.length(); i += 45) {
            splitLines << data_pravo.mid(i, 45);
        }
        label_pravo_val->setText(splitLines.join("\n"));
        label_pravo_val->setStyleSheet("font-size: 12px; font-weight: bold;");
        hLayout5_1->addWidget(label_pravo_val);
        hLayout5_1->addItem(spacer5_1);
        num_center_layouts_List = layouts_IO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout5_1);
        layouts_IO_List_center.append(hLayout5_1);

        QString data_vvod = data.at(7);
        if (data_vvod.split("@").size() > 1){
            QHBoxLayout *hLayout13 = new QHBoxLayout();
            QLabel *label_vvod = new QLabel();
            QLabel *label_vyvod = new QLabel();
            QSpacerItem *spacer13_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            QSpacerItem *spacer13_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            label_vvod->setText("Дата ввода:");
            label_vvod->setStyleSheet("font-size: 10px;");
            label_vyvod->setText("Дата вывода:");
            label_vyvod->setStyleSheet("font-size: 10px;");
            hLayout13->addWidget(label_vvod);
            hLayout13->addItem(spacer13_2);
            hLayout13->addWidget(label_vyvod);
            hLayout13->addItem(spacer13_3);
            num_center_layouts_List = layouts_IO_List_center.size();
            ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout13);
            layouts_IO_List_center.append(hLayout13);

            QHBoxLayout *hLayout14 = new QHBoxLayout();
            ClickableLabel *clicklabel_vvod = new ClickableLabel();
            ClickableLabel *clicklabel_vyvod = new ClickableLabel();
            QSpacerItem *spacer14_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            QSpacerItem *spacer14_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            clicklabel_vvod->setText(data_vvod.split("@").at(0));
            clicklabel_vvod->setStyleSheet("font-size: 16px; font-weight: bold;");
            clicklabel_vyvod->setText(data_vvod.split("@").at(1));
            clicklabel_vyvod->setStyleSheet("font-size: 16px; font-weight: bold;");
            hLayout14->addWidget(clicklabel_vvod);
            hLayout14->addItem(spacer14_2);
            hLayout14->addWidget(clicklabel_vyvod);
            hLayout14->addItem(spacer14_3);
            num_center_layouts_List = layouts_IO_List_center.size();
            ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout14);
            layouts_IO_List_center.append(hLayout14);
        } else {
            QHBoxLayout *hLayout13 = new QHBoxLayout();
            QLabel *label_vvod = new QLabel();
            QSpacerItem *spacer13 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            label_vvod->setText("Дата ввода:");
            label_vvod->setStyleSheet("font-size: 10px;");
            hLayout13->addWidget(label_vvod);
            hLayout13->addItem(spacer13);
            num_center_layouts_List = layouts_IO_List_center.size();
            ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout13);
            layouts_IO_List_center.append(hLayout13);

            QHBoxLayout *hLayout14 = new QHBoxLayout();
            ClickableLabel *clicklabel_vvod = new ClickableLabel();
            QString data_vvod = data.at(7);
            QSpacerItem *spacer14 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            clicklabel_vvod->setText(data_vvod);
            clicklabel_vvod->setStyleSheet("font-size: 16px; font-weight: bold;");
            hLayout14->addWidget(clicklabel_vvod);
            hLayout14->addItem(spacer14);
            num_center_layouts_List = layouts_IO_List_center.size();
            ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout14);
            layouts_IO_List_center.append(hLayout14);
        }

        QHBoxLayout *hLayout6_6 = new QHBoxLayout();
        QLabel *label_tecn = new QLabel();
        QLabel *label_type = new QLabel();
        QLabel *label_obj = new QLabel();
        QSpacerItem *spacer_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer_1_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer_1_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer_1_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_tecn->setText("Техн. характеристики");
        label_type->setText("Виды испытаний");
        label_obj->setText("Испытуемые объекты");
        label_tecn->setFixedWidth(170);
        label_type->setFixedWidth(170);
        label_obj->setFixedWidth(170);
        label_tecn->setAlignment(Qt::AlignHCenter);
        label_type->setAlignment(Qt::AlignHCenter);
        label_obj->setAlignment(Qt::AlignHCenter);
        label_tecn->setStyleSheet("font-size: 10px;");
        label_type->setStyleSheet("font-size: 10px;");
        label_obj->setStyleSheet("font-size: 10px;");
        hLayout6_6->addItem(spacer_1);
        hLayout6_6->addWidget(label_tecn);
        hLayout6_6->addItem(spacer_1_2);
        hLayout6_6->addWidget(label_type);
        hLayout6_6->addItem(spacer_1_3);
        hLayout6_6->addWidget(label_obj);
        hLayout6_6->addItem(spacer_1_4);
        num_bot_layouts_List = layouts_IO_List_bottom.size();
        ui->verticalLayout_5->insertLayout(num_bot_layouts_List, hLayout6_6);
        layouts_IO_List_bottom.append(hLayout6_6);

        QHBoxLayout *hLayout7 = new QHBoxLayout();
        ClickableLabel *text_tecn = new ClickableLabel();
        ClickableLabel *text_type = new ClickableLabel();
        ClickableLabel *text_obj = new ClickableLabel();
        QSpacerItem *spacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer_2_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer_2_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer_2_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_tecn = data.at(12);
        QString data_type = data.at(11);
        QString data_obj = data.at(10);
        splitLines.clear();
        for (int i = 0; i < data_tecn.length(); i += 23) {
            splitLines << data_tecn.mid(i, 23);
        }
        text_tecn->setText(splitLines.join("\n"));
        splitLines.clear();
        for (int i = 0; i < data_type.length(); i += 23) {
            splitLines << data_type.mid(i, 23);
        }
        text_type->setText(splitLines.join("\n"));
        splitLines.clear();
        for (int i = 0; i < data_obj.length(); i += 23) {
            splitLines << data_obj.mid(i, 23);
        }
        text_obj->setText(splitLines.join("\n"));
        text_tecn->setFixedWidth(170);
        text_type->setFixedWidth(170);
        text_obj->setFixedWidth(170);
        text_tecn->setStyleSheet("font-size: 12px; font-weight: bold;");
        text_type->setStyleSheet("font-size: 12px; font-weight: bold;");
        text_obj->setStyleSheet("font-size: 12px; font-weight: bold;");
        text_tecn->setAlignment(Qt::AlignHCenter);
        text_type->setAlignment(Qt::AlignHCenter);
        text_obj->setAlignment(Qt::AlignHCenter);
        hLayout7->addItem(spacer_2);
        hLayout7->addWidget(text_tecn);
        hLayout7->addItem(spacer_2_2);
        hLayout7->addWidget(text_type);
        hLayout7->addItem(spacer_2_3);
        hLayout7->addWidget(text_obj);
        hLayout7->addItem(spacer_2_4);
        num_bot_layouts_List = layouts_IO_List_bottom.size();
        ui->verticalLayout_5->insertLayout(num_bot_layouts_List, hLayout7);
        layouts_IO_List_bottom.append(hLayout7);

        QHBoxLayout *hLayout_verticalLayout_4_sp_99 = new QHBoxLayout();
        QSpacerItem *verticalLayout_4_sp_99 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        hLayout_verticalLayout_4_sp_99->addItem(verticalLayout_4_sp_99);
        num_bot_layouts_List = layouts_IO_List_bottom.size();
        ui->verticalLayout_5->insertItem(num_bot_layouts_List, hLayout_verticalLayout_4_sp_99);
        layouts_IO_List_bottom.append(hLayout_verticalLayout_4_sp_99);


        QHBoxLayout *hLayout8 = new QHBoxLayout();
        QLabel *label_att_ot = new QLabel();
        // QLabel *label_att_org = new QLabel();
        QLabel *label_att_prot = new QLabel();
        QLabel *label_att_do = new QLabel();
        label_att_ot->setText("от");
        // label_att_org->setText("организация");
        label_att_prot->setText("протокол");
        label_att_do->setText("до");
        label_att_ot->setStyleSheet("font-size: 10px;");
        label_att_prot->setStyleSheet("font-size: 10px;");
        label_att_do->setStyleSheet("font-size: 10px;");
        label_att_ot->setAlignment(Qt::AlignHCenter);
        label_att_ot->setMaximumWidth(80);
        // label_att_org->setAlignment(Qt::AlignHCenter);
        label_att_prot->setAlignment(Qt::AlignHCenter);
        label_att_do->setAlignment(Qt::AlignHCenter);
        label_att_do->setMaximumWidth(80);
        hLayout8->addWidget(label_att_ot);
        // hLayout8->addWidget(label_att_org);
        hLayout8->addWidget(label_att_prot);
        hLayout8->addWidget(label_att_do);
        num_bot_layouts_List = layouts_IO_List_bottom.size();
        ui->verticalLayout_5->insertLayout(num_bot_layouts_List, hLayout8);
        layouts_IO_List_bottom.append(hLayout8);

        QString data_att = data.at(9);
        QStringList att_list = data_att.split("@");
        for (int x = 0; x < att_list.size(); x++){
            QStringList one_att = att_list.at(x).split(";");
            QHBoxLayout *hLayout9 = new QHBoxLayout();
            ClickableLabel *line_att_ot = new ClickableLabel();
            // ClickableLabel *line_att_org = new ClickableLabel();
            ClickableLabel *line_att_prot = new ClickableLabel();
            ClickableLabel *line_att_do = new ClickableLabel();

            line_att_ot->setText(one_att.at(0));
            line_att_ot->setMaximumWidth(80);
            // line_att_org->setText(one_att.at(1));
            line_att_prot->setText(one_att.at(2));
            line_att_do->setText(one_att.at(3));
            line_att_do->setMaximumWidth(80);

            line_att_ot->setStyleSheet("font-size: 12px; font-weight: bold;");
            line_att_prot->setStyleSheet("font-size: 12px; font-weight: bold;");
            line_att_do->setStyleSheet("font-size: 12px; font-weight: bold;");

            line_att_ot->setAlignment(Qt::AlignHCenter);
            // line_att_org->setAlignment(Qt::AlignHCenter);
            line_att_prot->setAlignment(Qt::AlignHCenter);
            line_att_do->setAlignment(Qt::AlignHCenter);

            hLayout9->addWidget(line_att_ot);
            // hLayout9->addWidget(line_att_org);
            hLayout9->addWidget(line_att_prot);
            hLayout9->addWidget(line_att_do);
            num_bot_layouts_List = layouts_IO_List_bottom.size();
            ui->verticalLayout_5->insertLayout(num_bot_layouts_List, hLayout9);
            layouts_IO_List_bottom.append(hLayout9);
        }

        QHBoxLayout *hLayout_verticalLayout_5_sp_99 = new QHBoxLayout();
        QSpacerItem *verticalLayout_5_sp_99 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        hLayout_verticalLayout_5_sp_99->addItem(verticalLayout_5_sp_99);
        num_bot_layouts_List = layouts_IO_List_bottom.size();
        ui->verticalLayout_5->insertItem(num_bot_layouts_List, hLayout_verticalLayout_5_sp_99);
        layouts_IO_List_bottom.append(hLayout_verticalLayout_5_sp_99);

        QHBoxLayout *hLayout_pom = new QHBoxLayout();
        ClickableLabel *label_pom = new ClickableLabel();
        QSpacerItem *spacer_pom = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_pom = data.at(4);
        label_pom->setStyleSheet("font-size: 11px; font-weight: bold;");
        label_pom->setText("Помещение: " + data_pom);
        hLayout_pom->addItem(spacer_pom);
        hLayout_pom->addWidget(label_pom);
        num_bot_layouts_List = layouts_IO_List_bottom.size();
        ui->verticalLayout_5->insertLayout(num_bot_layouts_List, hLayout_pom);
        layouts_IO_List_bottom.append(hLayout_pom);

        ui->scrollArea->setMinimumWidth(260);
        QString directoryPath = QCoreApplication::applicationDirPath() + "/FILES/2. Испытательное оборудование/" + data.at(0);
        QDir directory(directoryPath);

        if (!directory.exists()) {
            qWarning() << "Directory does not exist:" << directoryPath;
            return;
        }

        QStringList filters;
        filters << "*.jpg" << "*.png";
        directory.setNameFilters(filters);
        QFileInfoList fileList = directory.entryInfoList(QDir::Files);

        QWidget* contentWidget = new QWidget;
        QVBoxLayout* layout = new QVBoxLayout(contentWidget);
        layout->setContentsMargins(2, 2, 2, 2);
        layout->setSpacing(4);

        for (const QFileInfo& fileInfo : fileList) {
            QPixmap pixmap(fileInfo.absoluteFilePath());
            if (!pixmap.isNull()) {
                QLabel* imageLabel = new QLabel;
                pixmap = pixmap.scaledToWidth(200, Qt::SmoothTransformation);
                imageLabel->setPixmap(pixmap);
                imageLabel->setAlignment(Qt::AlignCenter);
                QVBoxLayout* itemLayout = new QVBoxLayout;
                itemLayout->addWidget(imageLabel);
                layout->addLayout(itemLayout);
            }
        }

        layout->addStretch();
        ui->scrollArea->setWidgetResizable(true);
        ui->scrollArea->setWidget(contentWidget);
        ui->frame->setMinimumWidth(570);
    }
}

void MainWindow::fill_layout_VO(const QStringList data, QList<QHBoxLayout*> &layouts_List){

    if (layouts_List.isEmpty()){
        int numlayouts_List = 0;
        int num_center_layouts_List = 0;
        int num_bot_layouts_List = 0;

        QHBoxLayout *hLayout1 = new QHBoxLayout();
        ClickableLabel *label_inv = new ClickableLabel();
        QSpacerItem *spacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer1_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_inv = data.at(0);
        label_inv->setText(data_inv);
        label_inv->setStyleSheet("font-size: 16px; font-weight: bold;");
        hLayout1->addItem(spacer1_1);
        hLayout1->addWidget(label_inv);
        hLayout1->addItem(spacer1);
        numlayouts_List = layouts_VO_List.size();
        ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout1);
        layouts_VO_List.append(hLayout1);

        QHBoxLayout *hLayout2 = new QHBoxLayout();
        ClickableLabel *text_name = new ClickableLabel();
        QSpacerItem *spacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer2_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_name = data.at(1);
        QStringList splitLines;
        for (int i = 0; i < data_name.length(); i += 60) {
            splitLines << data_name.mid(i, 60);
        }
        text_name->setText(splitLines.join("\n"));
        text_name->setStyleSheet("font-size: 14px; font-weight: bold;");
        text_name->setAlignment(Qt::AlignHCenter);
        hLayout2->addItem(spacer2);
        hLayout2->addWidget(text_name);
        hLayout2->addItem(spacer2_1);
        numlayouts_List = layouts_VO_List.size();
        ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout2);
        layouts_VO_List.append(hLayout2);

        QHBoxLayout *hLayout3 = new QHBoxLayout();
        QLabel *label_zav = new QLabel();
        QSpacerItem *spacer3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_zav->setText("Зав. № ");
        label_zav->setStyleSheet("font-size: 10px;");
        hLayout3->addWidget(label_zav);
        hLayout3->addItem(spacer3);
        num_center_layouts_List = layouts_VO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout3);
        layouts_VO_List_center.append(hLayout3);

        QHBoxLayout *hLayout3_1 = new QHBoxLayout();
        ClickableLabel *label_zav_val = new ClickableLabel();
        QSpacerItem *spacer3_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_zav = data.at(2);
        label_zav_val->setText(data_zav);
        label_zav_val->setStyleSheet("font-size: 14px; font-weight: bold;");
        hLayout3_1->addWidget(label_zav_val);
        hLayout3_1->addItem(spacer3_1);
        num_center_layouts_List = layouts_VO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout3_1);
        layouts_VO_List_center.append(hLayout3_1);

        QHBoxLayout *hLayout4 = new QHBoxLayout();
        QLabel *label_izg = new QLabel();
        QSpacerItem *spacer4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_izg->setText("Изготовитель: ");
        label_izg->setStyleSheet("font-size: 10px;");
        hLayout4->addWidget(label_izg);
        hLayout4->addItem(spacer4);
        num_center_layouts_List = layouts_VO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout4);
        layouts_VO_List_center.append(hLayout4);

        ClickableLabel *label_izg_country = new ClickableLabel();
        ClickableLabel *label_izg_org = new ClickableLabel();
        ClickableLabel *label_izg_year = new ClickableLabel();
        QSpacerItem *spacer4_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer4_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer4_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_izg = data.at(4);
        QStringList izg_list = data_izg.split("@");
        label_izg_country->setText(izg_list.at(0));
        label_izg_org->setText(izg_list.at(1));
        label_izg_year->setText(izg_list.at(2));
        label_izg_country->setStyleSheet("font-size: 12px; font-weight: bold;");
        label_izg_org->setStyleSheet("font-size: 12px; font-weight: bold;");
        label_izg_year->setStyleSheet("font-size: 12px; font-weight: bold;");
        QHBoxLayout *hLayout4_2 = new QHBoxLayout();
        QHBoxLayout *hLayout4_3 = new QHBoxLayout();
        QHBoxLayout *hLayout4_4 = new QHBoxLayout();
        hLayout4_2->addWidget(label_izg_country);
        hLayout4_3->addWidget(label_izg_org);
        hLayout4_4->addWidget(label_izg_year);
        hLayout4_2->addItem(spacer4_2);
        hLayout4_3->addItem(spacer4_3);
        hLayout4_4->addItem(spacer4_4);
        num_center_layouts_List = layouts_VO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout4_2);
        layouts_VO_List_center.append(hLayout4_2);
        num_center_layouts_List = layouts_VO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout4_3);
        layouts_VO_List_center.append(hLayout4_3);
        num_center_layouts_List = layouts_VO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout4_4);
        layouts_VO_List_center.append(hLayout4_4);

        QHBoxLayout *hLayout5 = new QHBoxLayout();
        QLabel *label_pravo = new QLabel();
        QSpacerItem *spacer5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_pravo->setText("Право собственности: ");
        label_pravo->setStyleSheet("font-size: 10px;");
        hLayout5->addWidget(label_pravo);
        hLayout5->addItem(spacer5);
        num_center_layouts_List = layouts_VO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout5);
        layouts_VO_List_center.append(hLayout5);

        QHBoxLayout *hLayout5_1 = new QHBoxLayout();
        ClickableLabel *label_pravo_val = new ClickableLabel();
        QSpacerItem *spacer5_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_pravo = data.at(5);
        splitLines.clear();
        for (int i = 0; i < data_pravo.length(); i += 45) {
            splitLines << data_pravo.mid(i, 45);
        }
        label_pravo_val->setText(splitLines.join("\n"));
        label_pravo_val->setStyleSheet("font-size: 12px; font-weight: bold;");
        hLayout5_1->addWidget(label_pravo_val);
        hLayout5_1->addItem(spacer5_1);
        num_center_layouts_List = layouts_VO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout5_1);
        layouts_VO_List_center.append(hLayout5_1);

        QString data_vvod = data.at(6);
        if (data_vvod.split("@").size() > 1){
            QHBoxLayout *hLayout13 = new QHBoxLayout();
            QLabel *label_vvod = new QLabel();
            QLabel *label_vyvod = new QLabel();
            QSpacerItem *spacer13_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            QSpacerItem *spacer13_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            label_vvod->setText("Дата ввода:");
            label_vvod->setStyleSheet("font-size: 10px;");
            label_vyvod->setText("Дата вывода:");
            label_vyvod->setStyleSheet("font-size: 10px;");
            hLayout13->addWidget(label_vvod);
            hLayout13->addItem(spacer13_2);
            hLayout13->addWidget(label_vyvod);
            hLayout13->addItem(spacer13_3);
            num_center_layouts_List = layouts_VO_List_center.size();
            ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout13);
            layouts_VO_List_center.append(hLayout13);

            QHBoxLayout *hLayout14 = new QHBoxLayout();
            ClickableLabel *clicklabel_vvod = new ClickableLabel();
            ClickableLabel *clicklabel_vyvod = new ClickableLabel();
            QSpacerItem *spacer14_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            QSpacerItem *spacer14_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            clicklabel_vvod->setText(data_vvod.split("@").at(0));
            clicklabel_vvod->setStyleSheet("font-size: 16px; font-weight: bold;");
            clicklabel_vyvod->setText(data_vvod.split("@").at(1));
            clicklabel_vyvod->setStyleSheet("font-size: 16px; font-weight: bold;");
            hLayout14->addWidget(clicklabel_vvod);
            hLayout14->addItem(spacer14_2);
            hLayout14->addWidget(clicklabel_vyvod);
            hLayout14->addItem(spacer14_3);
            num_center_layouts_List = layouts_VO_List_center.size();
            ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout14);
            layouts_VO_List_center.append(hLayout14);
        } else {
            QHBoxLayout *hLayout13 = new QHBoxLayout();
            QLabel *label_vvod = new QLabel();
            QSpacerItem *spacer13 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            label_vvod->setText("Дата ввода:");
            label_vvod->setStyleSheet("font-size: 10px;");
            hLayout13->addWidget(label_vvod);
            hLayout13->addItem(spacer13);
            num_center_layouts_List = layouts_VO_List_center.size();
            ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout13);
            layouts_VO_List_center.append(hLayout13);

            QHBoxLayout *hLayout14 = new QHBoxLayout();
            ClickableLabel *clicklabel_vvod = new ClickableLabel();
            QSpacerItem *spacer14 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            clicklabel_vvod->setText(data_vvod);
            clicklabel_vvod->setStyleSheet("font-size: 16px; font-weight: bold;");
            hLayout14->addWidget(clicklabel_vvod);
            hLayout14->addItem(spacer14);
            num_center_layouts_List = layouts_VO_List_center.size();
            ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout14);
            layouts_VO_List_center.append(hLayout14);
        }

        QHBoxLayout *hLayout6 = new QHBoxLayout();
        QLabel *label_pred = new QLabel();
        QSpacerItem *spacer6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        label_pred->setText("Предназначение: ");
        label_pred->setStyleSheet("font-size: 10px;");
        hLayout6->addWidget(label_pred);
        hLayout6->addItem(spacer6);
        num_center_layouts_List = layouts_VO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout6);
        layouts_VO_List_center.append(hLayout6);

        QHBoxLayout *hLayout6_1 = new QHBoxLayout();
        ClickableLabel *label_pred_val = new ClickableLabel();
        QSpacerItem *spacer6_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_pred = data.at(7);
        splitLines.clear();
        for (int i = 0; i < data_pred.length(); i += 45) {
            splitLines << data_pred.mid(i, 45);
        }
        label_pred_val->setText(splitLines.join("\n"));
        label_pred_val->setStyleSheet("font-size: 12px; font-weight: bold;");
        hLayout6_1->addWidget(label_pred_val);
        hLayout6_1->addItem(spacer6_1);
        num_center_layouts_List = layouts_VO_List_center.size();
        ui->verticalLayout_3->insertLayout(num_center_layouts_List, hLayout6_1);
        layouts_VO_List_center.append(hLayout6_1);

        QHBoxLayout *hLayout_pom = new QHBoxLayout();
        ClickableLabel *label_pom = new ClickableLabel();
        QSpacerItem *spacer_pom = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_pom = data.at(3);
        label_pom->setStyleSheet("font-size: 11px; font-weight: bold;");
        label_pom->setText("Помещение: " + data_pom);
        hLayout_pom->addItem(spacer_pom);
        hLayout_pom->addWidget(label_pom);
        num_bot_layouts_List = layouts_VO_List_bottom.size();
        ui->verticalLayout_5->insertLayout(num_bot_layouts_List, hLayout_pom);
        layouts_VO_List_bottom.append(hLayout_pom);


        ui->scrollArea->setMinimumWidth(260);
        QString directoryPath = QCoreApplication::applicationDirPath() + "/FILES/3. Вспомогательное оборудование/" + data.at(0);
        QDir directory(directoryPath);
        if (!directory.exists()) {
            qWarning() << "Directory does not exist:" << directoryPath;
            return;
        }
        QStringList filters;
        filters << "*.jpg" << "*.png";
        directory.setNameFilters(filters);
        QFileInfoList fileList = directory.entryInfoList(QDir::Files);

        QWidget* contentWidget = new QWidget;
        QVBoxLayout* layout = new QVBoxLayout(contentWidget);
        layout->setContentsMargins(2, 2, 2, 2);
        layout->setSpacing(4);

        for (const QFileInfo& fileInfo : fileList) {
            QPixmap pixmap(fileInfo.absoluteFilePath());
            if (!pixmap.isNull()) {
                QLabel* imageLabel = new QLabel;
                pixmap = pixmap.scaledToWidth(200, Qt::SmoothTransformation);
                imageLabel->setPixmap(pixmap);
                imageLabel->setAlignment(Qt::AlignCenter);
                QVBoxLayout* itemLayout = new QVBoxLayout;
                itemLayout->addWidget(imageLabel);
                layout->addLayout(itemLayout);
            }
        }
        layout->addStretch();
        ui->scrollArea->setWidgetResizable(true);
        ui->scrollArea->setWidget(contentWidget);
        ui->frame->setMinimumWidth(570);
    }
}

void MainWindow::fill_layout_Arenda(const QStringList data, QList<QHBoxLayout*> &layouts_List){

    if (layouts_List.isEmpty()){
        int numlayouts_List;

        QHBoxLayout *hLayout01 = new QHBoxLayout();
        QLabel *lineEdit_type = new QLabel();
        QSpacerItem *spacer01 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem *spacer02 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_type = data.at(2);
        lineEdit_type->setText("Тип оборудования: " + data_type);
        hLayout01->addItem(spacer01);
        hLayout01->addWidget(lineEdit_type);
        hLayout01->addItem(spacer02);
        ui->verticalLayout_2->insertLayout(0, hLayout01);
        layouts_List.append(hLayout01);

        QHBoxLayout *hLayout1 = new QHBoxLayout();
        QLabel *lineEdit_inv = new QLabel();
        QSpacerItem *spacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_inv = data.at(0);
        lineEdit_inv->setText(data_inv);
        hLayout1->addWidget(lineEdit_inv);
        hLayout1->addItem(spacer1);
        numlayouts_List = layouts_List.size();
        ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout1);
        layouts_List.append(hLayout1);

        QHBoxLayout *hLayout2 = new QHBoxLayout();
        QLabel *lineEdit_name = new QLabel();
        QString data_name = data.at(1);
        QSpacerItem *spacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        lineEdit_name->setText(data_name);
        hLayout2->addWidget(lineEdit_name);
        hLayout2->addItem(spacer2);
        numlayouts_List = layouts_List.size();
        ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout2);
        layouts_List.append(hLayout2);

        if (data.at(2) == "СИ"){
            QHBoxLayout *hLayout3 = new QHBoxLayout();
            QLabel *label_reestr = new QLabel();
            QLabel *label_zav = new QLabel();
            QLabel *lineEdit_reestr = new QLabel();
            QLabel *lineEdit_zav = new QLabel();
            QString data_reestr = data.at(5);
            QString data_zav = data.at(3);
            QSpacerItem *spacer3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            label_reestr->setText("Номер в гос. реестре: ");
            label_zav->setText(" |    Зав. № ");
            lineEdit_reestr->setText(data_reestr);
            lineEdit_zav->setText(data_zav);
            hLayout3->addWidget(label_reestr);
            hLayout3->addWidget(lineEdit_reestr);
            hLayout3->addWidget(label_zav);
            hLayout3->addWidget(lineEdit_zav);
            hLayout3->addItem(spacer3);
            numlayouts_List = layouts_List.size();
            ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout3);
            layouts_List.append(hLayout3);
        } else {
            QHBoxLayout *hLayout3 = new QHBoxLayout();
            QLabel *label_zav = new QLabel();
            QLabel *lineEdit_zav = new QLabel();
            QString data_zav = data.at(3);
            QSpacerItem *spacer3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            label_zav->setText("Зав. № ");
            lineEdit_zav->setText(data_zav);
            hLayout3->addWidget(label_zav);
            hLayout3->addWidget(lineEdit_zav);
            hLayout3->addItem(spacer3);
            numlayouts_List = layouts_List.size();
            ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout3);
            layouts_List.append(hLayout3);
        }

        if (data.at(2) == "ВО"){
            QHBoxLayout *hLayout4 = new QHBoxLayout();
            QLabel *label_pred = new QLabel();
            QLabel *lineEdit_pred = new QLabel();
            QString data_pred = data.at(4);
            QSpacerItem *spacer4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            label_pred->setText("Предназначение: ");
            lineEdit_pred->setText(data_pred);
            hLayout4->addWidget(label_pred);
            hLayout4->addWidget(lineEdit_pred);
            hLayout4->addItem(spacer4);
            numlayouts_List = layouts_List.size();
            ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout4);
            layouts_List.append(hLayout4);
        }

        QHBoxLayout *hLayout5 = new QHBoxLayout();
        QLabel *label_izg = new QLabel();
        QLabel *lineEdit_izg_country = new QLabel();
        QLabel *lineEdit_izg_org = new QLabel();
        QLabel *lineEdit_izg_year = new QLabel();
        QSpacerItem *spacer5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_izg = data.at(6);
        QStringList izg_list = data_izg.split("@");
        label_izg->setText("Изготовитель: ");
        lineEdit_izg_country->setText(izg_list.at(0));
        lineEdit_izg_org->setText(izg_list.at(1));
        lineEdit_izg_year->setText(izg_list.at(2));
        hLayout5->addWidget(label_izg);
        hLayout5->addWidget(lineEdit_izg_country);
        hLayout5->addWidget(lineEdit_izg_org);
        hLayout5->addWidget(lineEdit_izg_year);
        hLayout5->addItem(spacer5);
        numlayouts_List = layouts_List.size();
        ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout5);
        layouts_List.append(hLayout5);

        QHBoxLayout *hLayout6 = new QHBoxLayout();
        QLabel *label_pravo = new QLabel();
        QLabel *lineEdit_pravo = new QLabel();
        QSpacerItem *spacer6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QString data_pravo = data.at(7);
        QString data_date = data.at(10);
        label_pravo->setText("Право собственности: ");
        lineEdit_pravo->setText(data_pravo);
        hLayout6->addWidget(label_pravo);
        hLayout6->addWidget(lineEdit_pravo);
        QStringList date_list = data_date.split("@");
        if (date_list.size() > 1) {
            QLabel *label_date = new QLabel();
            QLabel *lineEdit_date = new QLabel();
            QLabel *label_date_vyvod = new QLabel();
            QLabel *lineEdit_date_vyvod = new QLabel();
            label_date->setText(" |    Дата ввода: ");
            lineEdit_date->setText(date_list.at(0));
            label_date_vyvod->setText(" |    Дата вывода: ");
            lineEdit_date_vyvod->setText(date_list.at(1));
            hLayout6->addWidget(label_date);
            hLayout6->addWidget(lineEdit_date);
            hLayout6->addWidget(label_date_vyvod);
            hLayout6->addWidget(lineEdit_date_vyvod);
        } else {
            QLabel *label_date = new QLabel();
            QLabel *lineEdit_date = new QLabel();
            label_date->setText(" |    Дата ввода: ");
            lineEdit_date->setText(date_list.at(0));
            hLayout6->addWidget(label_date);
            hLayout6->addWidget(lineEdit_date);
        }
        hLayout6->addItem(spacer6);
        numlayouts_List = layouts_List.size();
        ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout6);
        layouts_List.append(hLayout6);

        if (data.at(2) == "СИ"){
            QHBoxLayout *hLayout6_5 = new QHBoxLayout();
            QLabel *label_diap = new QLabel();
            QLabel *label_pogr = new QLabel();
            label_diap->setText("диапазоны");
            label_pogr->setText("погрешность/КТ");
            label_diap->setAlignment(Qt::AlignHCenter);
            label_pogr->setAlignment(Qt::AlignHCenter);
            hLayout6_5->addWidget(label_diap);
            hLayout6_5->addWidget(label_pogr);
            numlayouts_List = layouts_List.size();
            ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout6_5);
            layouts_List.append(hLayout6_5);
            QHBoxLayout *hLayout7 = new QHBoxLayout();
            QTextEdit *text_edit_diap = new QTextEdit();
            QTextEdit *text_edit_pogr = new QTextEdit();
            QString data_diap = data.at(8);
            QStringList diap_list = data_diap.split("@");
            QStringList diapazony_l;
            QStringList pogreshnosty_l;
            for (int z = 0; z < diap_list.size(); z++){
                QStringList one_diap = diap_list.at(z).split(";");
                QString ddd;
                if (one_diap.at(1) == "") {
                    ddd = one_diap.at(2) + " " + one_diap.at(4);
                } else {
                    ddd = "от " + one_diap.at(1) + " до " + one_diap.at(2) + " " + one_diap.at(4);
                }
                diapazony_l.append(ddd);
                pogreshnosty_l.append(one_diap.at(3));
            }
            QString diapazony = diapazony_l.join("\n");
            QString pogreshnosty = pogreshnosty_l.join("\n");
            text_edit_diap->setText(diapazony);
            text_edit_pogr->setText(pogreshnosty);
            text_edit_diap->setMaximumSize(1000, 60);
            text_edit_pogr->setMaximumSize(1000, 60);
            text_edit_diap->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            text_edit_pogr->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            text_edit_diap->setFocusPolicy(Qt::NoFocus);
            text_edit_pogr->setFocusPolicy(Qt::NoFocus);
            text_edit_diap->setReadOnly(true);
            text_edit_pogr->setReadOnly(true);

            hLayout7->addWidget(text_edit_diap);
            hLayout7->addWidget(text_edit_pogr);
            numlayouts_List = layouts_List.size();
            ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout7);
            layouts_List.append(hLayout7);
        }

        QString data_pov = data.at(9);
        if (data.at(2) == "СИ" || data.at(2) == "ИО"){
            QHBoxLayout *hLayout8 = new QHBoxLayout();
            QLabel *label_pov_ot = new QLabel();
            QLabel *label_pov_org = new QLabel();
            QLabel *label_pov_svid = new QLabel();
            QLabel *label_pov_do = new QLabel();
            label_pov_ot->setText("от");
            label_pov_org->setText("организация");
            label_pov_svid->setText("№ документа");
            label_pov_do->setText("до");
            label_pov_ot->setAlignment(Qt::AlignHCenter);
            label_pov_ot->setMaximumWidth(80);
            label_pov_org->setAlignment(Qt::AlignHCenter);
            label_pov_svid->setAlignment(Qt::AlignHCenter);
            label_pov_do->setAlignment(Qt::AlignHCenter);
            label_pov_do->setMaximumWidth(80);
            hLayout8->addWidget(label_pov_ot);
            hLayout8->addWidget(label_pov_org);
            hLayout8->addWidget(label_pov_svid);
            hLayout8->addWidget(label_pov_do);
            numlayouts_List = layouts_List.size();
            ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout8);
            layouts_List.append(hLayout8);

            QStringList pov_list = data_pov.split("@");
            for (int x = 0; x < pov_list.size(); x++){
                QHBoxLayout *hLayout = new QHBoxLayout();
                QStringList one_pov = pov_list.at(x).split(";");
                QLineEdit *label_cur_pov_ot = new QLineEdit();
                QLineEdit *label_cur_pov_org = new QLineEdit();
                QLineEdit *label_cur_pov_svid = new QLineEdit();
                QLineEdit *label_cur_pov_do = new QLineEdit();

                label_cur_pov_ot->setText(one_pov.at(0));
                label_cur_pov_ot->setMaximumWidth(80);
                label_cur_pov_org->setText(one_pov.at(1));
                label_cur_pov_svid->setText(one_pov.at(2));
                label_cur_pov_do->setText(one_pov.at(3));
                label_cur_pov_do->setMaximumWidth(80);

                label_cur_pov_ot->setAlignment(Qt::AlignHCenter);
                label_cur_pov_org->setAlignment(Qt::AlignHCenter);
                label_cur_pov_svid->setAlignment(Qt::AlignHCenter);
                label_cur_pov_do->setAlignment(Qt::AlignHCenter);

                label_cur_pov_ot->setFocusPolicy(Qt::NoFocus);
                label_cur_pov_org->setFocusPolicy(Qt::NoFocus);
                label_cur_pov_svid->setFocusPolicy(Qt::NoFocus);
                label_cur_pov_do->setFocusPolicy(Qt::NoFocus);

                label_cur_pov_ot->setReadOnly(true);
                label_cur_pov_org->setReadOnly(true);
                label_cur_pov_svid->setReadOnly(true);
                label_cur_pov_do->setReadOnly(true);

                hLayout->addWidget(label_cur_pov_ot);
                hLayout->addWidget(label_cur_pov_org);
                hLayout->addWidget(label_cur_pov_svid);
                hLayout->addWidget(label_cur_pov_do);

                numlayouts_List = layouts_List.size();
                ui->verticalLayout_2->insertLayout(numlayouts_List, hLayout);
                layouts_List.append(hLayout);
            }
        }

        QHBoxLayout *hLayout99 = new QHBoxLayout();
        QSpacerItem *spacer99 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        hLayout99->addItem(spacer99);
        numlayouts_List = layouts_List.size();
        ui->verticalLayout_2->insertItem(numlayouts_List, hLayout99);
        layouts_List.append(hLayout99);
    }
}

void MainWindow::clean_layout(){
    if (!layouts_SI_List.isEmpty()) {
        while (!layouts_SI_List.isEmpty()){
            QHBoxLayout *hLayout = layouts_SI_List.takeLast();
            QLayoutItem *item;

            while ((item = hLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            ui->verticalLayout_2->removeItem(hLayout);
            delete hLayout;
        }
    }
    if (!layouts_SI_List_center.isEmpty()) {
        while (!layouts_SI_List_center.isEmpty()){
            QHBoxLayout *hLayout = layouts_SI_List_center.takeLast();
            QLayoutItem *item;

            while ((item = hLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            ui->verticalLayout_3->removeItem(hLayout);
            delete hLayout;
        }
    }
    if (!layouts_SI_List_bottom.isEmpty()) {
        while (!layouts_SI_List_bottom.isEmpty()){
            QHBoxLayout *hLayout = layouts_SI_List_bottom.takeLast();
            QLayoutItem *item;

            while ((item = hLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            ui->verticalLayout_5->removeItem(hLayout);
            delete hLayout;
        }
    }
    if (!layouts_IO_List.isEmpty()) {
        while (!layouts_IO_List.isEmpty()){
            QHBoxLayout *hLayout = layouts_IO_List.takeLast();
            QLayoutItem *item;

            while ((item = hLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            ui->verticalLayout_2->removeItem(hLayout);
            delete hLayout;
        }
    }
    if (!layouts_IO_List_center.isEmpty()) {
        while (!layouts_IO_List_center.isEmpty()){
            QHBoxLayout *hLayout = layouts_IO_List_center.takeLast();
            QLayoutItem *item;

            while ((item = hLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            ui->verticalLayout_3->removeItem(hLayout);
            delete hLayout;
        }
    }
    if (!layouts_IO_List_bottom.isEmpty()) {
        while (!layouts_IO_List_bottom.isEmpty()){
            QHBoxLayout *hLayout = layouts_IO_List_bottom.takeLast();
            QLayoutItem *item;

            while ((item = hLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            ui->verticalLayout_5->removeItem(hLayout);
            delete hLayout;
        }
    }
    if (!layouts_VO_List.isEmpty()) {
        while (!layouts_VO_List.isEmpty()){
            QHBoxLayout *hLayout = layouts_VO_List.takeLast();
            QLayoutItem *item;

            while ((item = hLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            ui->verticalLayout_2->removeItem(hLayout);
            delete hLayout;
        }
    }
    if (!layouts_VO_List_center.isEmpty()) {
        while (!layouts_VO_List_center.isEmpty()){
            QHBoxLayout *hLayout = layouts_VO_List_center.takeLast();
            QLayoutItem *item;

            while ((item = hLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            ui->verticalLayout_3->removeItem(hLayout);
            delete hLayout;
        }
    }
    if (!layouts_VO_List_bottom.isEmpty()) {
        while (!layouts_VO_List_bottom.isEmpty()){
            QHBoxLayout *hLayout = layouts_VO_List_bottom.takeLast();
            QLayoutItem *item;

            while ((item = hLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            ui->verticalLayout_5->removeItem(hLayout);
            delete hLayout;
        }
    }
    if (!layouts_Arenda_List.isEmpty()) {
        while (!layouts_Arenda_List.isEmpty()){
            QHBoxLayout *hLayout = layouts_Arenda_List.takeLast();
            QLayoutItem *item;

            while ((item = hLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            ui->verticalLayout_2->removeItem(hLayout);
            delete hLayout;
        }
    }
}

void MainWindow::set_table(const QString table_name){
    delete model;
    delete sqlmodel;
    Database db2;
    db2.openDatabase("database.db");
    sqlmodel = new QSqlQueryModel();
    QSqlQuery query(db2.db_r());

    if (table_name == "SI"){
        query.prepare("SELECT * FROM SI ORDER BY CAST(inv AS INTEGER)");
        if (!query.exec()) {
            qDebug() << "Ошибка модели СИ: " << query.lastError().text();
            delete sqlmodel;
        }
        sqlmodel->setQuery(query);
        while (sqlmodel->canFetchMore()) {
            sqlmodel->fetchMore();
        }

        model = new QStandardItemModel(this);
        QStringList headers;
        for(int i = 0; i < sqlmodel->columnCount(); ++i) {
            headers << sqlmodel->headerData(i, Qt::Horizontal).toString();
        }
        model->setHorizontalHeaderLabels(headers);
        for(int row = 0; row < sqlmodel->rowCount(); ++row) {
            bool is_exist = true;
            QList<QStandardItem*> items;
            for(int col = 0; (col < sqlmodel->columnCount() - 1); ++col) {
                if (col == 2){
                    continue;
                } else if (col == 3){
                    QStandardItem* item = new QStandardItem(
                        sqlmodel->data(sqlmodel->index(row, 2)).toString() + " " +
                        sqlmodel->data(sqlmodel->index(row, col)).toString()
                        );
                    items.append(item);
                } else if (col == 11) {
                    QStandardItem* item = new QStandardItem(
                        sqlmodel->data(sqlmodel->index(row, col)).toString()
                        );
                    QString date = item->text();
                    QStringList date_List = date.split("@");
                    if (date_List.size() > 1) {
                        is_exist = false;
                    }
                } else {
                    QStandardItem* item = new QStandardItem(
                        sqlmodel->data(sqlmodel->index(row, col)).toString()
                        );
                    items.append(item);
                }
            }
            if (!is_exist){
                for (QStandardItem* item : items) {
                    item->setBackground(QBrush(QColor("#f2d7d5")));
                    item->setForeground(QBrush(QColor("#943126")));
                }
            }
            model->appendRow(items);
        }
        qDebug() << "Модель СИ установлена";

    } else if (table_name == "IO"){
        query.prepare("SELECT * FROM IO ORDER BY CAST(inv AS INTEGER)");
        if (!query.exec()) {
            qDebug() << "Ошибка модели ИО: " << query.lastError().text();
            delete sqlmodel;
        }
        sqlmodel->setQuery(query);
        while (sqlmodel->canFetchMore()) {
            sqlmodel->fetchMore();
        }
        model = new QStandardItemModel(this);
        QStringList headers;
        for(int i = 0; i < sqlmodel->columnCount(); ++i) {
            headers << sqlmodel->headerData(i, Qt::Horizontal).toString();
        }
        model->setHorizontalHeaderLabels(headers);
        for(int row = 0; row < sqlmodel->rowCount(); ++row) {
            bool is_exist = true;
            QList<QStandardItem*> items;
            for(int col = 0; col < sqlmodel->columnCount(); ++col) {
                QStandardItem* item = new QStandardItem(
                    sqlmodel->data(sqlmodel->index(row, col)).toString()
                    );
                if (col == 8){
                    QString date = item->text();
                    QStringList date_List = date.split("@");
                    if (date_List.size() > 1) {
                        is_exist = false;
                    }
                }
                items.append(item);
            }
            if (!is_exist){
                for (QStandardItem* item : items) {
                    item->setBackground(QBrush(QColor("#f2d7d5")));
                    item->setForeground(QBrush(QColor("#943126")));
                }
            }
            model->appendRow(items);
        }
        qDebug() << "Модель ИО установлена";

    } else if (table_name == "VO"){
        query.prepare("SELECT * FROM VO ORDER BY CAST(inv AS INTEGER)");
        if (!query.exec()) {
            qDebug() << "Ошибка модели ВО: " << query.lastError().text();
            delete sqlmodel;
        }
        sqlmodel->setQuery(query);
        while (sqlmodel->canFetchMore()) {
            sqlmodel->fetchMore();
        }
        model = new QStandardItemModel(this);
        QStringList headers;
        for(int i = 0; i < sqlmodel->columnCount(); ++i) {
            headers << sqlmodel->headerData(i, Qt::Horizontal).toString();
        }
        model->setHorizontalHeaderLabels(headers);
        for(int row = 0; row < sqlmodel->rowCount(); ++row) {
            bool is_exist = true;
            QList<QStandardItem*> items;
            for(int col = 0; col < sqlmodel->columnCount(); ++col) {
                QStandardItem* item = new QStandardItem(
                    sqlmodel->data(sqlmodel->index(row, col)).toString()
                    );
                if (col == 7){
                    QString date = item->text();
                    QStringList date_List = date.split("@");
                    if (date_List.size() > 1) {
                        is_exist = false;
                    }
                }
                items.append(item);
            }
            if (!is_exist){
                for (QStandardItem* item : items) {
                    item->setBackground(QBrush(QColor("#f2d7d5")));
                    item->setForeground(QBrush(QColor("#943126")));
                }
            }
            model->appendRow(items);
        }
        qDebug() << "Модель ВО установлена";

    } else if (table_name == "Prtkl"){
        query.prepare("SELECT * FROM Prtkl WHERE num LIKE 'ППБ-%' ORDER BY CAST(SUBSTR(num, 5) AS INTEGER)");
        // query.prepare("SELECT * FROM VO ORDER BY CAST(inv AS INTEGER)");
        if (!query.exec()) {
            qDebug() << "Ошибка модели Prtkl: " << query.lastError().text();
            delete sqlmodel;
        }
        sqlmodel->setQuery(query);
        while (sqlmodel->canFetchMore()) {
            sqlmodel->fetchMore();
        }
        model = new QStandardItemModel(this);
        QStringList headers;
        for(int i = 0; i < sqlmodel->columnCount(); ++i) {
            headers << sqlmodel->headerData(i, Qt::Horizontal).toString();
        }
        model->setHorizontalHeaderLabels(headers);
        for(int row = 0; row < sqlmodel->rowCount(); ++row) {
            QList<QStandardItem*> items;
            for(int col = 0; col < sqlmodel->columnCount(); ++col) {
                QStandardItem* item = new QStandardItem(
                    sqlmodel->data(sqlmodel->index(row, col)).toString()
                    );
                items.append(item);
            }
            model->appendRow(items);
        }
        qDebug() << "Модель Prtkl установлена";

    } else if (table_name == "Arenda"){
        query.prepare("SELECT * FROM Arenda");
        if (!query.exec()) {
            qDebug() << "Ошибка модели Arenda: " << query.lastError().text();
            delete sqlmodel;
        }
        sqlmodel->setQuery(query);
        while (sqlmodel->canFetchMore()) {
            sqlmodel->fetchMore();
        }
        model = new QStandardItemModel(this);
        QStringList headers;
        for(int i = 0; i < sqlmodel->columnCount(); ++i) {
            headers << sqlmodel->headerData(i, Qt::Horizontal).toString();
        }
        model->setHorizontalHeaderLabels(headers);
        for(int row = 0; row < sqlmodel->rowCount(); ++row) {
            QList<QStandardItem*> items;
            bool is_exist = true;
            for(int col = 0; col < sqlmodel->columnCount(); ++col) {
                QStandardItem* item = new QStandardItem(
                    sqlmodel->data(sqlmodel->index(row, col)).toString()
                    );
                if (col == 11){
                    QString date = item->text();
                    QStringList date_List = date.split("@");
                    if (date_List.size() > 1) {
                        is_exist = false;
                    }
                }
                items.append(item);
            }
            if (!is_exist){
                for (QStandardItem* item : items) {
                    item->setBackground(QBrush(QColor("#f2d7d5")));
                    item->setForeground(QBrush(QColor("#943126")));
                }
            }
            model->appendRow(items);
        }
        qDebug() << "Модель Arenda установлена";
    }
    db2.close_conn();
    ui->tableView->setModel(model);
    set_table_columns();
    ui->tableView->show();
}

void MainWindow::set_table_columns(){
    if (this->table == 1){
        ui->tableView->setColumnHidden(0, true);
        ui->tableView->setColumnHidden(1, false);
        ui->tableView->setColumnHidden(2, false);
        ui->tableView->setColumnHidden(3, true);
        ui->tableView->setColumnHidden(4, true);
        ui->tableView->setColumnHidden(5, true);
        ui->tableView->setColumnHidden(6, false);
        ui->tableView->setColumnHidden(7, true);
        ui->tableView->setColumnHidden(8, true);
        ui->tableView->setColumnHidden(9, true);
        ui->tableView->setColumnHidden(10, true);
        ui->tableView->setColumnHidden(11, true);
        ui->tableView->setColumnHidden(12, true);
        ui->tableView->setColumnHidden(13, true);
        ui->tableView->setColumnHidden(14, true);
        ui->tableView->setColumnHidden(15, true);
        ui->tableView->setColumnHidden(16, true);
        ui->tableView->setColumnHidden(17, true);
        ui->tableView->setColumnHidden(18, true);
        ui->tableView->setColumnHidden(19, true);
        ui->tableView->setColumnHidden(20, true);
        ui->tableView->setColumnHidden(21, true);
        ui->tableView->setColumnHidden(22, true);
        QHeaderView* header = ui->tableView->horizontalHeader();

        ui->tableView->setColumnWidth(1, 400);

        header->setSectionResizeMode(QHeaderView::Interactive);

        QList<int> visibleColumns;
        for (int i = 0; i < model->columnCount(); ++i) {
            if (!ui->tableView->isColumnHidden(i)) {
                visibleColumns.append(i);
            }
        }

        for (int col : visibleColumns) {
            if (col == 1) {
                header->setSectionResizeMode(col, QHeaderView::Interactive);
                header->setMaximumSectionSize(400);
            } else if (col == visibleColumns.last()) {
                header->setSectionResizeMode(col, QHeaderView::Stretch);
            } else {
                header->setSectionResizeMode(col, QHeaderView::ResizeToContents);
            }
        }
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setMinimumWidth(541);
        ui->tableView->setMaximumWidth(541);
        // ui->frame->setMinimumWidth(2200);
        ui->frame->setHidden(true);
        ui->pushButton_form->setHidden(false);
        ui->pushButton_type->setHidden(false);
    } else if (this->table == 2) {
        ui->tableView->setColumnHidden(0, true);
        ui->tableView->setColumnHidden(1, false);
        ui->tableView->setColumnHidden(2, true);
        ui->tableView->setColumnHidden(3, false);
        ui->tableView->setColumnHidden(4, true);
        ui->tableView->setColumnHidden(5, true);
        ui->tableView->setColumnHidden(6, true);
        ui->tableView->setColumnHidden(7, true);
        ui->tableView->setColumnHidden(8, true);
        ui->tableView->setColumnHidden(9, true);
        ui->tableView->setColumnHidden(10, true);
        ui->tableView->setColumnHidden(11, true);
        ui->tableView->setColumnHidden(12, true);
        ui->tableView->setColumnHidden(13, true);
        ui->tableView->setColumnHidden(14, true);
        ui->tableView->setColumnHidden(15, true);
        ui->tableView->setColumnHidden(16, true);
        ui->tableView->setColumnHidden(17, true);
        ui->tableView->setColumnHidden(18, true);
        ui->tableView->setColumnHidden(19, true);
        ui->tableView->setColumnHidden(20, true);
        ui->tableView->setColumnHidden(21, true);
        ui->tableView->setColumnHidden(22, true);
        QHeaderView* header = ui->tableView->horizontalHeader();

        ui->tableView->setColumnWidth(3, 200);

        header->setSectionResizeMode(QHeaderView::Interactive);

        QList<int> visibleColumns;
        for (int i = 0; i < model->columnCount(); ++i) {
            if (!ui->tableView->isColumnHidden(i)) {
                visibleColumns.append(i);
            }
        }

        for (int col : visibleColumns) {
            if (col == 1 || col == 2) {

                header->setSectionResizeMode(col, QHeaderView::Interactive);
                header->setMaximumSectionSize(300);
            } else if (col == visibleColumns.last()) {

                header->setSectionResizeMode(col, QHeaderView::Stretch);
            } else {

                header->setSectionResizeMode(col, QHeaderView::ResizeToContents);
            }
        }

        ui->tableView->resizeColumnsToContents();
        ui->tableView->setMinimumWidth(281);
        ui->tableView->setMaximumWidth(281);
        ui->frame->setHidden(true);
        ui->pushButton_form->setHidden(false);
        ui->pushButton_type->setHidden(true);
    } else if (this->table == 3) {
        ui->tableView->setColumnHidden(0, true);
        ui->tableView->setColumnHidden(1, false);
        ui->tableView->setColumnHidden(2, false);
        ui->tableView->setColumnHidden(3, true);
        ui->tableView->setColumnHidden(4, true);
        ui->tableView->setColumnHidden(5, true);
        ui->tableView->setColumnHidden(6, true);
        ui->tableView->setColumnHidden(7, true);
        ui->tableView->setColumnHidden(8, true);
        ui->tableView->setColumnHidden(9, true);
        ui->tableView->setColumnHidden(10, true);
        ui->tableView->setColumnHidden(11, true);
        ui->tableView->setColumnHidden(12, true);
        ui->tableView->setColumnHidden(13, true);
        ui->tableView->setColumnHidden(14, true);
        ui->tableView->setColumnHidden(15, true);
        ui->tableView->setColumnHidden(16, true);
        ui->tableView->setColumnHidden(17, true);
        ui->tableView->setColumnHidden(18, true);
        ui->tableView->setColumnHidden(19, true);
        ui->tableView->setColumnHidden(20, true);
        ui->tableView->setColumnHidden(21, true);
        ui->tableView->setColumnHidden(22, true);
        QHeaderView* header = ui->tableView->horizontalHeader();
        header->setSectionResizeMode(QHeaderView::Interactive);

        QList<int> visibleColumns;
        for (int i = 0; i < model->columnCount(); ++i) {
            if (!ui->tableView->isColumnHidden(i)) {
                visibleColumns.append(i);
            }
        }

        for (int i = 0; i < visibleColumns.size(); ++i) {
            if (i < visibleColumns.size() - 1) {
                header->setSectionResizeMode(visibleColumns[i], QHeaderView::ResizeToContents);
            } else {
                header->setSectionResizeMode(visibleColumns[i], QHeaderView::Stretch);
            }
        }

        ui->tableView->resizeColumnsToContents();
        ui->tableView->setMinimumWidth(541);
        ui->tableView->setMaximumWidth(541);
        ui->frame->setHidden(true);
        ui->pushButton_form->setHidden(false);
        ui->pushButton_type->setHidden(true);

    } else if (this->table == 4) {
        ui->tableView->setColumnHidden(0, true);
        ui->tableView->setColumnHidden(1, false);
        ui->tableView->setColumnHidden(2, false);
        ui->tableView->setColumnHidden(3, true);
        ui->tableView->setColumnHidden(4, true);
        ui->tableView->setColumnHidden(5, true);
        ui->tableView->setColumnHidden(6, true);
        ui->tableView->setColumnHidden(7, true);
        ui->tableView->setColumnHidden(8, true);
        ui->tableView->setColumnHidden(9, true);
        ui->tableView->setColumnHidden(10, true);
        ui->tableView->setColumnHidden(11, true);
        ui->tableView->setColumnHidden(12, true);
        ui->tableView->setColumnHidden(13, true);
        ui->tableView->setColumnHidden(14, true);
        ui->tableView->setColumnHidden(15, true);
        ui->tableView->setColumnHidden(16, true);
        ui->tableView->setColumnHidden(17, true);
        ui->tableView->setColumnHidden(18, true);
        ui->tableView->setColumnHidden(19, true);
        ui->tableView->setColumnHidden(20, true);
        ui->tableView->setColumnHidden(21, true);
        ui->tableView->setColumnHidden(22, true);
        QHeaderView* header = ui->tableView->horizontalHeader();
        header->setSectionResizeMode(QHeaderView::Interactive);

        QList<int> visibleColumns;
        for (int i = 0; i < model->columnCount(); ++i) {
            if (!ui->tableView->isColumnHidden(i)) {
                visibleColumns.append(i);
            }
        }

        for (int i = 0; i < visibleColumns.size(); ++i) {
            if (i < visibleColumns.size() - 1) {
                header->setSectionResizeMode(visibleColumns[i], QHeaderView::ResizeToContents);
            } else {
                header->setSectionResizeMode(visibleColumns[i], QHeaderView::Stretch);
            }
        }

        ui->tableView->resizeColumnsToContents();
        ui->tableView->setMinimumWidth(541);
        ui->tableView->setMaximumWidth(541);
        ui->frame->setHidden(true);
        ui->pushButton_form->setHidden(true);
        ui->pushButton_type->setHidden(true);
    }
}

void MainWindow::on_pushButton_staff_clicked()
{
    staff staff_ui;
    staff_ui.setModal(true);
    staff_ui.exec();
}

void MainWindow::on_pushButton_type_clicked()
{
    add_type_si type_si;
    type_si.setModal(true);
    type_si.exec();
    table = 1;
    set_table("SI");
}

void MainWindow::on_pushButton_form_clicked()
{
    forms ui_forms;
    ui_forms.setModal(true);
    ui_forms.exec();
}

void MainWindow::create_directory_for_eqipment(QVector <QStringList> &equipmentList, int type)
{
    QString appDirPath = QCoreApplication::applicationDirPath();
    QString filesDirPath;

    if (type == 0){
        filesDirPath = appDirPath + "/FILES/1. Средства измерения";
        QDir filesDir(filesDirPath);

        if (!filesDir.exists())
        {
            if (!filesDir.mkpath("."))
            {
                qWarning() << "Failed to create base directory:" << filesDirPath;
                return;
            }
            qDebug() << "Base directory created:" << filesDirPath;
        }

        foreach (const QStringList &equipment, equipmentList)
        {
            QString equipmentName = equipment.at(0);

            QString equipmentDirPath = filesDir.filePath(equipmentName);
            QDir equipmentDir(equipmentDirPath);

            if (!equipmentDir.exists())
            {
                if (!equipmentDir.mkpath("."))
                {
                    qWarning() << "Failed to create directory for" << equipmentName;
                    continue;
                }
                qDebug() << "Directory created:" << equipmentDirPath;
            }
            else
            {
                qDebug() << "Directory already exists:" << equipmentDirPath;
            }
        }

    } else if (type == 1) {
        filesDirPath = appDirPath + "/FILES/2. Испытательное оборудование";
        QDir filesDir(filesDirPath);
        if (!filesDir.exists())
        {
            if (!filesDir.mkpath("."))
            {
                qWarning() << "Failed to create base directory:" << filesDirPath;
                return;
            }
            qDebug() << "Base directory created:" << filesDirPath;
        }

        foreach (const QStringList &equipment, equipmentList)
        {
            QString equipmentName = equipment.at(0);

            QString equipmentDirPath = filesDir.filePath(equipmentName);
            QDir equipmentDir(equipmentDirPath);

            if (!equipmentDir.exists())
            {
                if (!equipmentDir.mkpath("."))
                {
                    qWarning() << "Failed to create directory for" << equipmentName;
                    continue;
                }
                qDebug() << "Directory created:" << equipmentDirPath;
            }
            else
            {
                qDebug() << "Directory already exists:" << equipmentDirPath;
            }
        }
    } else {
        filesDirPath = appDirPath + "/FILES/3. Вспомогательное оборудование";
        QDir filesDir(filesDirPath);

        if (!filesDir.exists())
        {
            if (!filesDir.mkpath("."))
            {
                qWarning() << "Failed to create base directory:" << filesDirPath;
                return;
            }
            qDebug() << "Base directory created:" << filesDirPath;
        }

        foreach (const QStringList &equipment, equipmentList)
        {
            QString equipmentName = equipment.at(0);

            QString equipmentDirPath = filesDir.filePath(equipmentName);
            QDir equipmentDir(equipmentDirPath);
            if (!equipmentDir.exists())
            {
                if (!equipmentDir.mkpath("."))
                {
                    qWarning() << "Failed to create directory for" << equipmentName;
                    continue;
                }
                qDebug() << "Directory created:" << equipmentDirPath;
            } else {
                qDebug() << "Directory already exists:" << equipmentDirPath;
            }
        }
    }

}

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent, f) {
    setCursor(Qt::PointingHandCursor);
}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(text().replace("\n", ""));
    }
    QLabel::mousePressEvent(event);
}

void MainWindow::on_pushButton_2_clicked()
{
    QString appDirPath = QCoreApplication::applicationDirPath();
    QString filesDirPath;
    filesDirPath = appDirPath + "/FILES";

    if (!QDir(filesDirPath).exists()) {
        qDebug() << "Папка не существует:" << filesDirPath;
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(filesDirPath));
}
