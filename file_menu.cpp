//file_menu.cpp
#include "file_menu.h"
#include "ui_file_menu.h"
#include "database.h"
#include <qt_windows.h>

#include <QtSql>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QCheckBox>

#include <QDateTime>
#include <QStandardItemModel>

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QIODevice>
#include <QDataStream>
#include <QtEndian>
#include <QFileDialog>
#include <QRandomGenerator>



const quint32 file_menu::crc32_table[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59
};


file_menu::file_menu(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::file_menu)
{
    ui->setupUi(this);
    table = 1;
    ui->radioButton_SI->setChecked(true);

    Database db2;
    db2.openDatabase("database.db");
    data_SI = db2.get_data_SI();
    data_IO = db2.get_data_IO();
    data_VO = db2.get_data_VO();
    data_metr = db2.get_data_metr();
    data_info = db2.get_data_info();
    db2.close_conn();

    model = new QStandardItemModel(this);
    for (int x = 0; x < data_SI.size(); x++){
        if (data_SI.at(x).at(10).split("@").size() <= 1){
            QString si = data_SI.at(x).at(0) + ", " + data_SI.at(x).at(1) + " " + data_SI.at(x).at(2);
            QStandardItem *item = new QStandardItem(si);
            model->appendRow(item);
        }
    }
    ui->listView_allinv->setModel(model);
    ui->listView_allinv->show();
}

file_menu::~file_menu()
{
    delete ui;
}

void file_menu::on_radioButton_SI_clicked()
{
    ui->pushButton_birki->setText("Выгрузить бирки");
    ui->pushButton_karts->setText("Выгрузить карточки");
    list_inv_nums_SI.clear();
    list_inv_nums_IO.clear();
    list_inv_nums_VO.clear();
    delete model;
    model = new QStandardItemModel(this);
    for (int x = 0; x < data_SI.size(); x++){
        if (data_SI.at(x).at(10).split("@").size() <= 1){
            QString si = data_SI.at(x).at(0) + ", " + data_SI.at(x).at(1) + " " + data_SI.at(x).at(2);
            QStandardItem *item = new QStandardItem(si);
            model->appendRow(item);
        }
    }
    ui->listView_allinv->setModel(model);
    ui->listView_allinv->show();
}

void file_menu::on_radioButton_IO_clicked()
{
    ui->pushButton_birki->setText("Выгрузить бирки");
    ui->pushButton_karts->setText("Выгрузить карточки");
    list_inv_nums_SI.clear();
    list_inv_nums_IO.clear();
    list_inv_nums_VO.clear();
    delete model;
    model = new QStandardItemModel(this);
    for (int x = 0; x < data_IO.size(); x++){
        if (data_IO.at(x).at(7).split("@").size() <= 1){
            QString si = data_IO.at(x).at(0) + ", " + data_IO.at(x).at(1);
            QStandardItem *item = new QStandardItem(si);
            model->appendRow(item);
        }
    }
    ui->listView_allinv->setModel(model);
    ui->listView_allinv->show();
}

void file_menu::on_radioButton_VO_clicked()
{
    ui->pushButton_birki->setText("Выгрузить бирки");
    ui->pushButton_karts->setText("Выгрузить карточки");
    list_inv_nums_SI.clear();
    list_inv_nums_IO.clear();
    list_inv_nums_VO.clear();
    delete model;
    model = new QStandardItemModel(this);
    for (int x = 0; x < data_VO.size(); x++){
        if (data_VO.at(x).at(6).split("@").size() <= 1){
            QString si = data_VO.at(x).at(0) + ", " + data_VO.at(x).at(1);
            QStandardItem *item = new QStandardItem(si);
            model->appendRow(item);
        }
    }
    ui->listView_allinv->setModel(model);
    ui->listView_allinv->show();
}



void file_menu::on_pushButton_birki_clicked()
{
    QStringList selectedItems;
    QModelIndexList indexes = ui->listView_allinv->selectionModel()->selectedIndexes();
    foreach(const QModelIndex &index, indexes) {
        if(index.isValid()) {
            QString text = model->data(index, Qt::DisplayRole).toString();
            selectedItems << text.split(",").at(0);
        }
    }
    if (ui->radioButton_SI->isChecked()){
        list_inv_nums_SI = selectedItems;
    } else if (ui->radioButton_IO->isChecked()) {
        list_inv_nums_IO = selectedItems;
    } else {
        list_inv_nums_VO = selectedItems;
    }

    if (!list_inv_nums_SI.isEmpty()){
        QString file_name;
        QString folderPath;
        // current_metr = getEmployeeOnPost(data_metr, QDate::currentDate());
        folderPath = QFileDialog::getExistingDirectory(
            this,
            "Выберите папку для выгрузки бирок",
            "",
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
            );
        if (folderPath != ""){
            file_name += folderPath + "/";
            file_name += "Бирки СИ " + QDate::currentDate().toString("dd.MM.yyyy") + ".docx";
            create_birks_SI_Docx(file_name);
            ui->pushButton_birki->setText("Готово");
        }
    } else if (!list_inv_nums_IO.isEmpty()) {
        QString file_name;
        QString folderPath;
        // current_metr = getEmployeeOnPost(data_metr, QDate::currentDate());
        folderPath = QFileDialog::getExistingDirectory(
            this,
            "Выберите папку для выгрузки бирок",
            "",
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
            );
        if (folderPath != ""){
            file_name += folderPath + "/";
            file_name += "Бирки ИО " + QDate::currentDate().toString("dd.MM.yyyy") + ".docx";
            create_birks_IO_Docx(file_name);
            ui->pushButton_birki->setText("Готово");
        }
    } else {
        QString file_name;
        QString folderPath;
        // current_metr = getEmployeeOnPost(data_metr, QDate::currentDate());
        folderPath = QFileDialog::getExistingDirectory(
            this,
            "Выберите папку для выгрузки бирок",
            "",
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
            );
        if (folderPath != ""){
            file_name += folderPath + "/";
            file_name += "Бирки ВО " + QDate::currentDate().toString("dd.MM.yyyy") + ".docx";
            create_birks_VO_Docx(file_name);
            ui->pushButton_birki->setText("Готово");
        }
    }
}

void file_menu::on_pushButton_karts_clicked()
{
    QStringList selectedItems;
    QModelIndexList indexes = ui->listView_allinv->selectionModel()->selectedIndexes();
    foreach(const QModelIndex &index, indexes) {
        if(index.isValid()) {
            QString text = model->data(index, Qt::DisplayRole).toString();
            selectedItems << text.split(",").at(0);
        }
    }
    if (ui->radioButton_SI->isChecked()){
        list_inv_nums_SI = selectedItems;
    } else if (ui->radioButton_IO->isChecked()) {
        list_inv_nums_IO = selectedItems;
    } else {
        list_inv_nums_VO = selectedItems;
    }

    if (!list_inv_nums_SI.isEmpty()){
        QString file_name;
        QString folderPath;
        // current_metr = getEmployeeOnPost(data_metr, QDate::currentDate());
        folderPath = QFileDialog::getExistingDirectory(
            this,
            "Выберите папку для выгрузки бирок",
            "",
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
            );
        if (folderPath != ""){
            file_name += folderPath + "/";
            for (int z = 0; z < list_inv_nums_SI.size(); z++){
                for (int z1 = 0; z1 < data_SI.size(); z1++){
                    QString inv = list_inv_nums_SI.at(z);
                    QString inv_in_base = data_SI.at(z1).at(0);
                    if (inv == inv_in_base){
                        cur_data_SI = data_SI.at(z1);
                        break;
                    }
                }
                QString full_folderPath = file_name + "Карточка " + cur_data_SI.at(0) + ".docx";
                create_kart_SI_Docx(full_folderPath);
            }
            ui->pushButton_karts->setText("Готово");
        }
    } else if (!list_inv_nums_IO.isEmpty()) {
        QString file_name;
        QString folderPath;
        // current_metr = getEmployeeOnPost(data_metr, QDate::currentDate());
        folderPath = QFileDialog::getExistingDirectory(
            this,
            "Выберите папку для выгрузки бирок",
            "",
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
            );
        if (folderPath != ""){
            file_name += folderPath + "/";
            for (int z = 0; z < list_inv_nums_IO.size(); z++){
                for (int z1 = 0; z1 < data_IO.size(); z1++){
                    QString inv = list_inv_nums_IO.at(z);
                    QString inv_in_base = data_IO.at(z1).at(0);
                    if (inv == inv_in_base){
                        cur_data_IO = data_IO.at(z1);
                        break;
                    }
                }
                QString full_folderPath = file_name + "Карточка " + cur_data_IO.at(0) + ".docx";
                create_kart_IO_Docx(full_folderPath);
            }
            ui->pushButton_karts->setText("Готово");
        }
    } else {
        QString file_name;
        QString folderPath;
        // current_metr = getEmployeeOnPost(data_metr, QDate::currentDate());
        folderPath = QFileDialog::getExistingDirectory(
            this,
            "Выберите папку для выгрузки бирок",
            "",
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
            );
        if (folderPath != ""){
            file_name += folderPath + "/";
            for (int z = 0; z < list_inv_nums_VO.size(); z++){
                for (int z1 = 0; z1 < data_VO.size(); z1++){
                    QString inv = list_inv_nums_VO.at(z);
                    QString inv_in_base = data_VO.at(z1).at(0);
                    if (inv == inv_in_base){
                        cur_data_VO = data_VO.at(z1);
                        break;
                    }
                }
                QString full_folderPath = file_name + "Карточка " + cur_data_VO.at(0) + ".docx";
                create_kart_VO_Docx(full_folderPath);
            }
            ui->pushButton_karts->setText("Готово");
        }
    }
}

