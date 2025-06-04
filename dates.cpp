//dates.cpp
#include "dates.h"
#include "ui_dates.h"
#include "database.h"

#include <QtSql>
#include <QTextCharFormat>
#include <QHBoxLayout>
#include <QLineEdit>

#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QDateTime>
#include <QDataStream>
#include <QtEndian>
#include <QFileDialog>

const quint32 Dates::crc32_table[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59
};


Dates::Dates(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dates)
{
    ui->setupUi(this);
    Database db2;
    db2.openDatabase("database.db");
    existsSI = db2.getExistsSIFromDatabase();
    datespov = db2.getDatesPovFromDatabase();
    existsIO = db2.getExistsIOFromDatabase();
    datesatt = db2.getDatesAttFromDatabase();
    QStringList inv_pov = db2.getInvPovFromDatabase();
    QStringList names_pov = db2.getNamesPovFromDatabase();
    QStringList inv_att = db2.getInvAttFromDatabase();
    QStringList names_att = db2.getNamesAttFromDatabase();
    data_SI = db2.get_data_SI();
    data_IO = db2.get_data_IO();
    data_VO = db2.get_data_VO();
    metr = db2.get_data_metr();
    gen = db2.get_data_gen();
    ruk = db2.get_data_ruk();
    info = db2.get_data_info();
    db2.close_conn();

    QDate current_date;
    current_date = current_date.currentDate();
    current_date = current_date.addMonths(1);

    for (int x = 0; x < datespov.size(); x++){
        if ((current_date > datespov.at(x)) && existsSI.at(x)){
            this->inv_pov_in_month.append(inv_pov.at(x));
            this->names_pov_in_month.append(names_pov.at(x));
            this->datespov_in_month.append(datespov.at(x));
        }
    }

    for (int x = 0; x < datesatt.size(); x++){
        if ((current_date > datesatt.at(x)) && existsIO.at(x)){
            this->inv_att_in_month.append(inv_att.at(x));
            this->names_att_in_month.append(names_att.at(x));
            this->datesatt_in_month.append(datesatt.at(x));
        }
    }

    year = QDate::currentDate().year();

    ui->calendarWidget_jan->setMinimumDate(QDate(year, 1, 1));
    ui->calendarWidget_jan->setMaximumDate(QDate(year, 1, 31));

    int daysInfeb = QDate(year, 2, 1).daysInMonth();
    ui->calendarWidget_feb->setMinimumDate(QDate(year, 2, 1));
    ui->calendarWidget_feb->setMaximumDate(QDate(year, 2, daysInfeb));

    ui->calendarWidget_mar->setMinimumDate(QDate(year, 3, 1));
    ui->calendarWidget_mar->setMaximumDate(QDate(year, 3, 31));

    ui->calendarWidget_apr->setMinimumDate(QDate(year, 4, 1));
    ui->calendarWidget_apr->setMaximumDate(QDate(year, 4, 30));

    ui->calendarWidget_may->setMinimumDate(QDate(year, 5, 1));
    ui->calendarWidget_may->setMaximumDate(QDate(year, 5, 31));

    ui->calendarWidget_jun->setMinimumDate(QDate(year, 6, 1));
    ui->calendarWidget_jun->setMaximumDate(QDate(year, 6, 30));

    ui->calendarWidget_jul->setMinimumDate(QDate(year, 7, 1));
    ui->calendarWidget_jul->setMaximumDate(QDate(year, 7, 31));

    ui->calendarWidget_aug->setMinimumDate(QDate(year, 8, 1));
    ui->calendarWidget_aug->setMaximumDate(QDate(year, 8, 31));

    ui->calendarWidget_sep->setMinimumDate(QDate(year, 9, 1));
    ui->calendarWidget_sep->setMaximumDate(QDate(year, 9, 30));

    ui->calendarWidget_oct->setMinimumDate(QDate(year, 10, 1));
    ui->calendarWidget_oct->setMaximumDate(QDate(year, 10, 31));

    ui->calendarWidget_nov->setMinimumDate(QDate(year, 11, 1));
    ui->calendarWidget_nov->setMaximumDate(QDate(year, 11, 30));

    ui->calendarWidget_dec->setMinimumDate(QDate(year, 12, 1));
    ui->calendarWidget_dec->setMaximumDate(QDate(year, 12, 31));

    QColor color_pov(255, 222, 173);
    QTextCharFormat format_pov;
    format_pov.setBackground(color_pov);
    for (int x = 0; x < this->datespov.size(); x++){
        if (existsSI.at(x)){
            ui->calendarWidget_jan->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_feb->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_mar->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_apr->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_may->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_jun->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_jul->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_aug->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_sep->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_oct->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_nov->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_dec->setDateTextFormat(this->datespov.at(x), format_pov);
        }
    }

    ui->label->setText("Поверок в пределах текущей даты + месяц:");
    ui->spinBox->setValue(this->datespov_in_month.size());

    clean_layout(layouts_att_List);
    fill_layout(datespov_in_month, inv_pov_in_month, names_pov_in_month, layouts_pov_List);
    ui->pushButton->setChecked(true);
}