void file_menu::create_birks_SI_Docx(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return;

    QDataStream zip(&file);
    zip.setByteOrder(QDataStream::LittleEndian);

    // 1. Основные файлы документа
    QList<QPair<QString, QByteArray>> files = {
        // [Content_Types].xml
        {
            "[Content_Types].xml",
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
            "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
            "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
            "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
            "<Override PartName=\"/word/document.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>"
            "<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>"
            "</Types>"
        },

        // _rels/.rels
        {
            "_rels/.rels",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"word/document.xml\"/>"
            "<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/>"
            "</Relationships>"
        },

        // word/document.xml
        {
            "word/document.xml",
            [this]() {
                QString documentContent = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                                          "<w:document xmlns:wpc=\"http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas\" "
                                          "xmlns:cx=\"http://schemas.microsoft.com/office/drawing/2014/chartex\" "
                                          "xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" "
                                          "xmlns:o=\"urn:schemas-microsoft-com:office:office\" "
                                          "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" "
                                          "xmlns:m=\"http://schemas.openxmlformats.org/officeDocument/2006/math\" "
                                          "xmlns:v=\"urn:schemas-microsoft-com:vml\" "
                                          "xmlns:wp14=\"http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing\" "
                                          "xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\" "
                                          "xmlns:w10=\"urn:schemas-microsoft-com:office:word\" "
                                          "xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" "
                                          "xmlns:w14=\"http://schemas.microsoft.com/office/word/2010/wordml\" "
                                          "xmlns:w15=\"http://schemas.microsoft.com/office/word/2012/wordml\" "
                                          "xmlns:w16se=\"http://schemas.microsoft.com/office/word/2015/wordml/symex\" "
                                          "xmlns:wpg=\"http://schemas.microsoft.com/office/word/2010/wordprocessingGroup\" "
                                          "xmlns:wpi=\"http://schemas.microsoft.com/office/word/2010/wordprocessingInk\" "
                                          "xmlns:wne=\"http://schemas.microsoft.com/office/word/2006/wordml\" "
                                          "xmlns:wps=\"http://schemas.microsoft.com/office/word/2010/wordprocessingShape\" "
                                          "mc:Ignorable=\"w14 w15 w16se wp14\">"
                                          "<w:body>";

                documentContent +=
                    "<w:tbl>"

                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblBorders>"
                    "<w:top w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:left w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:bottom w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:right w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideH w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideV w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "</w:tblBorders>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"5000\"/>"
                    "<w:gridCol w:w=\"5000\"/>"
                    "</w:tblGrid>";

                    for (int x = 0; x < data_SI.size(); x++){
                        QStringList cur_data_SI = data_SI.at(x);

                        for (int z = 0; z < list_inv_nums_SI.size(); z++){
                            QString inv = cur_data_SI.at(0);
                            QString inv_in_list = list_inv_nums_SI.at(z);

                            if (inv == inv_in_list){
                                Database db2;
                                db2.openDatabase("database.db");
                                SI_dop_info = db2.get_data_BirksKoeft(inv);
                                db2.close_conn();

                                QString pov = cur_data_SI.at(12);
                                QStringList pov_last_split;
                                if (pov == "-"){
                                    pov_last_split.append("–");
                                    pov_last_split.append("–");
                                    pov_last_split.append("–");
                                    pov_last_split.append("–");
                                } else {
                                    QStringList pov_split = pov.split("@");
                                    QString pov_last = pov_split.last();
                                    pov_last_split = pov_last.split(";");
                                }
                                QDate date_att = QDate::fromString(pov_last_split.at(0), "dd.MM.yyyy");
                                // Написать логику для ситуации, когда ввели оборудование позже даты последней поверки
                                QString metrolog = getEmployeeOnPost(data_metr, date_att);
                                QString properties = "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/>";
                                QString dop_info;
                                if (SI_dop_info != ""){
                                    dop_info = "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr><w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + SI_dop_info + "]]></w:t></w:r></w:p>";
                                } else {
                                    dop_info = "";
                                }

                                QString PO_SI = "";
                                if (cur_data_SI.at(7) == "-" || cur_data_SI.at(7) == "отсутствует" || cur_data_SI.at(7) == ""){
                                    PO_SI = "";
                                } else {
                                    PO_SI = "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                            "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>ПО&#xA0;</w:t></w:r>"
                                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_SI.at(7) + "]]></w:t></w:r>"
                                            "</w:p>";
                                }

                                documentContent +=
                                    "<w:tr>"
                                    "<w:tc><w:tcPr><w:tcW w:w=\"5000\" w:type=\"dxa\"/></w:tcPr>"
                                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Тип оборудования&#xA0;</w:t></w:r>"
                                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>СИ</w:t></w:r>"
                                        "</w:p>"

                                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_SI.at(1) + " " + cur_data_SI.at(2) + "]]></w:t></w:r>"
                                        "</w:p>" + dop_info + PO_SI +

                                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Зав №&#xA0;</w:t></w:r>"
                                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_data_SI.at(4) + "</w:t></w:r>"
                                        "</w:p>"

                                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Инв №&#xA0;</w:t></w:r>"
                                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_data_SI.at(0) + "</w:t></w:r>"
                                        "</w:p>"

                                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Дата текущей поверки&#xA0;</w:t></w:r>"
                                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + pov_last_split.at(0) + "</w:t></w:r>"
                                        "</w:p>"

                                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Дата следующей поверки&#xA0;</w:t></w:r>"
                                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + pov_last_split.at(3) + "</w:t></w:r>"
                                        "</w:p>"

                                    // "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    //     "<w:r><w:rPr>" + properties + "</w:rPr><w:t>&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;</w:t></w:r>"
                                    //     "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + metrolog + "</w:t></w:r>"
                                    //     "</w:p>"
                                    "</w:tc>"


                                    "<w:tc>"
                                    "<w:tcPr><w:tcW w:w=\"5000\" w:type=\"dxa\"/></w:tcPr>"
                                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"20\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Условия эксплуатации</w:t></w:r>"
                                        "</w:p>"

                                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_SI.at(1) + " " + cur_data_SI.at(2) + "]]></w:t></w:r>"
                                        "</w:p>"

                                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Температура окружающей среды, ℃:&#xA0;</w:t></w:r>"
                                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_SI.at(14) + "]]></w:t></w:r>"
                                        "</w:p>"

                                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Атмосферное давление, кПа:&#xA0;</w:t></w:r>"
                                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_SI.at(16) + "]]></w:t></w:r>"
                                        "</w:p>"

                                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Относительная влажность воздуха, %:&#xA0;</w:t></w:r>"
                                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_SI.at(15) + "]]></w:t></w:r>"
                                        "</w:p>"

                                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Напряжение электросети, В:&#xA0;</w:t></w:r>"
                                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_SI.at(17) + "]]></w:t></w:r>"
                                        "</w:p>"

                                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Частота питающей сети, Гц:&#xA0;</w:t></w:r>"
                                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_SI.at(18) + "]]></w:t></w:r>"
                                        "</w:p>"
                                    "</w:tc>"
                                    "</w:tr>";
                            }
                        }
                    }

                documentContent +=
                    "</w:tbl>"
                    "<w:sectPr>"
                    "<w:pgSz w:w=\"11906\" w:h=\"16838\"/>"
                    "<w:pgMar w:top=\"1134\" w:right=\"850\" w:bottom=\"1134\" w:left=\"1701\" w:header=\"708\" w:footer=\"708\" w:gutter=\"0\"/>"
                    "<w:cols w:space=\"720\"/>"
                    "<w:docGrid w:linePitch=\"360\"/>"
                    "</w:sectPr>"
                    "</w:body>"
                    "</w:document>";

                qDebug() << "Done";
                return documentContent.toUtf8();
            }()
        },

        // word/_rels/document.xml.rels
        {
            "word/_rels/document.xml.rels",
            [this]() {
                QString content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                  "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">";
                content += "</Relationships>";
                return content.toUtf8();
            }()
        },

        // docProps/core.xml
        {
            "docProps/core.xml",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" "
            "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "
            "xmlns:dcterms=\"http://purl.org/dc/terms/\" "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
            "<dc:creator>LENOVO</dc:creator>"
            "<dcterms:created xsi:type=\"dcterms:W3CDTF\">"
                + QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8() +
                "</dcterms:created>"
                "</cp:coreProperties>"
        }
    };

    QList<quint32> offsets;
    for (const auto &file : files) {
        quint32 offset = write_local_file(zip, file.first, file.second);
        offsets.append(offset);
    }

    write_central_directory(zip, files, offsets);
    file.close();
}

void file_menu::create_birks_IO_Docx(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return;

    QDataStream zip(&file);
    zip.setByteOrder(QDataStream::LittleEndian);

    // 1. Основные файлы документа
    QList<QPair<QString, QByteArray>> files = {
        // [Content_Types].xml
        {
            "[Content_Types].xml",
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
            "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
            "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
            "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
            "<Override PartName=\"/word/document.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>"
            "<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>"
            "</Types>"
        },

        // _rels/.rels
        {
            "_rels/.rels",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"word/document.xml\"/>"
            "<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/>"
            "</Relationships>"
        },

        // word/document.xml
        {
            "word/document.xml",
            [this]() {
                QString documentContent = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                                          "<w:document xmlns:wpc=\"http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas\" "
                                          "xmlns:cx=\"http://schemas.microsoft.com/office/drawing/2014/chartex\" "
                                          "xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" "
                                          "xmlns:o=\"urn:schemas-microsoft-com:office:office\" "
                                          "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" "
                                          "xmlns:m=\"http://schemas.openxmlformats.org/officeDocument/2006/math\" "
                                          "xmlns:v=\"urn:schemas-microsoft-com:vml\" "
                                          "xmlns:wp14=\"http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing\" "
                                          "xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\" "
                                          "xmlns:w10=\"urn:schemas-microsoft-com:office:word\" "
                                          "xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" "
                                          "xmlns:w14=\"http://schemas.microsoft.com/office/word/2010/wordml\" "
                                          "xmlns:w15=\"http://schemas.microsoft.com/office/word/2012/wordml\" "
                                          "xmlns:w16se=\"http://schemas.microsoft.com/office/word/2015/wordml/symex\" "
                                          "xmlns:wpg=\"http://schemas.microsoft.com/office/word/2010/wordprocessingGroup\" "
                                          "xmlns:wpi=\"http://schemas.microsoft.com/office/word/2010/wordprocessingInk\" "
                                          "xmlns:wne=\"http://schemas.microsoft.com/office/word/2006/wordml\" "
                                          "xmlns:wps=\"http://schemas.microsoft.com/office/word/2010/wordprocessingShape\" "
                                          "mc:Ignorable=\"w14 w15 w16se wp14\">"
                                          "<w:body>";

                documentContent +=
                    "<w:tbl>"

                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblBorders>"
                    "<w:top w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:left w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:bottom w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:right w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideH w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideV w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "</w:tblBorders>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"5000\"/>"
                    "<w:gridCol w:w=\"5000\"/>"
                    "</w:tblGrid>";

                for (int x = 0; x < data_IO.size(); x++){
                    QStringList cur_data_IO = data_IO.at(x);

                    for (int z = 0; z < list_inv_nums_IO.size(); z++){
                        QString inv = cur_data_IO.at(0);
                        QString inv_in_list = list_inv_nums_IO.at(z);

                        if (inv == inv_in_list){
                            QString att = cur_data_IO.at(9);
                            QStringList att_last_split;
                            QStringList att_split = att.split("@");
                            QString att_last = att_split.takeLast();
                            att_last_split = att_last.split(";");

                            QDate date_att = QDate::fromString(att_last_split.at(0), "dd.MM.yyyy");
                            QString metrolog = getEmployeeOnPost(data_metr, date_att);
                            QString properties = "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/>";

                            documentContent +=
                                "<w:tr>"
                                "<w:tc><w:tcPr><w:tcW w:w=\"5000\" w:type=\"dxa\"/></w:tcPr>"
                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Тип оборудования&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t>ИО</w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_data_IO.at(2) + "</w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Зав №&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_data_IO.at(3) + "</w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Инв №&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_data_IO.at(0)+ "</w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Дата текущей аттестации&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + att_last_split.at(0) + "</w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Дата следующей аттестации&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + att_last_split.at(3) + "</w:t></w:r>"
                                    "</w:p>"

                                // "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                //     "<w:r><w:rPr>" + properties + "</w:rPr><w:t>&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;</w:t></w:r>"
                                //     "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + metrolog + "</w:t></w:r>"
                                //     "</w:p>"
                                "</w:tc>"


                                "<w:tc>"
                                "<w:tcPr><w:tcW w:w=\"5000\" w:type=\"dxa\"/></w:tcPr>"
                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"20\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Условия эксплуатации</w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_IO.at(2) + "]]></w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Температура окружающей среды, ℃:&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_IO.at(13) + "]]></w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Атмосферное давление, кПа:&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_IO.at(15) + "]]></w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Относительная влажность воздуха, %:&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_IO.at(14) + "]]></w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Напряжение электросети, В:&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_IO.at(16) + "]]></w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Частота питающей сети, Гц:&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_IO.at(17) + "]]></w:t></w:r>"
                                    "</w:p>"
                                "</w:tc>"
                                "</w:tr>";
                        }
                    }
                }

                documentContent +=
                    "</w:tbl>"
                    "<w:sectPr>"
                    "<w:pgSz w:w=\"11906\" w:h=\"16838\"/>"
                    "<w:pgMar w:top=\"1134\" w:right=\"850\" w:bottom=\"1134\" w:left=\"1701\" w:header=\"708\" w:footer=\"708\" w:gutter=\"0\"/>"
                    "<w:cols w:space=\"720\"/>"
                    "<w:docGrid w:linePitch=\"360\"/>"
                    "</w:sectPr>"
                    "</w:body>"
                    "</w:document>";

                qDebug() << "Done";
                return documentContent.toUtf8();
            }()
        },

        // word/_rels/document.xml.rels
        {
            "word/_rels/document.xml.rels",
            [this]() {
                QString content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                  "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">";
                content += "</Relationships>";
                return content.toUtf8();
            }()
        },

        // docProps/core.xml
        {
            "docProps/core.xml",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" "
            "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "
            "xmlns:dcterms=\"http://purl.org/dc/terms/\" "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
            "<dc:creator>LENOVO</dc:creator>"
            "<dcterms:created xsi:type=\"dcterms:W3CDTF\">"
                + QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8() +
                "</dcterms:created>"
                "</cp:coreProperties>"
        }
    };

    QList<quint32> offsets;
    for (const auto &file : files) {
        quint32 offset = write_local_file(zip, file.first, file.second);
        offsets.append(offset);
    }

    write_central_directory(zip, files, offsets);
    file.close();
}

void file_menu::create_birks_VO_Docx(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return;

    QDataStream zip(&file);
    zip.setByteOrder(QDataStream::LittleEndian);

    // 1. Основные файлы документа
    QList<QPair<QString, QByteArray>> files = {
        // [Content_Types].xml
        {
            "[Content_Types].xml",
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
            "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
            "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
            "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
            "<Override PartName=\"/word/document.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>"
            "<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>"
            "</Types>"
        },

        // _rels/.rels
        {
            "_rels/.rels",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"word/document.xml\"/>"
            "<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/>"
            "</Relationships>"
        },

        // word/document.xml
        {
            "word/document.xml",
            [this]() {
                QString documentContent = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                                          "<w:document xmlns:wpc=\"http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas\" "
                                          "xmlns:cx=\"http://schemas.microsoft.com/office/drawing/2014/chartex\" "
                                          "xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" "
                                          "xmlns:o=\"urn:schemas-microsoft-com:office:office\" "
                                          "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" "
                                          "xmlns:m=\"http://schemas.openxmlformats.org/officeDocument/2006/math\" "
                                          "xmlns:v=\"urn:schemas-microsoft-com:vml\" "
                                          "xmlns:wp14=\"http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing\" "
                                          "xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\" "
                                          "xmlns:w10=\"urn:schemas-microsoft-com:office:word\" "
                                          "xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" "
                                          "xmlns:w14=\"http://schemas.microsoft.com/office/word/2010/wordml\" "
                                          "xmlns:w15=\"http://schemas.microsoft.com/office/word/2012/wordml\" "
                                          "xmlns:w16se=\"http://schemas.microsoft.com/office/word/2015/wordml/symex\" "
                                          "xmlns:wpg=\"http://schemas.microsoft.com/office/word/2010/wordprocessingGroup\" "
                                          "xmlns:wpi=\"http://schemas.microsoft.com/office/word/2010/wordprocessingInk\" "
                                          "xmlns:wne=\"http://schemas.microsoft.com/office/word/2006/wordml\" "
                                          "xmlns:wps=\"http://schemas.microsoft.com/office/word/2010/wordprocessingShape\" "
                                          "mc:Ignorable=\"w14 w15 w16se wp14\">"
                                          "<w:body>";

                documentContent +=
                    "<w:tbl>"

                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblBorders>"
                    "<w:top w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:left w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:bottom w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:right w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideH w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideV w:val=\"double\" w:sz=\"16\" w:space=\"0\" w:color=\"000000\"/>"
                    "</w:tblBorders>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"5000\"/>"
                    "<w:gridCol w:w=\"5000\"/>"
                    "</w:tblGrid>";

                for (int x = 0; x < data_VO.size(); x++){
                    QStringList cur_data_VO = data_VO.at(x);

                    for (int z = 0; z < list_inv_nums_VO.size(); z++){
                        QString inv = cur_data_VO.at(0);
                        QString inv_in_list = list_inv_nums_VO.at(z);
                        if (inv == inv_in_list){
                            QString PO_VO = "";
                            QString properties = "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/>";

                            if (cur_data_VO.at(8) == "-" || cur_data_VO.at(8) == "отсутствует" || cur_data_VO.at(8) == ""){
                                PO_VO = "";
                            } else {
                                PO_VO = "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                        "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>ПО&#xA0;</w:t></w:r>"
                                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_VO.at(8) + "]]></w:t></w:r>"
                                        "</w:p>";
                            }
                            documentContent +=
                                "<w:tr>"
                                "<w:tc><w:tcPr><w:tcW w:w=\"5000\" w:type=\"dxa\"/></w:tcPr>"
                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Тип оборудования&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t>ВО</w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_data_VO.at(1) + "</w:t></w:r>"
                                    "</w:p>" + PO_VO +

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Зав №&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_data_VO.at(2) + "</w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Инв №&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_data_VO.at(0) + "</w:t></w:r>"
                                    "</w:p>"

                                // "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                //     "<w:r><w:rPr>" + properties + "</w:rPr><w:t>&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;&#xA0;</w:t></w:r>"
                                //     "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + metrolog + "</w:t></w:r>"
                                    // "</w:p>"
                                "</w:tc>"


                                "<w:tc>"
                                "<w:tcPr><w:tcW w:w=\"5000\" w:type=\"dxa\"/></w:tcPr>"
                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"20\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Условия эксплуатации</w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_VO.at(1) + "]]></w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Температура окружающей среды, ℃:&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_VO.at(9) + "]]></w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Атмосферное давление, кПа:&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_VO.at(11) + "]]></w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Относительная влажность воздуха, %:&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_VO.at(10) + "]]></w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Напряжение электросети, В:&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_VO.at(12) + "]]></w:t></w:r>"
                                    "</w:p>"

                                "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                    "<w:r><w:rPr>" + properties + "<w:b/></w:rPr><w:t>Частота питающей сети, Гц:&#xA0;</w:t></w:r>"
                                    "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + cur_data_VO.at(13) + "]]></w:t></w:r>"
                                    "</w:p>"
                                    "</w:tc>"
                                "</w:tr>";
                        }
                    }
                }

                documentContent +=
                    "</w:tbl>"
                    "<w:sectPr>"
                    "<w:pgSz w:w=\"11906\" w:h=\"16838\"/>"
                    "<w:pgMar w:top=\"1134\" w:right=\"850\" w:bottom=\"1134\" w:left=\"1701\" w:header=\"708\" w:footer=\"708\" w:gutter=\"0\"/>"
                    "<w:cols w:space=\"720\"/>"
                    "<w:docGrid w:linePitch=\"360\"/>"
                    "</w:sectPr>"
                    "</w:body>"
                    "</w:document>";

                qDebug() << "Done";
                return documentContent.toUtf8();
            }()
        },

        // word/_rels/document.xml.rels
        {
            "word/_rels/document.xml.rels",
            [this]() {
                QString content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                  "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">";
                content += "</Relationships>";
                return content.toUtf8();
            }()
        },

        // docProps/core.xml
        {
            "docProps/core.xml",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" "
            "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "
            "xmlns:dcterms=\"http://purl.org/dc/terms/\" "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
            "<dc:creator>LENOVO</dc:creator>"
            "<dcterms:created xsi:type=\"dcterms:W3CDTF\">"
                + QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8() +
                "</dcterms:created>"
                "</cp:coreProperties>"
        }
    };

    QList<quint32> offsets;
    for (const auto &file : files) {
        quint32 offset = write_local_file(zip, file.first, file.second);
        offsets.append(offset);
    }

    write_central_directory(zip, files, offsets);
    file.close();
}