void Dates::create_graf_pov(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return;

    QDataStream zip(&file);
    zip.setByteOrder(QDataStream::LittleEndian);

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
                                          "<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">"
                                          "<w:body>";
                QString lab = info.at(0);
                lab = lab.replace("ИЛ ", "");
                lab = lab.replace("ПБ ", "");
                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:jc w:val=\"right\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t>УТВЕРЖДАЮ<w:br/>Генеральный директор<w:br/>" + lab + "<w:br/>______________" + current_gen + "<w:br/>«___» _________ 20___ г.</w:t>"
                    "</w:r>"
                    "</w:p>";
                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:jc w:val=\"center\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"20\"/>"
                    "<w:szCs w:val=\"20\"/>"
                    "<w:b/>"
                    "</w:rPr>"
                    "<w:t>ГРАФИК<w:br/>периодической поверки<w:br/>средств измерений на " + QString::number(year) + " год</w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
                    "<w:tbl>"
                    "<w:tblPr>"

                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
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
                    "<w:gridCol w:w=\"279\"/>"
                    "<w:gridCol w:w=\"1009\"/>"
                    "<w:gridCol w:w=\"850\"/>"
                    "<w:gridCol w:w=\"709\"/>"
                    "<w:gridCol w:w=\"567\"/>"

                    "<w:gridCol w:w=\"709\"/>"
                    "<w:gridCol w:w=\"758\"/>"
                    "<w:gridCol w:w=\"330\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"

                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"330\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"

                    "<w:gridCol w:w=\"330\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"1062\"/>"
                    "</w:tblGrid>"



                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>№ п/п</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1009\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Наименование, тип, заводской номер</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Диапазон измерений</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Разряд, класс точности</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Общее кол-во</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Периодичность поверки<w:br/>в месяцах</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"758\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-40\" w:right=\"-40\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Дата последней поверки</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"3969\" w:type=\"dxa\"/><w:gridSpan w:val=\"12\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Сроки проведения поверки<w:br/>(количество по месяцам)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1062\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Примечание</w:t></w:r></w:p></w:tc>"
                    "</w:tr>"


                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1009\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"758\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"992\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>I кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"992\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>II кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"992\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>III кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"993\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>IV кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1062\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>"



                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1009\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"758\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>I</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>II</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>III</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>IV</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>V</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>VI</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>VII</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>VIII</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>IX</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>X</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>XI</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>XII</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1062\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>";
                int stroka = 0;
                QString arhDate_string = "01.01.";
                arhDate_string += QString::number(year - 1);
                QDate arhDate = QDate::fromString(arhDate_string, "dd.MM.yyyy");
                for (int i = 0; i < data_SI.size(); ++i) {
                    bool found = false;
                    if (data_SI.at(i).at(12) == "-") {
                        continue;
                    }
                    QString pov;
                    QStringList pov_list;
                    QStringList all_pov_list = data_SI.at(i).at(12).split("@");
                    pov = all_pov_list.last();
                    pov_list = pov.split(";");

                    QDate startDate;
                    QDate endDate;
                    int totalMonths;
                    for (int j = all_pov_list.size()-1; j >= 0; j--) {
                        QStringList pov_parts = all_pov_list.at(j).split(";");
                        startDate = QDate::fromString(pov_parts.at(0), "dd.MM.yyyy");
                        endDate = QDate::fromString(pov_parts.at(3), "dd.MM.yyyy");
                        int years = endDate.year() - startDate.year();
                        int months = endDate.month() - startDate.month();
                        if (months % 6 != 0){
                            months += 1;
                        }
                        totalMonths = years * 12 + months;

                        if (startDate <= arhDate && arhDate <= endDate) {
                            int pov_count = 0;
                            if (endDate.year() < year){
                                while (endDate.year() < year) {
                                    endDate = endDate.addMonths(totalMonths);
                                    pov_count += 1;
                                }
                            }
                            int days = (-1 * pov_count);
                            endDate = endDate.addDays(days);
                            pov_list = pov_parts;
                            found = true;
                            if (endDate.year() != year) {
                                found = false;
                            }
                            break;
                        }
                    }
                    if (!found) {
                        continue;
                    }

                    QString date_SI = data_SI.at(i).at(10);
                    QStringList date_SI_List = date_SI.split("@");
                    found = false;

                    if (date_SI_List.size() > 1){
                        QDate startDate = QDate::fromString(date_SI_List.at(0), "dd.MM.yyyy");
                        QDate endDate = QDate::fromString(date_SI_List.at(1), "dd.MM.yyyy");
                        if (startDate <= arhDate && arhDate <= endDate) {
                            date_SI = date_SI_List.at(0).split(".").at(2);
                            found = true;
                        }
                    } else {
                        QDate startDate = QDate::fromString(date_SI_List.at(0), "dd.MM.yyyy");
                        if (startDate <= arhDate) {
                            date_SI = date_SI_List.at(0).split(".").at(2);
                            found = true;
                        }
                    }
                    if (!found) {
                        continue;
                    }

                    QString diap_str = data_SI.at(i).at(11);
                    QStringList diap_list = diap_str.split("@");
                    QStringList pokazatel_l;
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
                        pokazatel_l.append(one_diap.at(0));
                        diapazony_l.append(ddd);
                        pogreshnosty_l.append(one_diap.at(3));
                    }

                    QSet<QString> uniqueStrings;
                    QStringList resultList;

                    for (const QString& one_pokazatel : qAsConst(pokazatel_l)) {
                        if (!uniqueStrings.contains(one_pokazatel)) {
                            uniqueStrings.insert(one_pokazatel);
                            resultList.append(one_pokazatel);
                        }
                    }

                    pokazatel_l = resultList;
                    QString pokazatel = pokazatel_l.join(",<w:br/>");
                    QString diapazony = diapazony_l.join("<w:br/>");
                    QString pogreshnosty = pogreshnosty_l.join("<w:br/>");

                    QString jan;
                    QString feb;
                    QString mar;
                    QString apr;
                    QString may;
                    QString jun;
                    QString jul;
                    QString aug;
                    QString sep;
                    QString oct;
                    QString nov;
                    QString dec;

                    switch (endDate.month()) {
                    case 1:
                        jan = QString::number(endDate.day());
                        break;
                    case 2:
                        feb = QString::number(endDate.day());
                        break;
                    case 3:
                        mar = QString::number(endDate.day());
                        break;
                    case 4:
                        apr = QString::number(endDate.day());
                        break;
                    case 5:
                        may = QString::number(endDate.day());
                        break;
                    case 6:
                        jun = QString::number(endDate.day());
                        break;
                    case 7:
                        jul = QString::number(endDate.day());
                        break;
                    case 8:
                        aug = QString::number(endDate.day());
                        break;
                    case 9:
                        sep = QString::number(endDate.day());
                        break;
                    case 10:
                        oct = QString::number(endDate.day());
                        break;
                    case 11:
                        nov = QString::number(endDate.day());
                        break;
                    case 12:
                        dec = QString::number(endDate.day());
                        break;
                    default:
                        break;
                    }

                    stroka += 1;
                    documentContent +=
                        "<w:tr>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + QString::number(stroka) + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1009\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + data_SI.at(i).at(1) + ",<w:br/>" + data_SI.at(i).at(2) + ",<w:br/>зав. № " + data_SI.at(i).at(4) + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + diapazony + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + pogreshnosty + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>1</w:t></w:r></w:p></w:tc>"

                        "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + QString::number(totalMonths) + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"758\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + pov_list.at(0) + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + jan + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + feb + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + mar + "</w:t></w:r></w:p></w:tc>"

                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + apr + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + may + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + jun + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + jul + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + aug + "</w:t></w:r></w:p></w:tc>"

                        "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + sep + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + oct + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + nov + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + dec + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1062\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                        "</w:tr>";
                }

                documentContent +=
                    "</w:tbl>"
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"0\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"0\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t>Составлено:<w:br/>Ответственный за метрологическое обеспечение</w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
                    "<w:tbl>"

                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"4677\"/>"
                    "<w:gridCol w:w=\"4678\"/>"
                    "</w:tblGrid>" // 9355

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>(инженер-метролог)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>" + current_metr + "</w:t></w:r></w:p></w:tc>"
                    "</w:tr>"

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>"
                    "</w:tbl>";

                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"0\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t>Согласовано:</w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
                    "<w:tbl>"

                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"4677\"/>"
                    "<w:gridCol w:w=\"4678\"/>"
                    "</w:tblGrid>" // 9355

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>Руководитель " + info.at(0) + "</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>" + current_ruk + "</w:t></w:r></w:p></w:tc>"
                    "</w:tr>"

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>"
                    "</w:tbl>";

                documentContent +=
                    "<w:sectPr>"
                    "<w:pgSz w:w=\"11906\" w:h=\"16838\"/>"
                    "<w:pgMar w:top=\"1134\" w:right=\"850\" w:bottom=\"1134\" w:left=\"1701\" w:header=\"708\" w:footer=\"708\" w:gutter=\"0\"/>"
                    "<w:cols w:space=\"720\"/>"
                    "<w:docGrid w:linePitch=\"360\"/>"
                    "</w:sectPr>"
                    "</w:body>"
                    "</w:document>";

                return documentContent.toUtf8();
            }()
        },

        // word/_rels/document.xml.rels
        {
            "word/_rels/document.xml.rels",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "</Relationships>"
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

void Dates::create_graf_pov_dop(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return;

    QDataStream zip(&file);
    zip.setByteOrder(QDataStream::LittleEndian);

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
                                          "<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">"
                                          "<w:body>";
                QString lab = info.at(0);
                lab = lab.replace("ИЛ", "");
                lab = lab.replace("ПБ", "");
                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:jc w:val=\"right\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t>УТВЕРЖДАЮ<w:br/>Генеральный директор<w:br/>" + lab + "<w:br/>______________" + current_gen + "<w:br/>«___» _________ 20___ г.</w:t>"
                                                            "</w:r>"
                                                            "</w:p>";
                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:jc w:val=\"center\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"20\"/>"
                    "<w:szCs w:val=\"20\"/>"
                    "<w:b/>"
                    "</w:rPr>"
                    "<w:t>Дополнение № " + QString::number(dop_num) + "<w:br/>к графику периодической поверки<w:br/>средств измерений на " + QString::number(year) + " год</w:t>"
                                              "</w:r>"
                                              "</w:p>";

                documentContent +=
                    "<w:tbl>"
                    "<w:tblPr>"

                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
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
                    "<w:gridCol w:w=\"279\"/>"
                    "<w:gridCol w:w=\"1009\"/>"
                    "<w:gridCol w:w=\"850\"/>"
                    "<w:gridCol w:w=\"709\"/>"
                    "<w:gridCol w:w=\"567\"/>"

                    "<w:gridCol w:w=\"709\"/>"
                    "<w:gridCol w:w=\"758\"/>"
                    "<w:gridCol w:w=\"330\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"

                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"330\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"

                    "<w:gridCol w:w=\"330\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"1062\"/>"
                    "</w:tblGrid>"



                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>№ п/п</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1009\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Наименование, тип, заводской номер</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Диапазон измерений</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Разряд, класс точности</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Общее кол-во</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Периодичность поверки<w:br/>в месяцах</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"758\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-40\" w:right=\"-40\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Дата последней поверки</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"3969\" w:type=\"dxa\"/><w:gridSpan w:val=\"12\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Сроки проведения поверки<w:br/>(количество по месяцам)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1062\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Примечание</w:t></w:r></w:p></w:tc>"
                    "</w:tr>"


                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1009\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"758\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"992\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>I кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"992\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>II кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"992\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>III кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"993\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>IV кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1062\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>"



                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1009\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"758\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>I</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>II</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>III</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>IV</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>V</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>VI</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>VII</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>VIII</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>IX</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>X</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>XI</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>XII</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1062\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>";
                int stroka = 0;
                QString arhDate_string = "01.01.";
                arhDate_string += QString::number(year);
                QDate arhDate = QDate::fromString(arhDate_string, "dd.MM.yyyy");
                for (int i = 0; i < data_SI.size(); ++i) {
                    if (data_SI.at(i).at(12) == "-") {
                        continue;
                    }
                    QString pov;
                    QStringList pov_list;
                    QStringList all_pov_list = data_SI.at(i).at(12).split("@");
                    pov = all_pov_list.last();
                    pov_list = pov.split(";");
                    bool found = false;

                    QDate startDate;
                    QDate endDate;
                    int totalMonths;
                    for (int j = all_pov_list.size()-1; j >= 0; j--) {
                        QStringList pov_parts = all_pov_list.at(j).split(";");
                        startDate = QDate::fromString(pov_parts.at(0), "dd.MM.yyyy");
                        endDate = QDate::fromString(pov_parts.at(3), "dd.MM.yyyy");
                        int years = endDate.year() - startDate.year();
                        int months = endDate.month() - startDate.month();
                        if (months % 6 != 0){
                            months += 1;
                        }
                        totalMonths = years * 12 + months;

                        if (startDate <= arhDate && arhDate <= endDate) {
                            int pov_count = 0;
                            if (endDate.year() < year){
                                while (endDate.year() < year) {
                                    endDate = endDate.addMonths(totalMonths);
                                    pov_count += 1;
                                }
                            }
                            int days = (-1 * pov_count);
                            endDate = endDate.addDays(days);
                            pov_list = pov_parts;
                            found = true;
                            if (endDate.year() != year) {
                                found = false;
                            }
                            break;
                        }
                    }
                    if (!found) {
                        continue;
                    }

                    QString date_SI = data_SI.at(i).at(10);
                    QStringList date_SI_List = date_SI.split("@");
                    found = false;

                    if (date_SI_List.size() > 1){
                        QDate startDate = QDate::fromString(date_SI_List.at(0), "dd.MM.yyyy");
                        QDate endDate = QDate::fromString(date_SI_List.at(1), "dd.MM.yyyy");
                        if (startDate <= arhDate && arhDate <= endDate) {
                            date_SI = date_SI_List.at(0).split(".").at(2);
                            found = true;
                        }
                    } else {
                        QDate startDate = QDate::fromString(date_SI_List.at(0), "dd.MM.yyyy");
                        if (startDate <= arhDate) {
                            date_SI = date_SI_List.at(0).split(".").at(2);
                            found = true;
                        }
                    }
                    if (!found) {
                        continue;
                    }

                    QString diap_str = data_SI.at(i).at(11);
                    QStringList diap_list = diap_str.split("@");
                    QStringList pokazatel_l;
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
                        pokazatel_l.append(one_diap.at(0));
                        diapazony_l.append(ddd);
                        pogreshnosty_l.append(one_diap.at(3));
                    }

                    QSet<QString> uniqueStrings;
                    QStringList resultList;

                    for (const QString& one_pokazatel : qAsConst(pokazatel_l)) {
                        if (!uniqueStrings.contains(one_pokazatel)) {
                            uniqueStrings.insert(one_pokazatel);
                            resultList.append(one_pokazatel);
                        }
                    }

                    pokazatel_l = resultList;
                    QString pokazatel = pokazatel_l.join(",<w:br/>");
                    QString diapazony = diapazony_l.join("<w:br/>");
                    QString pogreshnosty = pogreshnosty_l.join("<w:br/>");

                    QString jan;
                    QString feb;
                    QString mar;
                    QString apr;
                    QString may;
                    QString jun;
                    QString jul;
                    QString aug;
                    QString sep;
                    QString oct;
                    QString nov;
                    QString dec;

                    switch (endDate.month()) {
                    case 1:
                        jan = QString::number(endDate.day());
                        break;
                    case 2:
                        feb = QString::number(endDate.day());
                        break;
                    case 3:
                        mar = QString::number(endDate.day());
                        break;
                    case 4:
                        apr = QString::number(endDate.day());
                        break;
                    case 5:
                        may = QString::number(endDate.day());
                        break;
                    case 6:
                        jun = QString::number(endDate.day());
                        break;
                    case 7:
                        jul = QString::number(endDate.day());
                        break;
                    case 8:
                        aug = QString::number(endDate.day());
                        break;
                    case 9:
                        sep = QString::number(endDate.day());
                        break;
                    case 10:
                        oct = QString::number(endDate.day());
                        break;
                    case 11:
                        nov = QString::number(endDate.day());
                        break;
                    case 12:
                        dec = QString::number(endDate.day());
                        break;
                    default:
                        break;
                    }

                    if (data_SI.at(i).at(10).split("@").at(0) != date_dop){
                        continue;
                    }

                    stroka += 1;
                    documentContent +=
                        "<w:tr>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + QString::number(stroka) + "</w:t></w:r></w:p></w:tc>"
                                                    "<w:tc><w:tcPr><w:tcW w:w=\"1009\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + data_SI.at(i).at(1) + ",<w:br/>" + data_SI.at(i).at(2) + ",<w:br/>зав. № " + data_SI.at(i).at(4) + "<w:br/>" + data_SI.at(i).at(0) + "</w:t></w:r></w:p></w:tc>"
                                                                                                                             "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + diapazony + "</w:t></w:r></w:p></w:tc>"
                                      "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + pogreshnosty + "</w:t></w:r></w:p></w:tc>"
                                         "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>1</w:t></w:r></w:p></w:tc>"

                                         "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + QString::number(totalMonths) + "</w:t></w:r></w:p></w:tc>"
                                                         "<w:tc><w:tcPr><w:tcW w:w=\"758\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + pov_list.at(0) + "</w:t></w:r></w:p></w:tc>"
                                           "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + jan + "</w:t></w:r></w:p></w:tc>"
                                "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + feb + "</w:t></w:r></w:p></w:tc>"
                                "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + mar + "</w:t></w:r></w:p></w:tc>"

                                "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + apr + "</w:t></w:r></w:p></w:tc>"
                                "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + may + "</w:t></w:r></w:p></w:tc>"
                                "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + jun + "</w:t></w:r></w:p></w:tc>"
                                "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + jul + "</w:t></w:r></w:p></w:tc>"
                                "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + aug + "</w:t></w:r></w:p></w:tc>"

                                "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + sep + "</w:t></w:r></w:p></w:tc>"
                                "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + oct + "</w:t></w:r></w:p></w:tc>"
                                "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + nov + "</w:t></w:r></w:p></w:tc>"
                                "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + dec + "</w:t></w:r></w:p></w:tc>"
                                "<w:tc><w:tcPr><w:tcW w:w=\"1062\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                                "</w:tr>";
                }

                documentContent +=
                    "</w:tbl>"
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"0\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"0\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t>Составлено:<w:br/>Ответственный за метрологическое обеспечение</w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
                    "<w:tbl>"

                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"4677\"/>"
                    "<w:gridCol w:w=\"4678\"/>"
                    "</w:tblGrid>" // 9355

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>(инженер-метролог)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>" + current_metr + "</w:t></w:r></w:p></w:tc>"
                                              "</w:tr>"

                                              "<w:tr>"
                                              "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                                              "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                                              "</w:tr>"
                                              "</w:tbl>";

                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"0\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t>Согласовано:</w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
                    "<w:tbl>"

                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"4677\"/>"
                    "<w:gridCol w:w=\"4678\"/>"
                    "</w:tblGrid>" // 9355

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>Руководитель " + info.at(0) + "</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>" + current_ruk + "</w:t></w:r></w:p></w:tc>"
                                             "</w:tr>"

                                             "<w:tr>"
                                             "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                                             "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                                             "</w:tr>"
                                             "</w:tbl>";

                documentContent +=
                    "<w:sectPr>"
                    "<w:pgSz w:w=\"11906\" w:h=\"16838\"/>"
                    "<w:pgMar w:top=\"1134\" w:right=\"850\" w:bottom=\"1134\" w:left=\"1701\" w:header=\"708\" w:footer=\"708\" w:gutter=\"0\"/>"
                    "<w:cols w:space=\"720\"/>"
                    "<w:docGrid w:linePitch=\"360\"/>"
                    "</w:sectPr>"
                    "</w:body>"
                    "</w:document>";

                return documentContent.toUtf8();
            }()
        },

        // word/_rels/document.xml.rels
        {
            "word/_rels/document.xml.rels",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "</Relationships>"
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

void Dates::create_graf_att(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return;

    QDataStream zip(&file);
    zip.setByteOrder(QDataStream::LittleEndian);

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
                                          "<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">"
                                          "<w:body>";
                QString lab = info.at(0);
                lab = lab.replace("ИЛ", "");
                lab = lab.replace("ПБ", "");
                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:jc w:val=\"right\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t>УТВЕРЖДАЮ<w:br/>Генеральный директор<w:br/>" + lab + "<w:br/>______________" + current_gen + "<w:br/>«___» _________ 20___ г.</w:t>"
                                                                    "</w:r>"
                                                                    "</w:p>";
                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:jc w:val=\"center\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"20\"/>"
                    "<w:szCs w:val=\"20\"/>"
                    "<w:b/>"
                    "</w:rPr>"
                    "<w:t>ГРАФИК<w:br/>периодической аттестации<w:br/>испытательного обрудования на " + QString::number(year) + " год</w:t>"
                                              "</w:r>"
                                              "</w:p>";

                documentContent +=
                    "<w:tbl>"
                    "<w:tblPr>"

                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
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
                    "<w:gridCol w:w=\"279\"/>"
                    "<w:gridCol w:w=\"2568\"/>"
                    // "<w:gridCol w:w=\"850\"/>"
                    // "<w:gridCol w:w=\"709\"/>"
                    "<w:gridCol w:w=\"567\"/>"

                    "<w:gridCol w:w=\"910\"/>"
                    "<w:gridCol w:w=\"959\"/>"
                    "<w:gridCol w:w=\"330\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"

                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"330\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"

                    "<w:gridCol w:w=\"330\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"660\"/>"
                    "</w:tblGrid>"



                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>№ п/п</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"2568\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Наименование оборудования, заводской № (инвентарный №)</w:t></w:r></w:p></w:tc>"
                    // "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Диапазон измерений</w:t></w:r></w:p></w:tc>"
                    // "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Разряд, класс точности</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Общее кол-во</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"910\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Периодичность аттестации<w:br/>в месяцах</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"959\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-40\" w:right=\"-40\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Дата последней аттестации</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"3969\" w:type=\"dxa\"/><w:gridSpan w:val=\"12\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Сроки проведения аттестации</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"660\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Примечание</w:t></w:r></w:p></w:tc>"
                    "</w:tr>"


                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"2568\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    // "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    // "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"910\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"959\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"992\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>I кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"992\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>II кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"992\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>III кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"993\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>IV кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"660\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>"



                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"2568\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    // "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    // "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"910\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"959\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>I</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>II</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>III</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>IV</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>V</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>VI</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>VII</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>VIII</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>IX</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>X</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>XI</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>XII</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"660\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>";
                int stroka = 0;
                QString arhDate_string = "01.01.";
                arhDate_string += QString::number(year - 1);
                QDate arhDate = QDate::fromString(arhDate_string, "dd.MM.yyyy");
                for (int i = 0; i < data_IO.size(); ++i) {
                    QString pov;
                    QStringList pov_list;
                    QStringList all_pov_list = data_IO.at(i).at(9).split("@");
                    pov = all_pov_list.last();
                    pov_list = pov.split(";");
                    bool found = false;

                    QDate startDate;
                    QDate endDate;
                    int totalMonths;
                    for (int j = all_pov_list.size()-1; j >= 0; j--) {
                        QStringList pov_parts = all_pov_list.at(j).split(";");
                        startDate = QDate::fromString(pov_parts.at(0), "dd.MM.yyyy");
                        endDate = QDate::fromString(pov_parts.at(3), "dd.MM.yyyy");
                        int years = endDate.year() - startDate.year();
                        int months = endDate.month() - startDate.month();
                        if (months % 6 != 0){
                            months += 1;
                        }
                        totalMonths = years * 12 + months;

                        if (startDate <= arhDate && arhDate <= endDate) {
                            int pov_count = 0;
                            if (endDate.year() < year){
                                while (endDate.year() < year) {
                                    endDate = endDate.addMonths(totalMonths);
                                    pov_count += 1;
                                }
                            }
                            int days = (-1 * pov_count);
                            endDate = endDate.addDays(days);
                            pov_list = pov_parts;
                            found = true;
                            if (endDate.year() != year) {
                                found = false;
                            }
                            break;
                        }
                    }
                    if (!found) {
                        continue;
                    }

                    QString date_SI = data_IO.at(i).at(7);
                    QStringList date_SI_List = date_SI.split("@");
                    found = false;

                    if (date_SI_List.size() > 1){
                        QDate startDate = QDate::fromString(date_SI_List.at(0), "dd.MM.yyyy");
                        QDate endDate = QDate::fromString(date_SI_List.at(1), "dd.MM.yyyy");
                        if (startDate <= arhDate && arhDate <= endDate) {
                            date_SI = date_SI_List.at(0).split(".").at(2);
                            found = true;
                        }
                    } else {
                        QDate startDate = QDate::fromString(date_SI_List.at(0), "dd.MM.yyyy");
                        if (startDate <= arhDate) {
                            date_SI = date_SI_List.at(0).split(".").at(2);
                            found = true;
                        }
                    }
                    if (!found) {
                        continue;
                    }

                    QString jan;
                    QString feb;
                    QString mar;
                    QString apr;
                    QString may;
                    QString jun;
                    QString jul;
                    QString aug;
                    QString sep;
                    QString oct;
                    QString nov;
                    QString dec;

                    switch (endDate.month()) {
                    case 1:
                        jan = QString::number(endDate.day());
                        break;
                    case 2:
                        feb = QString::number(endDate.day());
                        break;
                    case 3:
                        mar = QString::number(endDate.day());
                        break;
                    case 4:
                        apr = QString::number(endDate.day());
                        break;
                    case 5:
                        may = QString::number(endDate.day());
                        break;
                    case 6:
                        jun = QString::number(endDate.day());
                        break;
                    case 7:
                        jul = QString::number(endDate.day());
                        break;
                    case 8:
                        aug = QString::number(endDate.day());
                        break;
                    case 9:
                        sep = QString::number(endDate.day());
                        break;
                    case 10:
                        oct = QString::number(endDate.day());
                        break;
                    case 11:
                        nov = QString::number(endDate.day());
                        break;
                    case 12:
                        dec = QString::number(endDate.day());
                        break;
                    default:
                        break;
                    }

                    stroka += 1;
                    documentContent +=
                        "<w:tr>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + QString::number(stroka) + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"2568\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + data_IO.at(i).at(2) + ",<w:br/>зав. № " + data_IO.at(i).at(3) + ",<w:br/>(инв. № " + data_IO.at(i).at(0) + ")</w:t></w:r></w:p></w:tc>"
                        // "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + diapazony + "</w:t></w:r></w:p></w:tc>"
                        // "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + pogreshnosty + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>1</w:t></w:r></w:p></w:tc>"

                        "<w:tc><w:tcPr><w:tcW w:w=\"910\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + QString::number(totalMonths) + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"959\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + pov_list.at(0) + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + jan + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + feb + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + mar + "</w:t></w:r></w:p></w:tc>"

                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + apr + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + may + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + jun + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + jul + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + aug + "</w:t></w:r></w:p></w:tc>"

                        "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + sep + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + oct + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + nov + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + dec + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"660\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                        "</w:tr>";
                }

                documentContent +=
                    "</w:tbl>"
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"0\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"0\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t>Составлено:<w:br/>Ответственный за метрологическое обеспечение</w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
                    "<w:tbl>"

                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"4677\"/>"
                    "<w:gridCol w:w=\"4678\"/>"
                    "</w:tblGrid>" // 9355

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>(инженер-метролог)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>" + current_metr + "</w:t></w:r></w:p></w:tc>"
                    "</w:tr>"

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>"
                    "</w:tbl>";

                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"0\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t>Согласовано:</w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
                    "<w:tbl>"

                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"4677\"/>"
                    "<w:gridCol w:w=\"4678\"/>"
                    "</w:tblGrid>" // 9355

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>Руководитель " + info.at(0) + "</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>" + current_ruk + "</w:t></w:r></w:p></w:tc>"
                    "</w:tr>"

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>"
                    "</w:tbl>";

                documentContent +=
                    "<w:sectPr>"
                    "<w:pgSz w:w=\"11906\" w:h=\"16838\"/>"
                    "<w:pgMar w:top=\"1134\" w:right=\"850\" w:bottom=\"1134\" w:left=\"1701\" w:header=\"708\" w:footer=\"708\" w:gutter=\"0\"/>"
                    "<w:cols w:space=\"720\"/>"
                    "<w:docGrid w:linePitch=\"360\"/>"
                    "</w:sectPr>"
                    "</w:body>"
                    "</w:document>";

                return documentContent.toUtf8();
            }()
        },

        // word/_rels/document.xml.rels
        {
            "word/_rels/document.xml.rels",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "</Relationships>"
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

void Dates::create_graf_att_dop(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return;

    QDataStream zip(&file);
    zip.setByteOrder(QDataStream::LittleEndian);
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
                                          "<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">"
                                          "<w:body>";
                QString lab = info.at(0);
                lab = lab.replace("ИЛ", "");
                lab = lab.replace("ПБ", "");
                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:jc w:val=\"right\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t>УТВЕРЖДАЮ<w:br/>Генеральный директор<w:br/>" + lab + "<w:br/>______________" + current_gen + "<w:br/>«___» _________ 20___ г.</w:t>"
                                                                    "</w:r>"
                                                                    "</w:p>";
                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:jc w:val=\"center\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"20\"/>"
                    "<w:szCs w:val=\"20\"/>"
                    "<w:b/>"
                    "</w:rPr>"
                    "<w:t>Дополнение № " + QString::number(dop_num) + "<w:br/>к графику периодической аттестации<w:br/>испытательного оборудования на " + QString::number(year) + " год</w:t>"
                                                                                                                                                "</w:r>"
                                                                                                                                                "</w:p>";

                documentContent +=
                    "<w:tbl>"
                    "<w:tblPr>"

                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
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
                    "<w:gridCol w:w=\"279\"/>"
                    "<w:gridCol w:w=\"2568\"/>"
                    // "<w:gridCol w:w=\"850\"/>"
                    // "<w:gridCol w:w=\"709\"/>"
                    "<w:gridCol w:w=\"567\"/>"

                    "<w:gridCol w:w=\"910\"/>"
                    "<w:gridCol w:w=\"959\"/>"
                    "<w:gridCol w:w=\"330\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"

                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"330\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"

                    "<w:gridCol w:w=\"330\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"331\"/>"
                    "<w:gridCol w:w=\"660\"/>"
                    "</w:tblGrid>"



                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>№ п/п</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"2568\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Наименование оборудования, заводской № (инвентарный №)</w:t></w:r></w:p></w:tc>"
                    // "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Диапазон измерений</w:t></w:r></w:p></w:tc>"
                    // "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Разряд, класс точности</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Общее кол-во</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"910\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Периодичность аттестации<w:br/>в месяцах</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"959\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-40\" w:right=\"-40\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Дата последней аттестации</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"3969\" w:type=\"dxa\"/><w:gridSpan w:val=\"12\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Сроки проведения аттестации</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"660\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>Примечание</w:t></w:r></w:p></w:tc>"
                    "</w:tr>"


                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"2568\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    // "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    // "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"910\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"959\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"992\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>I кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"992\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>II кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"992\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>III кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"993\" w:type=\"dxa\"/><w:gridSpan w:val=\"3\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>IV кв.</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"660\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>"



                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"2568\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    // "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    // "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"910\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"959\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>I</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>II</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>III</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>IV</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>V</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>VI</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>VII</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>VIII</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>IX</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>X</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>XI</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>XII</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"660\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>";
                int stroka = 0;
                QString arhDate_string = "01.01.";
                arhDate_string += QString::number(year);
                QDate arhDate = QDate::fromString(arhDate_string, "dd.MM.yyyy");
                for (int i = 0; i < data_IO.size(); ++i) {

                    QString pov;
                    QStringList pov_list;
                    QStringList all_pov_list = data_IO.at(i).at(9).split("@");
                    pov = all_pov_list.last();
                    pov_list = pov.split(";");
                    bool found = false;

                    QDate startDate;
                    QDate endDate;
                    int totalMonths;
                    for (int j = all_pov_list.size()-1; j >= 0; j--) {
                        QStringList pov_parts = all_pov_list.at(j).split(";");
                        startDate = QDate::fromString(pov_parts.at(0), "dd.MM.yyyy");
                        endDate = QDate::fromString(pov_parts.at(3), "dd.MM.yyyy");
                        int years = endDate.year() - startDate.year();
                        int months = endDate.month() - startDate.month();
                        if (months % 6 != 0){
                            months += 1;
                        }
                        totalMonths = years * 12 + months;

                        if (startDate <= arhDate && arhDate <= endDate) {
                            int pov_count = 0;
                            if (endDate.year() < year){
                                while (endDate.year() < year) {
                                    endDate = endDate.addMonths(totalMonths);
                                    pov_count += 1;
                                }
                            }
                            int days = (-1 * pov_count);
                            endDate = endDate.addDays(days);
                            pov_list = pov_parts;
                            found = true;
                            if (endDate.year() != year) {
                                found = false;
                            }
                            break;
                        }
                    }
                    if (!found) {
                        continue;
                    }

                    QString date_SI = data_IO.at(i).at(7);
                    QStringList date_SI_List = date_SI.split("@");
                    found = false;

                    if (date_SI_List.size() > 1){
                        QDate startDate = QDate::fromString(date_SI_List.at(0), "dd.MM.yyyy");
                        QDate endDate = QDate::fromString(date_SI_List.at(1), "dd.MM.yyyy");
                        if (startDate <= arhDate && arhDate <= endDate) {
                            date_SI = date_SI_List.at(0).split(".").at(2);
                            found = true;
                        }
                    } else {
                        QDate startDate = QDate::fromString(date_SI_List.at(0), "dd.MM.yyyy");
                        if (startDate <= arhDate) {
                            date_SI = date_SI_List.at(0).split(".").at(2);
                            found = true;
                        }
                    }
                    if (!found) {
                        continue;
                    }

                    QString jan;
                    QString feb;
                    QString mar;
                    QString apr;
                    QString may;
                    QString jun;
                    QString jul;
                    QString aug;
                    QString sep;
                    QString oct;
                    QString nov;
                    QString dec;

                    switch (endDate.month()) {
                    case 1:
                        jan = QString::number(endDate.day());
                        break;
                    case 2:
                        feb = QString::number(endDate.day());
                        break;
                    case 3:
                        mar = QString::number(endDate.day());
                        break;
                    case 4:
                        apr = QString::number(endDate.day());
                        break;
                    case 5:
                        may = QString::number(endDate.day());
                        break;
                    case 6:
                        jun = QString::number(endDate.day());
                        break;
                    case 7:
                        jul = QString::number(endDate.day());
                        break;
                    case 8:
                        aug = QString::number(endDate.day());
                        break;
                    case 9:
                        sep = QString::number(endDate.day());
                        break;
                    case 10:
                        oct = QString::number(endDate.day());
                        break;
                    case 11:
                        nov = QString::number(endDate.day());
                        break;
                    case 12:
                        dec = QString::number(endDate.day());
                        break;
                    default:
                        break;
                    }

                    if (data_IO.at(i).at(7).split("@").at(0) != date_dop){
                        continue;
                    }

                    stroka += 1;
                    documentContent +=
                        "<w:tr>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"279\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + QString::number(stroka) + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"2568\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + data_IO.at(i).at(2) + ",<w:br/>зав. № " + data_IO.at(i).at(3) + "<w:br/>(инв. № " + data_IO.at(i).at(0) + ")</w:t></w:r></w:p></w:tc>"
                        // "<w:tc><w:tcPr><w:tcW w:w=\"850\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + diapazony + "</w:t></w:r></w:p></w:tc>"
                        // "<w:tc><w:tcPr><w:tcW w:w=\"709\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + pogreshnosty + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>1</w:t></w:r></w:p></w:tc>"

                        "<w:tc><w:tcPr><w:tcW w:w=\"910\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + QString::number(totalMonths) + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"959\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + pov_list.at(0) + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + jan + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + feb + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + mar + "</w:t></w:r></w:p></w:tc>"

                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + apr + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + may + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + jun + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + jul + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + aug + "</w:t></w:r></w:p></w:tc>"

                        "<w:tc><w:tcPr><w:tcW w:w=\"330\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + sep + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + oct + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + nov + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"331\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t>" + dec + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"660\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"16\"/><w:szCs w:val=\"16\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                        "</w:tr>";
                }

                documentContent +=
                    "</w:tbl>"
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"0\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t></w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"0\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t>Составлено:<w:br/>Ответственный за метрологическое обеспечение</w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
                    "<w:tbl>"

                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"4677\"/>"
                    "<w:gridCol w:w=\"4678\"/>"
                    "</w:tblGrid>" // 9355

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>(инженер-метролог)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>" + current_metr + "</w:t></w:r></w:p></w:tc>"
                    "</w:tr>"

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>"
                    "</w:tbl>";

                documentContent +=
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"0\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"22\"/>"
                    "<w:szCs w:val=\"22\"/>"
                    "</w:rPr>"
                    "<w:t>Согласовано:</w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
                    "<w:tbl>"

                    "<w:tblPr>"
                    "<w:tblW w:w=\"0\" w:type=\"auto\"/>"
                    "<w:tblInd w:w=\"10\" w:type=\"dxa\"/>"
                    "<w:tblLayout w:type=\"fixed\"/>"
                    "</w:tblPr>"

                    "<w:tblGrid>"
                    "<w:gridCol w:w=\"4677\"/>"
                    "<w:gridCol w:w=\"4678\"/>"
                    "</w:tblGrid>" // 9355

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>Руководитель " + info.at(0) + "</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t>" + current_ruk + "</w:t></w:r></w:p></w:tc>"
                    "</w:tr>"

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4677\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4678\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/><w:szCs w:val=\"22\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "</w:tr>"
                    "</w:tbl>";

                documentContent +=
                    "<w:sectPr>"
                    "<w:pgSz w:w=\"11906\" w:h=\"16838\"/>"
                    "<w:pgMar w:top=\"1134\" w:right=\"850\" w:bottom=\"1134\" w:left=\"1701\" w:header=\"708\" w:footer=\"708\" w:gutter=\"0\"/>"
                    "<w:cols w:space=\"720\"/>"
                    "<w:docGrid w:linePitch=\"360\"/>"
                    "</w:sectPr>"
                    "</w:body>"
                    "</w:document>";

                return documentContent.toUtf8();
            }()
        },

        // word/_rels/document.xml.rels
        {
            "word/_rels/document.xml.rels",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "</Relationships>"
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

void Dates::fill_layout(const QVector<QDate> &dates_in_month,
                        const QStringList &inv_in_month,
                        const QStringList &names_in_month,
                        QList<QHBoxLayout*> &layouts_List){

    if (layouts_List.isEmpty()){
        for (int x = 0; x < dates_in_month.size(); x++){

            QHBoxLayout *hLayout = new QHBoxLayout();

            QLabel *label_inv = new QLabel();
            QLabel *label_name = new QLabel();
            QLabel *label_date = new QLabel();

            QString inv_num = inv_in_month.at(x);
            QString naim = names_in_month.at(x);
            QDate date_pov = dates_in_month.at(x);

            label_inv->setText(inv_num);
            label_inv->setMaximumWidth(45);
            label_name->setText(naim);
            label_name->setMaximumWidth(200);
            label_name->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            label_date->setText(date_pov.toString("dd.MM.yyyy"));
            label_date->setMaximumWidth(60);

            hLayout->addWidget(label_inv);
            hLayout->addWidget(label_name);
            hLayout->addWidget(label_date);

            if (!layouts_List.isEmpty()) {
                int numlayouts_pov_List = layouts_List.size();
                ui->verticalLayout_3->insertLayout(numlayouts_pov_List, hLayout);
            } else {
                ui->verticalLayout_3->insertLayout(0, hLayout);
            }

            layouts_List.append(hLayout);
        }
    }
}

void Dates::clean_layout(QList<QHBoxLayout*> &layouts_List){
    if (!layouts_List.isEmpty()) {
        while (!layouts_List.isEmpty()){
            QHBoxLayout *hLayout = layouts_List.takeLast();
            QLayoutItem *item;

            while ((item = hLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            ui->verticalLayout_3->removeItem(hLayout);
            delete hLayout;
        }
    }
}

void Dates::on_pushButton_clicked()
{
    if (!this->datesatt.isEmpty()){
        QColor color_default("white");
        QTextCharFormat format_default;
        format_default.setBackground(color_default);
        for (int x = 0; x < this->datesatt.size(); x++){
            if (existsIO.at(x)){
                ui->calendarWidget_jan->setDateTextFormat(this->datesatt.at(x), format_default);
                ui->calendarWidget_feb->setDateTextFormat(this->datesatt.at(x), format_default);
                ui->calendarWidget_mar->setDateTextFormat(this->datesatt.at(x), format_default);
                ui->calendarWidget_apr->setDateTextFormat(this->datesatt.at(x), format_default);
                ui->calendarWidget_may->setDateTextFormat(this->datesatt.at(x), format_default);
                ui->calendarWidget_jun->setDateTextFormat(this->datesatt.at(x), format_default);
                ui->calendarWidget_jul->setDateTextFormat(this->datesatt.at(x), format_default);
                ui->calendarWidget_aug->setDateTextFormat(this->datesatt.at(x), format_default);
                ui->calendarWidget_sep->setDateTextFormat(this->datesatt.at(x), format_default);
                ui->calendarWidget_oct->setDateTextFormat(this->datesatt.at(x), format_default);
                ui->calendarWidget_nov->setDateTextFormat(this->datesatt.at(x), format_default);
                ui->calendarWidget_dec->setDateTextFormat(this->datesatt.at(x), format_default);
            }
        }
    }

    QColor color_pov(255, 222, 173);
    QTextCharFormat format_pov;
    format_pov.setBackground(color_pov);
    for (int x = 0; x < this->datespov.size(); x++){
        if (existsSI.at(x)){
            ui->calendarWidget_jan->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_feb->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_mar->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_apr->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_may->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_jun->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_jul->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_aug->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_sep->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_oct->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_nov->setDateTextFormat(this->datespov.at(x), format_pov);
            ui->calendarWidget_dec->setDateTextFormat(this->datespov.at(x), format_pov);
        }
    }

    ui->calendarWidget_jan->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_feb->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_mar->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_apr->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_may->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_jun->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_jul->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_aug->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_sep->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_oct->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_nov->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_dec->setSelectedDate(QDate(1990, 1, 1));

    ui->label->setText("Поверок в пределах текущей даты + месяц:");
    ui->spinBox->setValue(this->datespov_in_month.size());

    clean_layout(layouts_att_List);
    fill_layout(datespov_in_month, inv_pov_in_month, names_pov_in_month, layouts_pov_List);
}

void Dates::on_pushButton_2_clicked()
{
    if (!this->datespov.isEmpty()){
        QColor color_default("white");
        QTextCharFormat format_default;
        format_default.setBackground(color_default);
        for (int x = 0; x < this->datespov.size(); x++){
            if (existsSI.at(x)){
                ui->calendarWidget_jan->setDateTextFormat(this->datespov.at(x), format_default);
                ui->calendarWidget_feb->setDateTextFormat(this->datespov.at(x), format_default);
                ui->calendarWidget_mar->setDateTextFormat(this->datespov.at(x), format_default);
                ui->calendarWidget_apr->setDateTextFormat(this->datespov.at(x), format_default);
                ui->calendarWidget_may->setDateTextFormat(this->datespov.at(x), format_default);
                ui->calendarWidget_jun->setDateTextFormat(this->datespov.at(x), format_default);
                ui->calendarWidget_jul->setDateTextFormat(this->datespov.at(x), format_default);
                ui->calendarWidget_aug->setDateTextFormat(this->datespov.at(x), format_default);
                ui->calendarWidget_sep->setDateTextFormat(this->datespov.at(x), format_default);
                ui->calendarWidget_oct->setDateTextFormat(this->datespov.at(x), format_default);
                ui->calendarWidget_nov->setDateTextFormat(this->datespov.at(x), format_default);
                ui->calendarWidget_dec->setDateTextFormat(this->datespov.at(x), format_default);
            }
        }
    }

    QColor color_pov(255, 222, 173);
    QTextCharFormat format_pov;
    format_pov.setBackground(color_pov);
    for (int x = 0; x < this->datesatt.size(); x++){
        if (existsIO.at(x)){
            ui->calendarWidget_jan->setDateTextFormat(this->datesatt.at(x), format_pov);
            ui->calendarWidget_feb->setDateTextFormat(this->datesatt.at(x), format_pov);
            ui->calendarWidget_mar->setDateTextFormat(this->datesatt.at(x), format_pov);
            ui->calendarWidget_apr->setDateTextFormat(this->datesatt.at(x), format_pov);
            ui->calendarWidget_may->setDateTextFormat(this->datesatt.at(x), format_pov);
            ui->calendarWidget_jun->setDateTextFormat(this->datesatt.at(x), format_pov);
            ui->calendarWidget_jul->setDateTextFormat(this->datesatt.at(x), format_pov);
            ui->calendarWidget_aug->setDateTextFormat(this->datesatt.at(x), format_pov);
            ui->calendarWidget_sep->setDateTextFormat(this->datesatt.at(x), format_pov);
            ui->calendarWidget_oct->setDateTextFormat(this->datesatt.at(x), format_pov);
            ui->calendarWidget_nov->setDateTextFormat(this->datesatt.at(x), format_pov);
            ui->calendarWidget_dec->setDateTextFormat(this->datesatt.at(x), format_pov);
        }
    }
    ui->calendarWidget_jan->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_feb->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_mar->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_apr->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_may->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_jun->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_jul->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_aug->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_sep->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_oct->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_nov->setSelectedDate(QDate(1990, 1, 1));
    ui->calendarWidget_dec->setSelectedDate(QDate(1990, 1, 1));

    ui->label->setText("Аттестаций в пределах текущей даты + месяц:");
    ui->spinBox->setValue(this->datesatt_in_month.size());

    clean_layout(layouts_pov_List);
    fill_layout(datesatt_in_month, inv_att_in_month, names_att_in_month, layouts_att_List);
}

void Dates::on_pushButton_to_file_clicked(){}

void Dates::on_dateEdit_year_userDateChanged(const QDate &date)
{
    year = date.year();

    ui->calendarWidget_jan->setMinimumDate(QDate(year, 1, 1));
    ui->calendarWidget_jan->setMaximumDate(QDate(year, 1, 31));

    int daysInfeb = QDate(year, 2, 1).daysInMonth();
    ui->calendarWidget_feb->setMinimumDate(QDate(year, 2, 1));
    ui->calendarWidget_feb->setMaximumDate(QDate(year, 2, daysInfeb));

    ui->calendarWidget_mar->setMinimumDate(QDate(year, 3, 1));
    ui->calendarWidget_mar->setMaximumDate(QDate(year, 3, 31));

    ui->calendarWidget_apr->setMinimumDate(QDate(year, 4, 1));
    ui->calendarWidget_apr->setMaximumDate(QDate(year, 4, 30));

    ui->calendarWidget_may->setMinimumDate(QDate(year, 5, 1));
    ui->calendarWidget_may->setMaximumDate(QDate(year, 5, 31));

    ui->calendarWidget_jun->setMinimumDate(QDate(year, 6, 1));
    ui->calendarWidget_jun->setMaximumDate(QDate(year, 6, 30));

    ui->calendarWidget_jul->setMinimumDate(QDate(year, 7, 1));
    ui->calendarWidget_jul->setMaximumDate(QDate(year, 7, 31));

    ui->calendarWidget_aug->setMinimumDate(QDate(year, 8, 1));
    ui->calendarWidget_aug->setMaximumDate(QDate(year, 8, 31));

    ui->calendarWidget_sep->setMinimumDate(QDate(year, 9, 1));
    ui->calendarWidget_sep->setMaximumDate(QDate(year, 9, 30));

    ui->calendarWidget_oct->setMinimumDate(QDate(year, 10, 1));
    ui->calendarWidget_oct->setMaximumDate(QDate(year, 10, 31));

    ui->calendarWidget_nov->setMinimumDate(QDate(year, 11, 1));
    ui->calendarWidget_nov->setMaximumDate(QDate(year, 11, 30));

    ui->calendarWidget_dec->setMinimumDate(QDate(year, 12, 1));
    ui->calendarWidget_dec->setMaximumDate(QDate(year, 12, 31));
}

void Dates::on_pushButton_graf_pov_clicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку для выгрузки графика",
        "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );
    if (folderPath != ""){
        QString arhDate_string = "01.01.";
        arhDate_string += QString::number(year - 1);
        QDate arhDate = QDate::fromString(arhDate_string, "dd.MM.yyyy");
        current_gen = getEmployeeOnPost(gen, arhDate);
        current_ruk = getEmployeeOnPost(ruk, arhDate);
        current_metr = getEmployeeOnPost(metr, arhDate);

        if (ui->pushButton->isChecked()) {
            create_graf_pov(folderPath + "/График поверки " + QString::number(year) + ".docx");

            dop_num = 0;
            QStringList dates_vvod;
    
            arhDate_string = "01.01.";
            arhDate_string += QString::number(year);
            arhDate = QDate::fromString(arhDate_string, "dd.MM.yyyy");
    
            for (int i = 0; i < data_SI.size(); i++) {
                if (data_SI.at(i).at(12) == "-") {
                    continue;
                }
                QString pov;
                QStringList pov_list;
                QStringList all_pov_list = data_SI.at(i).at(12).split("@");
                pov = all_pov_list.last();
                pov_list = pov.split(";");
                bool found = false;
    
                QDate startDate;
                QDate endDate;
                int totalMonths;
                for (int j = all_pov_list.size()-1; j >= 0; j--) {
                    QStringList pov_parts = all_pov_list.at(j).split(";");
                    startDate = QDate::fromString(pov_parts.at(0), "dd.MM.yyyy");
                    endDate = QDate::fromString(pov_parts.at(3), "dd.MM.yyyy");
                    int years = endDate.year() - startDate.year();
                    int months = endDate.month() - startDate.month();
                    if (months % 6 != 0){
                        months += 1;
                    }
                    totalMonths = years * 12 + months;
    
                    if (startDate <= arhDate && arhDate <= endDate) {
                        int pov_count = 0;
                        if (endDate.year() < year){
                            while (endDate.year() < year) {
                                endDate = endDate.addMonths(totalMonths);
                                pov_count += 1;
                            }
                        }
                        int days = (-1 * pov_count);
                        endDate = endDate.addDays(days);
                        pov_list = pov_parts;
                        found = true;
                        if (endDate.year() != year) {
                            found = false;
                        }
                        break;
                    }
                }
                if (!found) {
                    continue;
                }
    
                QString date_SI = data_SI.at(i).at(10);
                QStringList date_SI_List = date_SI.split("@");
                found = false;
    
                if (date_SI_List.size() > 1){
                    QDate startDate = QDate::fromString(date_SI_List.at(0), "dd.MM.yyyy");
                    QDate endDate = QDate::fromString(date_SI_List.at(1), "dd.MM.yyyy");
                    if (startDate <= arhDate && arhDate <= endDate) {
                        date_SI = date_SI_List.at(0).split(".").at(2);
                        found = true;
                    }
                } else {
                    QDate startDate = QDate::fromString(date_SI_List.at(0), "dd.MM.yyyy");
                    if (startDate <= arhDate) {
                        date_SI = date_SI_List.at(0).split(".").at(2);
                        found = true;
                    }
                }
                if (!found) {
                    continue;
                }
    
                dates_vvod.append(data_SI.at(i).at(10));
            }
    
            QSet<QString> uniqueStrings;
            QStringList resultList;
    
            for (const QString& one_pokazatel : qAsConst(dates_vvod)) {
                if (!uniqueStrings.contains(one_pokazatel.split("@").at(0))) {
                    uniqueStrings.insert(one_pokazatel.split("@").at(0));
                    resultList.append(one_pokazatel.split("@").at(0));
                }
            }
            qDebug() << resultList;
    
            QString start = "01.01.";
            start += QString::number(year - 1);
            QString end = "01.01.";
            end += QString::number(year);
    
            for (int i = 0; i < resultList.size(); i++){
                bool found = false;
                QDate arhDate = QDate::fromString(resultList.at(i), "dd.MM.yyyy");
    
                QDate startDate = QDate::fromString(start, "dd.MM.yyyy");
                QDate endDate = QDate::fromString(end, "dd.MM.yyyy");
                if (startDate <= arhDate && arhDate <= endDate) {
                    found = true;
                }
                if (!found) {
                    continue;
                }
    
                dop_num += 1;
                date_dop = resultList.at(i);
                current_gen = getEmployeeOnPost(gen, arhDate);
                current_ruk = getEmployeeOnPost(ruk, arhDate);
                current_metr = getEmployeeOnPost(metr, arhDate);
                qDebug() << arhDate;
                create_graf_pov_dop(folderPath + "/Доп. № " + QString::number(dop_num) + " к графику поверки " + QString::number(year) + " " + date_dop + " .docx");
            }
    
            qDebug() << "Данные СИ успешно сохранены в файл.";
        } else {
            create_graf_att(folderPath + "/График аттестации " + QString::number(year) + ".docx");

            dop_num = 0;
            QStringList dates_vvod;
    
            arhDate_string = "01.01.";
            arhDate_string += QString::number(year);
            arhDate = QDate::fromString(arhDate_string, "dd.MM.yyyy");
    
            for (int i = 0; i < data_IO.size(); i++) {
                if (data_IO.at(i).at(9) == "-") {
                    continue;
                }
                QString pov;
                QStringList pov_list;
                QStringList all_pov_list = data_IO.at(i).at(9).split("@");
                pov = all_pov_list.last();
                pov_list = pov.split(";");
                bool found = false;
    
                QDate startDate;
                QDate endDate;
                int totalMonths;
                for (int j = all_pov_list.size()-1; j >= 0; j--) {
                    QStringList pov_parts = all_pov_list.at(j).split(";");
                    startDate = QDate::fromString(pov_parts.at(0), "dd.MM.yyyy");
                    endDate = QDate::fromString(pov_parts.at(3), "dd.MM.yyyy");
                    int years = endDate.year() - startDate.year();
                    int months = endDate.month() - startDate.month();
                    if (months % 6 != 0){
                        months += 1;
                    }
                    totalMonths = years * 12 + months;
    
                    if (startDate <= arhDate && arhDate <= endDate) {
                        int pov_count = 0;
                        if (endDate.year() < year){
                            while (endDate.year() < year) {
                                endDate = endDate.addMonths(totalMonths);
                                pov_count += 1;
                            }
                        }
                        int days = (-1 * pov_count);
                        endDate = endDate.addDays(days);
                        pov_list = pov_parts;
                        found = true;
                        if (endDate.year() != year) {
                            found = false;
                        }
                        break;
                    }
                }
                if (!found) {
                    continue;
                }
    
                QString date_SI = data_IO.at(i).at(7);
                QStringList date_SI_List = date_SI.split("@");
                found = false;
    
                if (date_SI_List.size() > 1){
                    QDate startDate = QDate::fromString(date_SI_List.at(0), "dd.MM.yyyy");
                    QDate endDate = QDate::fromString(date_SI_List.at(1), "dd.MM.yyyy");
                    if (startDate <= arhDate && arhDate <= endDate) {
                        date_SI = date_SI_List.at(0).split(".").at(2);
                        found = true;
                    }
                } else {
                    QDate startDate = QDate::fromString(date_SI_List.at(0), "dd.MM.yyyy");
                    if (startDate <= arhDate) {
                        date_SI = date_SI_List.at(0).split(".").at(2);
                        found = true;
                    }
                }
                if (!found) {
                    continue;
                }
    
                dates_vvod.append(data_IO.at(i).at(7));
            }
    
            QSet<QString> uniqueStrings;
            QStringList resultList;
    
            for (const QString& one_pokazatel : qAsConst(dates_vvod)) {
                if (!uniqueStrings.contains(one_pokazatel.split("@").at(0))) {
                    uniqueStrings.insert(one_pokazatel.split("@").at(0));
                    resultList.append(one_pokazatel.split("@").at(0));
                }
            }
            qDebug() << resultList;
    
            QString start = "01.01.";
            start += QString::number(year - 1);
            QString end = "01.01.";
            end += QString::number(year);
    
            for (int i = 0; i < resultList.size(); i++){
                bool found = false;
                QDate arhDate = QDate::fromString(resultList.at(i), "dd.MM.yyyy");
    
                QDate startDate = QDate::fromString(start, "dd.MM.yyyy");
                QDate endDate = QDate::fromString(end, "dd.MM.yyyy");
                if (startDate <= arhDate && arhDate <= endDate) {
                    found = true;
                }
                if (!found) {
                    continue;
                }
    
                dop_num += 1;
                date_dop = resultList.at(i);
                current_gen = getEmployeeOnPost(gen, arhDate);
                current_ruk = getEmployeeOnPost(ruk, arhDate);
                current_metr = getEmployeeOnPost(metr, arhDate);
                qDebug() << arhDate;
                create_graf_att_dop(folderPath + "/Доп. № " + QString::number(dop_num) + " к графику аттестации " + QString::number(year) + " " + date_dop + " .docx");
            }
    
            qDebug() << "Данные СИ успешно сохранены в файл.";
        }
    }
}

QString Dates::getEmployeeOnPost(const QString &data, const QDate &targetDate) {
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


quint32 Dates::calculate_crc32(const QByteArray &data) {
    quint32 crc = 0xffffffff;
    for (char c : data) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ static_cast<quint8>(c)) & 0xff];
    }
    return crc ^ 0xffffffff;
}

quint32 Dates::datetime_to_dos(const QDateTime &dt) {
    QDate date = dt.date();
    QTime time = dt.time();

    quint32 dos_time = (time.second()/2) | (time.minute() << 5) | (time.hour() << 11);
    quint32 dos_date = date.day() | (date.month() << 5) | ((date.year() - 1980) << 9);
    return (dos_date << 16) | dos_time;
}

quint32 Dates::write_local_file(QDataStream &zip, const QString &name, const QByteArray &content) {
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

void Dates::write_central_directory(QDataStream &zip,
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

Dates::~Dates()
{
    delete ui;
}