void file_menu::create_kart_SI_Docx(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return;

    QDataStream zip(&file);
    zip.setByteOrder(QDataStream::LittleEndian);

    // 1. Основные файлы документа
    QList<QPair<QString, QByteArray>> files = {
        // [Content_Types].xml
        {
            "[Content_Types].xml",
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
            "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
            "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
            "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
            "<Override PartName=\"/word/document.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>"
            "<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>"
            "</Types>"
        },

        // _rels/.rels
        {
            "_rels/.rels",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"word/document.xml\"/>"
            "<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/>"
            "</Relationships>"
        },

        // word/document.xml
        {
            "word/document.xml",
            [this]() {
                QString documentContent = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                                          "<w:document xmlns:wpc=\"http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas\" "
                                          "xmlns:cx=\"http://schemas.microsoft.com/office/drawing/2014/chartex\" "
                                          "xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" "
                                          "xmlns:o=\"urn:schemas-microsoft-com:office:office\" "
                                          "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" "
                                          "xmlns:m=\"http://schemas.openxmlformats.org/officeDocument/2006/math\" "
                                          "xmlns:v=\"urn:schemas-microsoft-com:vml\" "
                                          "xmlns:wp14=\"http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing\" "
                                          "xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\" "
                                          "xmlns:w10=\"urn:schemas-microsoft-com:office:word\" "
                                          "xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" "
                                          "xmlns:w14=\"http://schemas.microsoft.com/office/word/2010/wordml\" "
                                          "xmlns:w15=\"http://schemas.microsoft.com/office/word/2012/wordml\" "
                                          "xmlns:w16se=\"http://schemas.microsoft.com/office/word/2015/wordml/symex\" "
                                          "xmlns:wpg=\"http://schemas.microsoft.com/office/word/2010/wordprocessingGroup\" "
                                          "xmlns:wpi=\"http://schemas.microsoft.com/office/word/2010/wordprocessingInk\" "
                                          "xmlns:wne=\"http://schemas.microsoft.com/office/word/2006/wordml\" "
                                          "xmlns:wps=\"http://schemas.microsoft.com/office/word/2010/wordprocessingShape\" "
                                          "mc:Ignorable=\"w14 w15 w16se wp14\">"
                                          "<w:body>";
                QString full_lab_name = data_info.at(0);
                QString PO = cur_data_SI.at(7);
                QString year = cur_data_SI.at(10);
                year = year.split(".").at(2);
                QString metrolog = getEmployeeOnPost(data_metr, QDate::currentDate());
                QStringList izg = cur_data_SI.at(8).split("@");
                QString properties = "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/>";

                if (PO == "отсутствует"){
                    PO = "";
                }
                documentContent +=
                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Испытательная лаборатория Общества с ограниченной ответственностью</w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + full_lab_name.replace("ИЛ ", "").replace("ПБ ", "") + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[(" + data_info.at(0) + ")]]></w:t>"
                    "</w:r>"
                    "</w:p>"


                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/><w:b/></w:rPr>"
                    "<w:t>КАРТОЧКА К ОБОРУДОВАНИЮ</w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + cur_data_SI.at(1) + " " + cur_data_SI.at(2) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + "рег. № " + cur_data_SI.at(3) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/><w:b/></w:rPr>"
                    "<w:t>Сведения об оборудовании:</w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Тип оборудования СИ</w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Идентификация программного обеспечения (при наличии)&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + PO + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Изготовитель (страна, наименование организации, год выпуска)&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + izg.at(0) + ", " + izg.at(1) + ", " + izg.at(2) +"]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Дата (год) ввода в эксплуатацию&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + year + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Заводской №&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + cur_data_SI.at(4) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Инвентарный №&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + cur_data_SI.at(0) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Местонахождение&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + data_info.at(1) + ", помещение " + cur_data_SI.at(5) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>№ инструкции по охране труда и проведении испытаний на оборудовании&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + "паспорт" + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Ответственное лицо за оборудование&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + metrolog + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"14\"/><w:szCs w:val=\"14\"/><w:b/></w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/><w:b/></w:rPr>"
                    "<w:t><![CDATA[Результаты государственной поверки/аттестации:]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:tbl>"
                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblBorders>"
                    "<w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideH w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideV w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "</w:tblBorders>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"   

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"1764\"/>"
                    "<w:gridCol w:w=\"1939\"/>"
                    "<w:gridCol w:w=\"1866\"/>"
                    "<w:gridCol w:w=\"1998\"/>"
                    "<w:gridCol w:w=\"1778\"/>" //9345
                    "</w:tblGrid>"
                    
                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1764\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Дата поверки/ аттестации</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1939\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Метрологическая организация</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1866\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>№ свидетельства/ аттестата</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1998\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Результат поверки/ аттестации</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1778\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Дата следующей поверки/ аттестации</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"
                    "</w:tr>";

                QString pov = cur_data_SI.at(12);
                if (pov == "-"){
                    documentContent +=
                        "<w:tr>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1764\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t>-</w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1939\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t>-</w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1866\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t>-</w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1998\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t>пригодно</w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1778\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t>бессрочно</w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"
                        "</w:tr>";
                } else {
                    QStringList cur_pov_split;
                    QStringList pov_split = pov.split("@");
                    int pov_split_size = pov_split.size();
                    for (int z = 0; z < pov_split_size; z++) {
                        QString pov_first = pov_split.takeFirst();
                        cur_pov_split = pov_first.split(";");
                        documentContent +=
                            "<w:tr>"
                            "<w:tc><w:tcPr><w:tcW w:w=\"1764\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                            "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_pov_split.at(0) + "</w:t></w:r>"
                                "</w:p>"
                            "</w:tc>"

                            "<w:tc>"
                            "<w:tcPr><w:tcW w:w=\"1939\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                            "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_pov_split.at(1) + "</w:t></w:r>"
                                "</w:p>"
                            "</w:tc>"

                            "<w:tc>"
                            "<w:tcPr><w:tcW w:w=\"1866\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                            "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_pov_split.at(2) + "</w:t></w:r>"
                                "</w:p>"
                            "</w:tc>"

                            "<w:tc>"
                            "<w:tcPr><w:tcW w:w=\"1998\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                            "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                "<w:r><w:rPr>" + properties + "</w:rPr><w:t>пригодно</w:t></w:r>"
                                "</w:p>"
                            "</w:tc>"

                            "<w:tc>"
                            "<w:tcPr><w:tcW w:w=\"1778\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                            "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_pov_split.at(3) + "</w:t></w:r>"
                                "</w:p>"
                            "</w:tc>"
                            "</w:tr>";
                    }
                }
                    // QDate date_att = QDate::fromString(pov_last_split.at(0), "dd.MM.yyyy");
                    // Написать логику для ситуации, когда ввели оборудование позже даты последней поверки

                QDate date_vvod = QDate::fromString(cur_data_SI.at(10), "dd.MM.yyyy");
                int TO_period = cur_data_SI.at(19).toInt();
                int clean_period = cur_data_SI.at(20).toInt();
                QVector<QStringList> dates_TO_rem_clean;

                QVector<QDate> date_TO_list;
                date_TO_list.append(date_vvod);
                int added_days = 0;
                if (TO_period != 0){
                    while (date_TO_list.last() < QDate::currentDate().addMonths(-1 * TO_period)) {
                        QDate new_date = date_TO_list.last().addMonths(TO_period);
                        new_date = new_date.addDays(-1 * added_days);
                        added_days = 0;
                        while (new_date.dayOfWeek() == 6 || new_date.dayOfWeek() == 7){
                            added_days += 1;
                            new_date = new_date.addDays(1);
                        }
                        qDebug() << date_TO_list;
                        date_TO_list.append(new_date);
                    }
                }
                date_TO_list.takeFirst();

                QVector<QDate> date_clean_list;
                date_clean_list.append(date_vvod);
                if (clean_period != 0){
                    added_days = 0;
                    while (date_clean_list.last() < QDate::currentDate().addMonths(-1 * clean_period)) {
                        QDate new_date = date_clean_list.last().addMonths(clean_period);
                        new_date = new_date.addDays(-1 * added_days);
                        added_days = 0;
                        while (new_date.dayOfWeek() == 6 || new_date.dayOfWeek() == 7){
                            added_days += 1;
                            new_date = new_date.addDays(1);
                        }
                        date_clean_list.append(new_date);
                    }
                }
                date_clean_list.takeFirst();


                for (int x = 0; x < date_TO_list.size(); x++) {
                    QString current_metrolog = getEmployeeOnPost(data_metr, date_TO_list.at(x));
                    QString date_to = date_TO_list.at(x).toString("dd.MM.yyyy");
                    QStringList one_TO;
                    one_TO << date_to << "не выявлено" << "техническое обслуживание" << "1 день" << current_metrolog;
                    dates_TO_rem_clean.append(one_TO);
                }

                for (int x = 0; x < date_clean_list.size(); x++) {
                    QString current_metrolog = getEmployeeOnPost(data_metr, date_clean_list.at(x));
                    QString date_clean = date_clean_list.at(x).toString("dd.MM.yyyy");
                    QStringList one_clean;
                    one_clean << date_clean << "не выявлено" << "очистка" << "1 день" << current_metrolog;
                    dates_TO_rem_clean.append(one_clean);
                }

                QString unique_rem = cur_data_SI.at(21);
                QStringList rem_list = unique_rem.split("@");
                for (int t = 0; t < rem_list.size(); t++) {
                    QString one_rem = rem_list.at(t);
                    QStringList one_rem_List = one_rem.split(";");
                    if (QDate::fromString(one_rem_List.at(0), "dd.MM.yyyy").isValid()){
                        dates_TO_rem_clean.append(one_rem_List);
                    }
                }

                if (dates_TO_rem_clean.size() >= 2){
                    for (int i = 0; i < dates_TO_rem_clean.size() - 1; ++i) {
                        for (int j = 0; j < dates_TO_rem_clean.size() - i - 1; ++j) {
                            QDate dateA = QDate::fromString(dates_TO_rem_clean[j][0], "dd.MM.yyyy");
                            QDate dateB = QDate::fromString(dates_TO_rem_clean[j + 1][0], "dd.MM.yyyy");
                            if (dateA > dateB) {
                                dates_TO_rem_clean.swapItemsAt(j, j + 1);
                            }
                        }
                    }
                }
                qDebug() << dates_TO_rem_clean;


                documentContent +=
                    "</w:tbl>"
                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"14\"/><w:szCs w:val=\"14\"/><w:b/></w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/><w:b/></w:rPr>"
                    "<w:t><![CDATA[Регистрация повреждений, неисправностей, модификации, ремонта, ТО:]]></w:t>"
                    "</w:r>"
                    "</w:p>"
                    
                    "<w:tbl>"
                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblBorders>"
                    "<w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideH w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideV w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "</w:tblBorders>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"   

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"1000\"/>"
                    "<w:gridCol w:w=\"3141\"/>"
                    "<w:gridCol w:w=\"1724\"/>"
                    "<w:gridCol w:w=\"1588\"/>"
                    "<w:gridCol w:w=\"1892\"/>" //9345
                    "</w:tblGrid>"
                    
                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1000\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Дата</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"3141\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Выявленное несоответствие (повреждение, неисправность и др.)</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1724\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Мероприятие</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1588\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Сроки</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1892\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>ФИО исполнителя</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"
                    "</w:tr>";
                
                for (int x = 0; x < dates_TO_rem_clean.size(); x++) {
                    documentContent +=
                        "<w:tr>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1000\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(0) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"3141\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(1) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1724\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(2) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1588\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(3) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1892\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(4) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"
                        "</w:tr>";
                }


                documentContent +=
                    "</w:tbl>"
                    "<w:sectPr>"
                    "<w:pgSz w:w=\"11906\" w:h=\"16838\"/>"
                    "<w:pgMar w:top=\"1134\" w:right=\"850\" w:bottom=\"1134\" w:left=\"1701\" w:header=\"708\" w:footer=\"708\" w:gutter=\"0\"/>"
                    "<w:cols w:space=\"708\"/>"
                    "<w:docGrid w:linePitch=\"360\"/>"
                    "</w:sectPr>"
                    "</w:body>"
                    "</w:document>";

                qDebug() << "Done";
                if (!dates_TO_rem_clean.isEmpty()){
                    last_modified = QDate::fromString(dates_TO_rem_clean.last().at(0), "dd.MM.yyyy");
                } else {
                    last_modified = QDate::fromString(cur_data_SI.at(10), "dd.MM.yyyy");
                }
                return documentContent.toUtf8();
            }()
        },

        // word/_rels/document.xml.rels
        {
            "word/_rels/document.xml.rels",
            [this]() {
                QString content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                  "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">";
                content += "</Relationships>";
                return content.toUtf8();
            }()
        },

        // docProps/core.xml
        {
            "docProps/core.xml",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" "
            "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "
            "xmlns:dcterms=\"http://purl.org/dc/terms/\" "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
            "<dc:creator>LENOVO</dc:creator>"
            "<dcterms:created xsi:type=\"dcterms:W3CDTF\">"
            + addRandomTimeToDate(QDate::fromString(cur_data_SI.at(10), "dd.MM.yyyy")).toString(Qt::ISODate).toUtf8() +
            "</dcterms:created>"
            "</cp:coreProperties>"
        }
    };

    QList<quint32> offsets;
    for (const auto &file : files) {
        quint32 offset = write_local_file(zip, file.first, file.second);
        offsets.append(offset);
    }

    write_central_directory(zip, files, offsets);
    file.close();
    QDateTime created = addRandomTimeToDate(QDate::fromString(cur_data_SI.at(10), "dd.MM.yyyy"));
    QDateTime modified = addRandomTimeToDate(last_modified);

    setFileTimes(fileName, created, modified);
}

void file_menu::create_kart_IO_Docx(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return;

    QDataStream zip(&file);
    zip.setByteOrder(QDataStream::LittleEndian);

    // 1. Основные файлы документа
    QList<QPair<QString, QByteArray>> files = {
        // [Content_Types].xml
        {
            "[Content_Types].xml",
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
            "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
            "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
            "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
            "<Override PartName=\"/word/document.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>"
            "<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>"
            "</Types>"
        },

        // _rels/.rels
        {
            "_rels/.rels",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"word/document.xml\"/>"
            "<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/>"
            "</Relationships>"
        },

        // word/document.xml
        {
            "word/document.xml",
            [this]() {
                QString documentContent = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                                          "<w:document xmlns:wpc=\"http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas\" "
                                          "xmlns:cx=\"http://schemas.microsoft.com/office/drawing/2014/chartex\" "
                                          "xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" "
                                          "xmlns:o=\"urn:schemas-microsoft-com:office:office\" "
                                          "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" "
                                          "xmlns:m=\"http://schemas.openxmlformats.org/officeDocument/2006/math\" "
                                          "xmlns:v=\"urn:schemas-microsoft-com:vml\" "
                                          "xmlns:wp14=\"http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing\" "
                                          "xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\" "
                                          "xmlns:w10=\"urn:schemas-microsoft-com:office:word\" "
                                          "xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" "
                                          "xmlns:w14=\"http://schemas.microsoft.com/office/word/2010/wordml\" "
                                          "xmlns:w15=\"http://schemas.microsoft.com/office/word/2012/wordml\" "
                                          "xmlns:w16se=\"http://schemas.microsoft.com/office/word/2015/wordml/symex\" "
                                          "xmlns:wpg=\"http://schemas.microsoft.com/office/word/2010/wordprocessingGroup\" "
                                          "xmlns:wpi=\"http://schemas.microsoft.com/office/word/2010/wordprocessingInk\" "
                                          "xmlns:wne=\"http://schemas.microsoft.com/office/word/2006/wordml\" "
                                          "xmlns:wps=\"http://schemas.microsoft.com/office/word/2010/wordprocessingShape\" "
                                          "mc:Ignorable=\"w14 w15 w16se wp14\">"
                                          "<w:body>";
                QString full_lab_name = data_info.at(0);
                QString year = cur_data_IO.at(7);
                year = year.split(".").at(2);
                QString metrolog = getEmployeeOnPost(data_metr, QDate::currentDate());
                QStringList izg = cur_data_IO.at(5).split("@");
                QString properties = "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/>";

                documentContent +=
                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Испытательная лаборатория Общества с ограниченной ответственностью</w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + full_lab_name.replace("ИЛ ", "").replace("ПБ ", "") + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[(" + data_info.at(0) + ")]]></w:t>"
                    "</w:r>"
                    "</w:p>"


                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/><w:b/></w:rPr>"
                    "<w:t>КАРТОЧКА К ОБОРУДОВАНИЮ</w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + cur_data_IO.at(2) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/><w:b/></w:rPr>"
                    "<w:t>Сведения об оборудовании:</w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Тип оборудования ИО</w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Идентификация программного обеспечения (при наличии)&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[отсутствует]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Изготовитель (страна, наименование организации, год выпуска)&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + izg.at(0) + ", " + izg.at(1) + ", " + izg.at(2) +"]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Дата (год) ввода в эксплуатацию&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + year + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Заводской №&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + cur_data_IO.at(3) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Инвентарный №&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + cur_data_IO.at(0) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Местонахождение&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + data_info.at(1) + ", помещение " + cur_data_IO.at(4) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>№ инструкции по охране труда и проведении испытаний на оборудовании&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + cur_data_IO.at(8) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Ответственное лицо за оборудование&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + metrolog + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"14\"/><w:szCs w:val=\"14\"/><w:b/></w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/><w:b/></w:rPr>"
                    "<w:t><![CDATA[Результаты государственной поверки/аттестации:]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:tbl>"
                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblBorders>"
                    "<w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideH w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideV w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "</w:tblBorders>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"   

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"1764\"/>"
                    "<w:gridCol w:w=\"1939\"/>"
                    "<w:gridCol w:w=\"1866\"/>"
                    "<w:gridCol w:w=\"1998\"/>"
                    "<w:gridCol w:w=\"1778\"/>" //9345
                    "</w:tblGrid>"
                    
                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1764\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Дата поверки/ аттестации</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1939\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Метрологическая организация</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1866\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>№ свидетельства/ аттестата</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1998\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Результат поверки/ аттестации</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1778\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Дата следующей поверки/ аттестации</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"
                    "</w:tr>";

                QString pov = cur_data_IO.at(9);
                if (pov == "-"){
                    documentContent +=
                        "<w:tr>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1764\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t>-</w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1939\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t>-</w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1866\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t>-</w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1998\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t>пригодно</w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1778\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t>бессрочно</w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"
                        "</w:tr>";
                } else {
                    QStringList cur_pov_split;
                    QStringList pov_split = pov.split("@");
                    int pov_split_size = pov_split.size();
                    for (int z = 0; z < pov_split_size; z++) {
                        QString pov_first = pov_split.takeFirst();
                        cur_pov_split = pov_first.split(";");
                        documentContent +=
                            "<w:tr>"
                            "<w:tc><w:tcPr><w:tcW w:w=\"1764\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                            "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_pov_split.at(0) + "</w:t></w:r>"
                                "</w:p>"
                            "</w:tc>"

                            "<w:tc>"
                            "<w:tcPr><w:tcW w:w=\"1939\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                            "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_pov_split.at(1) + "</w:t></w:r>"
                                "</w:p>"
                            "</w:tc>"

                            "<w:tc>"
                            "<w:tcPr><w:tcW w:w=\"1866\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                            "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_pov_split.at(2) + "</w:t></w:r>"
                                "</w:p>"
                            "</w:tc>"

                            "<w:tc>"
                            "<w:tcPr><w:tcW w:w=\"1998\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                            "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                "<w:r><w:rPr>" + properties + "</w:rPr><w:t>пригодно</w:t></w:r>"
                                "</w:p>"
                            "</w:tc>"

                            "<w:tc>"
                            "<w:tcPr><w:tcW w:w=\"1778\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                            "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                                "<w:r><w:rPr>" + properties + "</w:rPr><w:t>" + cur_pov_split.at(3) + "</w:t></w:r>"
                                "</w:p>"
                            "</w:tc>"
                            "</w:tr>";
                    }
                }
                    // QDate date_att = QDate::fromString(pov_last_split.at(0), "dd.MM.yyyy");
                    // Написать логику для ситуации, когда ввели оборудование позже даты последней поверки

                QDate date_vvod = QDate::fromString(cur_data_IO.at(7), "dd.MM.yyyy");
                int TO_period = cur_data_IO.at(18).toInt();
                int clean_period = cur_data_IO.at(19).toInt();
                QVector<QStringList> dates_TO_rem_clean;
                qDebug() << TO_period;

                QVector<QDate> date_TO_list;
                date_TO_list.append(date_vvod);
                int added_days = 0;
                if (TO_period != 0){
                    while (date_TO_list.last() < QDate::currentDate().addMonths(-1 * TO_period)) {
                        QDate new_date = date_TO_list.last().addMonths(TO_period);
                        new_date = new_date.addDays(-1 * added_days);
                        added_days = 0;
                        while (new_date.dayOfWeek() == 6 || new_date.dayOfWeek() == 7){
                            added_days += 1;
                            new_date = new_date.addDays(1);
                        }
                        qDebug() << date_TO_list;
                        date_TO_list.append(new_date);
                    }
                }
                date_TO_list.takeFirst();

                QVector<QDate> date_clean_list;
                date_clean_list.append(date_vvod);
                if (clean_period != 0){
                    added_days = 0;
                    while (date_clean_list.last() < QDate::currentDate().addMonths(-1 * clean_period)) {
                        QDate new_date = date_clean_list.last().addMonths(clean_period);
                        new_date = new_date.addDays(-1 * added_days);
                        added_days = 0;
                        while (new_date.dayOfWeek() == 6 || new_date.dayOfWeek() == 7){
                            added_days += 1;
                            new_date = new_date.addDays(1);
                        }
                        date_clean_list.append(new_date);
                    }
                }
                date_clean_list.takeFirst();

                if (!date_TO_list.isEmpty()){
                    for (int x = 0; x < date_TO_list.size(); x++) {
                        QString current_metrolog = getEmployeeOnPost(data_metr, date_TO_list.at(x));
                        QString date_to = date_TO_list.at(x).toString("dd.MM.yyyy");
                        QStringList one_TO;
                        one_TO << date_to << "не выявлено" << "техническое обслуживание" << "1 день" << current_metrolog;
                        dates_TO_rem_clean.append(one_TO);
                    }
                }

                if (!date_TO_list.isEmpty()){
                    for (int x = 0; x < date_clean_list.size(); x++) {
                        QString current_metrolog = getEmployeeOnPost(data_metr, date_clean_list.at(x));
                        QString date_clean = date_clean_list.at(x).toString("dd.MM.yyyy");
                        QStringList one_clean;
                        one_clean << date_clean << "не выявлено" << "очистка" << "1 день" << current_metrolog;
                        dates_TO_rem_clean.append(one_clean);
                    }
                }

                QString unique_rem = cur_data_IO.at(20);
                QStringList rem_list = unique_rem.split("@");
                if (rem_list.size() >= 1){
                    for (int t = 0; t < rem_list.size(); t++) {
                        QString one_rem = rem_list.at(t);
                        QStringList one_rem_List = one_rem.split(";");
                        if (QDate::fromString(one_rem_List.at(0), "dd.MM.yyyy").isValid()){
                            dates_TO_rem_clean.append(one_rem_List);
                        }
                    }
                }

                if (dates_TO_rem_clean.size() >= 2){
                    for (int i = 0; i < dates_TO_rem_clean.size() - 1; ++i) {
                        for (int j = 0; j < dates_TO_rem_clean.size() - i - 1; ++j) {
                            QDate dateA = QDate::fromString(dates_TO_rem_clean[j][0], "dd.MM.yyyy");
                            QDate dateB = QDate::fromString(dates_TO_rem_clean[j + 1][0], "dd.MM.yyyy");
                            if (dateA > dateB) {
                                dates_TO_rem_clean.swapItemsAt(j, j + 1);
                            }
                        }
                    }
                }
                qDebug() << dates_TO_rem_clean;


                documentContent +=
                    "</w:tbl>"
                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"14\"/><w:szCs w:val=\"14\"/><w:b/></w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/><w:b/></w:rPr>"
                    "<w:t><![CDATA[Регистрация повреждений, неисправностей, модификации, ремонта, ТО:]]></w:t>"
                    "</w:r>"
                    "</w:p>"
                    
                    "<w:tbl>"
                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblBorders>"
                    "<w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideH w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideV w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "</w:tblBorders>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"   

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"1000\"/>"
                    "<w:gridCol w:w=\"3141\"/>"
                    "<w:gridCol w:w=\"1724\"/>"
                    "<w:gridCol w:w=\"1588\"/>"
                    "<w:gridCol w:w=\"1892\"/>" //9345
                    "</w:tblGrid>"
                    
                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1000\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Дата</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"3141\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Выявленное несоответствие (повреждение, неисправность и др.)</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1724\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Мероприятие</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1588\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Сроки</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1892\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>ФИО исполнителя</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"
                    "</w:tr>";
                
                for (int x = 0; x < dates_TO_rem_clean.size(); x++) {
                    documentContent +=
                        "<w:tr>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1000\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(0) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"3141\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(1) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1724\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(2) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1588\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(3) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1892\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(4) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"
                        "</w:tr>";
                }


                documentContent +=
                    "</w:tbl>"
                    "<w:sectPr>"
                    "<w:pgSz w:w=\"11906\" w:h=\"16838\"/>"
                    "<w:pgMar w:top=\"1134\" w:right=\"850\" w:bottom=\"1134\" w:left=\"1701\" w:header=\"708\" w:footer=\"708\" w:gutter=\"0\"/>"
                    "<w:cols w:space=\"708\"/>"
                    "<w:docGrid w:linePitch=\"360\"/>"
                    "</w:sectPr>"
                    "</w:body>"
                    "</w:document>";

                qDebug() << "Done";
                if (!dates_TO_rem_clean.isEmpty()){
                    last_modified = QDate::fromString(dates_TO_rem_clean.last().at(0), "dd.MM.yyyy");
                } else {
                    last_modified = QDate::fromString(cur_data_IO.at(7), "dd.MM.yyyy");
                }
                
                return documentContent.toUtf8();
            }()
        },

        // word/_rels/document.xml.rels
        {
            "word/_rels/document.xml.rels",
            [this]() {
                QString content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                  "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">";
                content += "</Relationships>";
                return content.toUtf8();
            }()
        },

        // docProps/core.xml
        {
            "docProps/core.xml",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" "
            "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "
            "xmlns:dcterms=\"http://purl.org/dc/terms/\" "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
            "<dc:creator>LENOVO</dc:creator>"
            "<dcterms:created xsi:type=\"dcterms:W3CDTF\">"
            + addRandomTimeToDate(QDate::fromString(cur_data_IO.at(7), "dd.MM.yyyy")).toString(Qt::ISODate).toUtf8() +
            "</dcterms:created>"
            "</cp:coreProperties>"
        }
    };

    QList<quint32> offsets;
    for (const auto &file : files) {
        quint32 offset = write_local_file(zip, file.first, file.second);
        offsets.append(offset);
    }

    write_central_directory(zip, files, offsets);
    file.close();
    QDateTime created = addRandomTimeToDate(QDate::fromString(cur_data_IO.at(7), "dd.MM.yyyy"));
    QDateTime modified = addRandomTimeToDate(last_modified);

    setFileTimes(fileName, created, modified);
}

void file_menu::create_kart_VO_Docx(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return;

    QDataStream zip(&file);
    zip.setByteOrder(QDataStream::LittleEndian);

    // 1. Основные файлы документа
    QList<QPair<QString, QByteArray>> files = {
        // [Content_Types].xml
        {
            "[Content_Types].xml",
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
            "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
            "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
            "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
            "<Override PartName=\"/word/document.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>"
            "<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>"
            "</Types>"
        },

        // _rels/.rels
        {
            "_rels/.rels",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"word/document.xml\"/>"
            "<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/>"
            "</Relationships>"
        },

        // word/document.xml
        {
            "word/document.xml",
            [this]() {
                QString documentContent = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                                          "<w:document xmlns:wpc=\"http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas\" "
                                          "xmlns:cx=\"http://schemas.microsoft.com/office/drawing/2014/chartex\" "
                                          "xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" "
                                          "xmlns:o=\"urn:schemas-microsoft-com:office:office\" "
                                          "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" "
                                          "xmlns:m=\"http://schemas.openxmlformats.org/officeDocument/2006/math\" "
                                          "xmlns:v=\"urn:schemas-microsoft-com:vml\" "
                                          "xmlns:wp14=\"http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing\" "
                                          "xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\" "
                                          "xmlns:w10=\"urn:schemas-microsoft-com:office:word\" "
                                          "xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" "
                                          "xmlns:w14=\"http://schemas.microsoft.com/office/word/2010/wordml\" "
                                          "xmlns:w15=\"http://schemas.microsoft.com/office/word/2012/wordml\" "
                                          "xmlns:w16se=\"http://schemas.microsoft.com/office/word/2015/wordml/symex\" "
                                          "xmlns:wpg=\"http://schemas.microsoft.com/office/word/2010/wordprocessingGroup\" "
                                          "xmlns:wpi=\"http://schemas.microsoft.com/office/word/2010/wordprocessingInk\" "
                                          "xmlns:wne=\"http://schemas.microsoft.com/office/word/2006/wordml\" "
                                          "xmlns:wps=\"http://schemas.microsoft.com/office/word/2010/wordprocessingShape\" "
                                          "mc:Ignorable=\"w14 w15 w16se wp14\">"
                                          "<w:body>";
                QString full_lab_name = data_info.at(0);
                QString year = cur_data_VO.at(6);
                year = year.split(".").at(2);
                QString metrolog = getEmployeeOnPost(data_metr, QDate::currentDate());
                QStringList izg = cur_data_VO.at(4).split("@");
                QString properties = "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/>";
                QString PO = cur_data_VO.at(8);
                if (PO == "отсутствует") {
                    PO = "";
                }
                documentContent +=
                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Испытательная лаборатория Общества с ограниченной ответственностью</w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + full_lab_name.replace("ИЛ ", "").replace("ПБ ", "") + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[(" + data_info.at(0) + ")]]></w:t>"
                    "</w:r>"
                    "</w:p>"


                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/><w:b/></w:rPr>"
                    "<w:t>КАРТОЧКА К ОБОРУДОВАНИЮ</w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + cur_data_VO.at(1) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/><w:b/></w:rPr>"
                    "<w:t>Сведения об оборудовании:</w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Тип оборудования ВО</w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Идентификация программного обеспечения (при наличии)&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + PO + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Изготовитель (страна, наименование организации, год выпуска)&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + izg.at(0) + ", " + izg.at(1) + ", " + izg.at(2) +"]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Дата (год) ввода в эксплуатацию&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + year + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Заводской №&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + cur_data_VO.at(2) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Инвентарный №&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + cur_data_VO.at(0) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Местонахождение&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + data_info.at(1) + ", помещение " + cur_data_VO.at(3) + "]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>№ инструкции по охране труда и проведении испытаний на оборудовании&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[паспорт]]></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t>Ответственное лицо за оборудование&#xA0;</w:t>"
                    "</w:r>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr>"
                    "<w:t><![CDATA[" + metrolog + "]]></w:t>"
                    "</w:r>"
                    "</w:p>";

                QDate date_vvod = QDate::fromString(cur_data_VO.at(6), "dd.MM.yyyy");
                int TO_period = cur_data_VO.at(14).toInt();
                int clean_period = cur_data_VO.at(15).toInt();
                QVector<QStringList> dates_TO_rem_clean;
                qDebug() << TO_period;

                QVector<QDate> date_TO_list;
                date_TO_list.append(date_vvod);
                int added_days = 0;
                if (TO_period != 0){
                    while (date_TO_list.last() < QDate::currentDate().addMonths(-1 * TO_period)) {
                        QDate new_date = date_TO_list.last().addMonths(TO_period);
                        new_date = new_date.addDays(-1 * added_days);
                        added_days = 0;
                        while (new_date.dayOfWeek() == 6 || new_date.dayOfWeek() == 7){
                            added_days += 1;
                            new_date = new_date.addDays(1);
                        }
                        qDebug() << date_TO_list;
                        date_TO_list.append(new_date);
                    }
                }
                date_TO_list.takeFirst();

                QVector<QDate> date_clean_list;
                date_clean_list.append(date_vvod);
                if (clean_period != 0){
                    added_days = 0;
                    while (date_clean_list.last() < QDate::currentDate().addMonths(-1 * clean_period)) {
                        QDate new_date = date_clean_list.last().addMonths(clean_period);
                        new_date = new_date.addDays(-1 * added_days);
                        added_days = 0;
                        while (new_date.dayOfWeek() == 6 || new_date.dayOfWeek() == 7){
                            added_days += 1;
                            new_date = new_date.addDays(1);
                        }
                        date_clean_list.append(new_date);
                    }
                }
                date_clean_list.takeFirst();

                if (!date_TO_list.isEmpty()){
                    for (int x = 0; x < date_TO_list.size(); x++) {
                        QString current_metrolog = getEmployeeOnPost(data_metr, date_TO_list.at(x));
                        QString date_to = date_TO_list.at(x).toString("dd.MM.yyyy");
                        QStringList one_TO;
                        one_TO << date_to << "не выявлено" << "техническое обслуживание" << "1 день" << current_metrolog;
                        dates_TO_rem_clean.append(one_TO);
                    }
                }

                if (!date_TO_list.isEmpty()){
                    for (int x = 0; x < date_clean_list.size(); x++) {
                        QString current_metrolog = getEmployeeOnPost(data_metr, date_clean_list.at(x));
                        QString date_clean = date_clean_list.at(x).toString("dd.MM.yyyy");
                        QStringList one_clean;
                        one_clean << date_clean << "не выявлено" << "очистка" << "1 день" << current_metrolog;
                        dates_TO_rem_clean.append(one_clean);
                    }
                }

                QString unique_rem = cur_data_VO.at(16);
                QStringList rem_list = unique_rem.split("@");
                if (rem_list.size() >= 1){
                    for (int t = 0; t < rem_list.size(); t++) {
                        QString one_rem = rem_list.at(t);
                        QStringList one_rem_List = one_rem.split(";");
                        if (QDate::fromString(one_rem_List.at(0), "dd.MM.yyyy").isValid()){
                            dates_TO_rem_clean.append(one_rem_List);
                        }
                    }
                }

                if (dates_TO_rem_clean.size() >= 2){
                    for (int i = 0; i < dates_TO_rem_clean.size() - 1; ++i) {
                        for (int j = 0; j < dates_TO_rem_clean.size() - i - 1; ++j) {
                            QDate dateA = QDate::fromString(dates_TO_rem_clean[j][0], "dd.MM.yyyy");
                            QDate dateB = QDate::fromString(dates_TO_rem_clean[j + 1][0], "dd.MM.yyyy");
                            if (dateA > dateB) {
                                dates_TO_rem_clean.swapItemsAt(j, j + 1);
                            }
                        }
                    }
                }
                qDebug() << dates_TO_rem_clean;


                documentContent +=
                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"14\"/><w:szCs w:val=\"14\"/><w:b/></w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>"

                    "<w:p>"
                    "<w:pPr><w:jc w:val=\"left\"/><w:spacing w:after=\"0\"/></w:pPr>"
                    "<w:r>"
                    "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/><w:b/></w:rPr>"
                    "<w:t><![CDATA[Регистрация повреждений, неисправностей, модификации, ремонта, ТО:]]></w:t>"
                    "</w:r>"
                    "</w:p>"
                    
                    "<w:tbl>"
                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblBorders>"
                    "<w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideH w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "<w:insideV w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/>"
                    "</w:tblBorders>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"   

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"1000\"/>"
                    "<w:gridCol w:w=\"3141\"/>"
                    "<w:gridCol w:w=\"1724\"/>"
                    "<w:gridCol w:w=\"1588\"/>"
                    "<w:gridCol w:w=\"1892\"/>" //9345
                    "</w:tblGrid>"
                    
                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1000\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Дата</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"3141\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Выявленное несоответствие (повреждение, неисправность и др.)</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1724\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Мероприятие</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1588\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>Сроки</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"

                    "<w:tc>"
                    "<w:tcPr><w:tcW w:w=\"1892\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                    "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                        "<w:r><w:rPr>" + properties + "</w:rPr><w:t>ФИО исполнителя</w:t></w:r>"
                        "</w:p>"
                    "</w:tc>"
                    "</w:tr>";
                
                for (int x = 0; x < dates_TO_rem_clean.size(); x++) {
                    documentContent +=
                        "<w:tr>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1000\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(0) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"3141\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(1) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1724\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(2) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1588\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(3) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"

                        "<w:tc>"
                        "<w:tcPr><w:tcW w:w=\"1892\" w:type=\"dxa\"/><w:vAlign w:val=\"center\"/></w:tcPr>"
                        "<w:p><w:pPr><w:ind w:left=\"20\" w:right=\"20\"/><w:jc w:val=\"center\"/><w:spacing w:after=\"0\"/></w:pPr>"
                            "<w:r><w:rPr>" + properties + "</w:rPr><w:t><![CDATA[" + dates_TO_rem_clean.at(x).at(4) + "]]></w:t></w:r>"
                            "</w:p>"
                        "</w:tc>"
                        "</w:tr>";
                }


                documentContent +=
                    "</w:tbl>"
                    "<w:sectPr>"
                    "<w:pgSz w:w=\"11906\" w:h=\"16838\"/>"
                    "<w:pgMar w:top=\"1134\" w:right=\"850\" w:bottom=\"1134\" w:left=\"1701\" w:header=\"708\" w:footer=\"708\" w:gutter=\"0\"/>"
                    "<w:cols w:space=\"708\"/>"
                    "<w:docGrid w:linePitch=\"360\"/>"
                    "</w:sectPr>"
                    "</w:body>"
                    "</w:document>";

                qDebug() << "Done";
                if (!dates_TO_rem_clean.isEmpty()){
                    last_modified = QDate::fromString(dates_TO_rem_clean.last().at(0), "dd.MM.yyyy");
                } else {
                    last_modified = QDate::fromString(cur_data_VO.at(6), "dd.MM.yyyy");
                }
                
                return documentContent.toUtf8();
            }()
        },

        // word/_rels/document.xml.rels
        {
            "word/_rels/document.xml.rels",
            [this]() {
                QString content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                  "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">";
                content += "</Relationships>";
                return content.toUtf8();
            }()
        },

        // docProps/core.xml
        {
            "docProps/core.xml",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" "
            "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "
            "xmlns:dcterms=\"http://purl.org/dc/terms/\" "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
            "<dc:creator>LENOVO</dc:creator>"
            "<dcterms:created xsi:type=\"dcterms:W3CDTF\">"
            + addRandomTimeToDate(QDate::fromString(cur_data_VO.at(6), "dd.MM.yyyy")).toString(Qt::ISODate).toUtf8() +
            "</dcterms:created>"
            "</cp:coreProperties>"
        }
    };

    QList<quint32> offsets;
    for (const auto &file : files) {
        quint32 offset = write_local_file(zip, file.first, file.second);
        offsets.append(offset);
    }

    write_central_directory(zip, files, offsets);
    file.close();
    QDateTime created = addRandomTimeToDate(QDate::fromString(cur_data_VO.at(6), "dd.MM.yyyy"));
    QDateTime modified = addRandomTimeToDate(last_modified);

    setFileTimes(fileName, created, modified);
}

QString file_menu::getEmployeeOnPost(const QString &data, const QDate &targetDate) {
    QStringList employees = data.split('@', Qt::SkipEmptyParts);
    if (employees.isEmpty()) return QString();

    QString currentEmployee = employees[0].trimmed();
    if (employees.size() == 1) return currentEmployee;

    QList<QDate> dates{QDate(1, 1, 1)};
    QStringList names{currentEmployee};

    for (int i = 1; i < employees.size(); ++i) {
        QStringList parts = employees[i].split(';', Qt::SkipEmptyParts);
        if (parts.size() != 2) continue;

        QDate date = QDate::fromString(parts[0].trimmed(), "dd.MM.yyyy");
        if (date.isValid()) {
            dates.append(date);
            names.append(parts[1].trimmed());
        }
    }
    int resultIndex = 0;
    for (int i = 1; i < dates.size(); ++i) {
        if (dates[i] <= targetDate) {
            resultIndex = i;
        } else {
            break;
        }
    }

    return names[resultIndex];
}

quint32 file_menu::calculate_crc32(const QByteArray &data) {
    quint32 crc = 0xffffffff;
    for (char c : data) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ static_cast<quint8>(c)) & 0xff];
    }
    return crc ^ 0xffffffff;
}

quint32 file_menu::datetime_to_dos(const QDateTime &dt) {
    QDate date = dt.date();
    QTime time = dt.time();

    quint32 dos_time = (time.second()/2) | (time.minute() << 5) | (time.hour() << 11);
    quint32 dos_date = date.day() | (date.month() << 5) | ((date.year() - 1980) << 9);
    return (dos_date << 16) | dos_time;
}

quint32 file_menu::write_local_file(QDataStream &zip,
                                const QString &name,
                                const QByteArray &content) {
    const quint32 header_start = zip.device()->pos();

    // Local file header
    zip << quint32(0x04034b50);    // signature
    zip << quint16(20);            // version
    zip << quint16(0);             // flags
    zip << quint16(0);             // compression
    zip << datetime_to_dos(QDateTime::currentDateTime());
    zip << calculate_crc32(content);
    zip << quint32(content.size()); // compressed size
    zip << quint32(content.size()); // uncompressed size
    zip << quint16(name.toUtf8().size());
    zip << quint16(0);             // extra field length

    // File name
    zip.writeRawData(name.toUtf8().constData(), name.toUtf8().size());

    // File data
    zip.writeRawData(content.constData(), content.size());

    return header_start;
}

void file_menu::write_central_directory(QDataStream &zip,
                                    const QList<QPair<QString, QByteArray>> &files,
                                    const QList<quint32> &offsets) {
    const quint32 cd_start = zip.device()->pos();

    for (int i = 0; i < files.size(); ++i) {
        const auto &file = files[i];
        quint32 header_offset = offsets[i];
        QByteArray name = file.first.toUtf8();
        QByteArray content = file.second;

        zip << quint32(0x02014b50); // signature
        zip << quint16(20);         // version made by
        zip << quint16(20);         // version needed
        zip << quint16(0);          // flags
        zip << quint16(0);          // compression
        zip << datetime_to_dos(QDateTime::currentDateTime());
        zip << calculate_crc32(content);
        zip << quint32(content.size()); // compressed
        zip << quint32(content.size()); // uncompressed
        zip << quint16(name.size());    // name length
        zip << quint16(0);             // extra length
        zip << quint16(0);             // comment length
        zip << quint16(0);            // disk number
        zip << quint16(0);            // internal attributes
        zip << quint32(0);            // external attributes
        zip << quint32(header_offset); // offset

        zip.writeRawData(name.constData(), name.size());
    }

    const quint32 cd_size = zip.device()->pos() - cd_start;

    // End of central directory
    zip << quint32(0x06054b50);    // signature
    zip << quint16(0);             // disk number
    zip << quint16(0);             // start disk
    zip << quint16(files.size());  // entries on disk
    zip << quint16(files.size());  // total entries
    zip << quint32(cd_size);       // CD size
    zip << quint32(cd_start);      // CD offset
    zip << quint16(0);             // comment length
}

bool file_menu::setFileTimes(const QString &filePath, const QDateTime &creationTime, const QDateTime &modificationTime) {
    // Проверяем существование файла
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        qDebug() << "Ошибка: Файл не существует:" << filePath;
        return false;
    }

    // Проверяем валидность времени
    if (!creationTime.isValid() || !modificationTime.isValid()) {
        qDebug() << "Ошибка: Некорректное время. Создание:" << creationTime << "Изменение:" << modificationTime;
        return false;
    }

    // Конвертируем QString в UTF-16 для Win32 API
    LPCWSTR wFilePath = reinterpret_cast<LPCWSTR>(filePath.utf16());

    // Открываем файл с правами на изменение атрибутов
    HANDLE hFile = CreateFileW(
        wFilePath,
        FILE_WRITE_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
        );

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD lastError = GetLastError();
        qDebug() << "Ошибка открытия файла. Код ошибки:" << lastError;
        return false;
    }

    // Конвертация QDateTime в FILETIME
    const qint64 WINDOWS_TICK = 10000000; // 100 нс в секундах
    const qint64 SEC_TO_UNIX_EPOCH = 11644473600LL; // Секунды между 1601-01-01 и 1970-01-01

    FILETIME ftCreation, ftModification;
    qint64 creationTicks = (creationTime.toMSecsSinceEpoch() * 10000) + (SEC_TO_UNIX_EPOCH * WINDOWS_TICK);
    ftCreation.dwLowDateTime = static_cast<DWORD>(creationTicks & 0xFFFFFFFF);
    ftCreation.dwHighDateTime = static_cast<DWORD>(creationTicks >> 32);

    qint64 modificationTicks = (modificationTime.toMSecsSinceEpoch() * 10000) + (SEC_TO_UNIX_EPOCH * WINDOWS_TICK);
    ftModification.dwLowDateTime = static_cast<DWORD>(modificationTicks & 0xFFFFFFFF);
    ftModification.dwHighDateTime = static_cast<DWORD>(modificationTicks >> 32);

    // Установка времени
    BOOL result = SetFileTime(hFile, &ftCreation, nullptr, &ftModification);
    DWORD lastError = GetLastError();
    CloseHandle(hFile);

    if (!result) {
        qDebug() << "Ошибка установки времени файла. Код ошибки:" << lastError;
        return false;
    }

    // Проверяем, что время установлено корректно
    QFileInfo updatedInfo(filePath);
    QDateTime actualCreationTime = updatedInfo.birthTime();
    QDateTime actualModificationTime = updatedInfo.lastModified();
    if (actualCreationTime != creationTime || actualModificationTime != modificationTime) {
        qDebug() << "Время не соответствует. Ожидалось: Создание=" << creationTime
                 << "Изменение=" << modificationTime
                 << ", Получено: Создание=" << actualCreationTime
                 << "Изменение=" << actualModificationTime;
        return false;
    }

    qDebug() << "Время файла успешно установлено. Создание:" << creationTime
             << "Изменение:" << modificationTime;
    return true;
}

QDateTime file_menu::addRandomTimeToDate(const QDate &date) {
    // Инициализируем генератор случайных чисел
    QRandomGenerator *generator = QRandomGenerator::global();

    // Генерируем случайное время: часы от 9 до 16, минуты от 0 до 59
    int hours = generator->bounded(9, 17); // [9, 16]
    int minutes = generator->bounded(0, 60); // [0, 59]

    // Создаём QTime
    QTime randomTime(hours, minutes);

    // Создаём QDateTime с указанным QDate и случайным QTime
    QDateTime dateTime(date, randomTime, QTimeZone("Europe/Moscow"));

    return dateTime;
}

void file_menu::on_listView_allinv_entered(const QModelIndex &index)
{
    ui->pushButton_birki->setText("Выгрузить бирки");
    ui->pushButton_karts->setText("Выгрузить карточки");
}
