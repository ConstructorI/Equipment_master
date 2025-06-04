//forms.cpp
#include "forms.h"
#include "ui_forms.h"
#include "database.h"

#include <QtSql>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSpacerItem>
#include <QTextEdit>
#include <QStandardItemModel>

#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QDateTime>
#include <QDataStream>
#include <QtEndian>
#include <QDoubleSpinBox>
#include <QRandomGenerator>
#include <QtMath>
#include <QFileDialog>
#include <QDir>


const quint32 forms::crc32_table[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59
};


forms::forms(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::forms)
{
    ui->setupUi(this);
    Database db2;
    db2.openDatabase("database.db");
    data_SI = db2.get_data_SI();
    data_IO = db2.get_data_IO();
    data_VO = db2.get_data_VO();
    ruk = db2.get_data_ruk();
    info = db2.get_data_info();
    db2.close_conn();
    ui->dateEdit->setDate(QDate::currentDate());
    targetDate = QDate::currentDate().toString("dd.MM.yyyy");
}

forms::~forms()
{
    delete ui;
}

quint32 forms::calculate_crc32(const QByteArray &data) {
    quint32 crc = 0xffffffff;
    for (char c : data) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ static_cast<quint8>(c)) & 0xff];
    }
    return crc ^ 0xffffffff;
}

quint32 forms::datetime_to_dos(const QDateTime &dt) {
    QDate date = dt.date();
    QTime time = dt.time();

    quint32 dos_time = (time.second()/2) | (time.minute() << 5) | (time.hour() << 11);
    quint32 dos_date = date.day() | (date.month() << 5) | ((date.year() - 1980) << 9);
    return (dos_date << 16) | dos_time;
}

quint32 forms::write_local_file(QDataStream &zip,
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

void forms::write_central_directory(QDataStream &zip,
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

void forms::create_arh_form_SI_Docx(const QString &fileName) {

    qDebug() << data_SI.size();
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
            "<Override PartName=\"/word/header1.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.header+xml\"/>"
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
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"240\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"20\"/>"
                    "<w:szCs w:val=\"20\"/>"
                    "</w:rPr>"
                    "<w:t>Форма 2. Оснащенность ИЛ средствами измерений (СИ)</w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
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
                    "<w:gridCol w:w=\"567\"/>"
                    "<w:gridCol w:w=\"1271\"/>"
                    "<w:gridCol w:w=\"1847\"/>"
                    "<w:gridCol w:w=\"1418\"/>"
                    "<w:gridCol w:w=\"1843\"/>"

                    "<w:gridCol w:w=\"1558\"/>"
                    "<w:gridCol w:w=\"1135\"/>"
                    "<w:gridCol w:w=\"2828\"/>"
                    "<w:gridCol w:w=\"1283\"/>"
                    "<w:gridCol w:w=\"1134\"/>"

                    "</w:tblGrid>"


                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>N п/п</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1271\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Наименование определяемых (измеряемых) характеристик (параметров) продукции</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1847\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Наименование СИ, тип (марка), регистрационный номер в Федеральном информационном фонде по обеспечению единства измерений (при наличии)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1418\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Изготовитель (страна, наименование организации, год выпуска)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1843\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Год ввода в эксплуатацию, заводской номер (при наличии), инвентарный номер или другая уникальная идентификация</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"2693\" w:type=\"dxa\"/><w:gridSpan w:val=\"2\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Метрологические характеристики</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"2828\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Сведения о результатах поверки СИ в Федеральном информационном фонде по обеспечению единства измерений (номер, дата, срок действия) и (или) сертификат о калибровке СИ (номер, дата, срок действия (при наличии) в соответствии с требованиями законодательства Российской Федерации в области обеспечения единства измерений</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1283\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Право собственности или иное законное основание, предусматривающее право владения и пользования (реквизиты подтверждающих документов) (СФ – счет-фактура, ТН – товарная накладная, УПД – универсальный передаточный документ)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1134\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Место установки или хранения (Адрес: " + info.at(1) + ")</w:t></w:r></w:p></w:tc>"
                                   "</w:tr>"

                                   "<w:tr>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1271\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1847\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1418\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1843\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"

                                   "<w:tc><w:tcPr><w:tcW w:w=\"1558\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Диапазон измерений</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1135\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Класс точности (разряд), погрешность и (или) неопределенность (класс, разряд)</w:t></w:r></w:p></w:tc>"

                                   "<w:tc><w:tcPr><w:tcW w:w=\"2828\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1283\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1134\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:r><w:t></w:t></w:r></w:p></w:tc>"
                                   "</w:tr>"

                                   "<w:tr>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>1</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1271\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>2</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1847\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>3</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1418\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>4</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1843\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>5</w:t></w:r></w:p></w:tc>"

                                   "<w:tc><w:tcPr><w:tcW w:w=\"1558\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>6</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1135\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>7</w:t></w:r></w:p></w:tc>"

                                   "<w:tc><w:tcPr><w:tcW w:w=\"2828\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>8</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1283\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>9</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1134\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>10</w:t></w:r></w:p></w:tc>"
                                   "</w:tr>";
                int hyperlink_id = 1;
                int number = 1;
                for (int i = 0; i < data_SI.size(); ++i) {
                    hyperlink_id += 1;
                    bool found = false;
                    QStringList pov_list;
                    QStringList all_pov_list = data_SI.at(i).at(12).split("@");
                    pov_list << "клеймо на корпусе бессрочно" << "" << "" << "";
                    QDate arhDate = QDate::fromString(targetDate, "dd.MM.yyyy");
                    if (data_SI.at(i).at(12) != "-"){
                        for (int j = all_pov_list.size()-1; j >= 0; j--) {
                            QStringList pov_parts = all_pov_list.at(j).split(";");
                            QDate startDate = QDate::fromString(pov_parts.at(0), "dd.MM.yyyy");
                            QDate endDate = QDate::fromString(pov_parts.at(3), "dd.MM.yyyy");
                            if (startDate <= arhDate && arhDate <= endDate) {
                                pov_list = pov_parts;
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            continue;
                        }
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

                    // тут поработать над исключениями
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

                    number += 1;
                    QString poverka;
                    if (data_SI.at(i).at(12) != "-"){
                        poverka = "Свидетельство о поверке";
                        poverka += "<w:br/>";
                        poverka += pov_list.at(2);
                        poverka += "<w:br/></w:t></w:r><w:hyperlink r:id=\"rId";
                        poverka += QString::number(hyperlink_id);
                        poverka += "\" w:history=\"1\"><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>";
                        poverka += data_SI.at(i).at(13);
                        poverka += "</w:t></w:r></w:hyperlink><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t><w:br/>от ";
                        poverka += pov_list.at(0);
                        poverka += "<w:br/>до ";
                        poverka += pov_list.at(3);
                    } else {
                        poverka = "Клеймо на корпусе \"бессрочно\"";
                    }

                    documentContent +=
                        "<w:tr>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"567\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + QString::number(number - 1) + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1271\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + pokazatel + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1847\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + data_SI.at(i).at(1) + " " + data_SI.at(i).at(2) + "<w:br/>" + "рег. № " + data_SI.at(i).at(3) + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1418\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + "<![CDATA[" + data_SI.at(i).at(8).split("@").at(0) + "]]>" + ",<w:br/>" + "<![CDATA[" + data_SI.at(i).at(8).split("@").at(1) + "]]>" + ",<w:br/>" + "<![CDATA[" + data_SI.at(i).at(8).split("@").at(2) + "]]>" + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1843\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + date_SI + "<w:br/>" + "зав. № " + data_SI.at(i).at(4) + "<w:br/>" + "инв. № " + data_SI.at(i).at(0) + "</w:t></w:r></w:p></w:tc>"

                        "<w:tc><w:tcPr><w:tcW w:w=\"1558\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + diapazony + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1135\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + pogreshnosty + "</w:t></w:r></w:p></w:tc>"

                        "<w:tc><w:tcPr><w:tcW w:w=\"2828\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + poverka + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1283\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + data_SI.at(i).at(9) + "</w:t></w:r></w:p></w:tc>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"1134\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + "Пом" + "<w:br/>" + "№" + data_SI.at(i).at(5) + "<w:br/>" + data_SI.at(i).at(6) + "</w:t></w:r></w:p></w:tc>"
                        "</w:tr>";
                }

                documentContent +=
                    "</w:tbl>"
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"240\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"20\"/>"
                    "<w:szCs w:val=\"20\"/>"
                    "</w:rPr>"
                    "<w:t><w:br/></w:t>"
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
                    "<w:gridCol w:w=\"4900\"/>"
                    "<w:gridCol w:w=\"500\"/>"
                    "<w:gridCol w:w=\"4492\"/>"
                    "<w:gridCol w:w=\"500\"/>"
                    "<w:gridCol w:w=\"4492\"/>"
                    "</w:tblGrid>" // 14884

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4900\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Руководитель ИЛ ПБ ООО «ПромПожТест»</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"500\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4492\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"500\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4492\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + current_ruk + "</w:t></w:r></w:p></w:tc>"
                    "</w:tr>"

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4900\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>(должность уполномоченного лица)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"500\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4492\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>(подпись уполномоченного лица)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"500\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4492\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>(инициалы, фамилия уполномоченного лица)</w:t></w:r></w:p></w:tc>"
                    "</w:tr>"

                    "</w:tbl>";

                documentContent +=
                    "<w:sectPr>"
                    "<w:headerReference w:type=\"default\" r:id=\"rId1\"/>"
                    "<w:pgSz w:w=\"16838\" w:h=\"11909\" w:orient=\"landscape\" w:code=\"9\"/>"
                    "<w:pgMar w:top=\"851\" w:right=\"851\" w:bottom=\"851\" w:left=\"1134\" w:header=\"709\" w:footer=\"709\" w:gutter=\"0\"/>"
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
                                  "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
                                  "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/header\" Target=\"header1.xml\"/>";
                int hyperlink_id = 1;
                for (int i = 0; i < data_SI.size(); ++i){
                    hyperlink_id += 1;
                    content += "<Relationship Id=\"rId" + QString::number(hyperlink_id) + "\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/hyperlink\" Target=\"" + data_SI.at(i).at(13) + "\" TargetMode=\"External\"/>";
                }
                content += "</Relationships>";
                return content.toUtf8();
            }()
        },

        // word/header1.xml
        {
            "word/header1.xml",
            [this]() {
                QString content = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                                  "<w:hdr xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">"

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
                                  "<w:gridCol w:w=\"2977\"/>"
                                  "<w:gridCol w:w=\"8930\"/>"
                                  "<w:gridCol w:w=\"2977\"/>"
                                  "</w:tblGrid>" // 14884

                                  "<w:tr>"
                                  "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:b/><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Актуально на</w:t></w:r></w:p></w:tc>"
                                  "<w:tc><w:tcPr><w:tcW w:w=\"8930\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:b/><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Сведения об оснащенности средствами измерений (СИ)<w:br/>" + info.at(0) + "</w:t></w:r></w:p></w:tc>"
                                  "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/><w:vAlign w:val=\"center\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr>"

                                  "<w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t xml:space=\"preserve\">Стр. </w:t></w:r>"
                                  "<w:fldSimple w:instr=\" PAGE \"><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>1</w:t></w:r></w:fldSimple>"
                                  "<w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t xml:space=\"preserve\"> из </w:t></w:r>"
                                  "<w:fldSimple w:instr=\" NUMPAGES \"><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>1</w:t></w:r></w:fldSimple></w:p></w:tc>"
                                  "</w:tr>"

                                  "<w:tr>"
                                "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>"+ QDateTime::currentDateTime().toString("dd.MM.yyyy").toUtf8() + "</w:t></w:r></w:p></w:tc>"
                                                                                                   "<w:tc><w:tcPr><w:tcW w:w=\"8930\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                                                                                                   "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                                                                                                   "</w:tr>"

                                                                                                   "</w:tbl>"

                                                                                                   "<w:p>"
                                                                                                   "<w:r>"
                                                                                                   "<w:t></w:t>"
                                                                                                   "</w:r>"
                                                                                                   "</w:p>"
                                                                                                   "</w:hdr>";
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
            "<dc:creator>amogus</dc:creator>"
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

void forms::create_arh_form_IO_Docx(const QString &fileName) {

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
            "<Override PartName=\"/word/header1.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.header+xml\"/>"
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
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"240\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"20\"/>"
                    "<w:szCs w:val=\"20\"/>"
                    "</w:rPr>"
                    "<w:t>Форма 3. Оснащенность ИЛ испытательным оборудованием (ИО)</w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
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
                    "<w:gridCol w:w=\"424\"/>"
                    "<w:gridCol w:w=\"1560\"/>"
                    "<w:gridCol w:w=\"1560\"/>"
                    "<w:gridCol w:w=\"2024\"/>"
                    "<w:gridCol w:w=\"1447\"/>"

                    "<w:gridCol w:w=\"2908\"/>"
                    "<w:gridCol w:w=\"1276\"/>"
                    "<w:gridCol w:w=\"1367\"/>"
                    "<w:gridCol w:w=\"1560\"/>"
                    "<w:gridCol w:w=\"758\"/>"

                    "</w:tblGrid>" // 14884


                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"424\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>N п/п</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1560\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Наименование определяемых (измеряемых) характеристик (параметров) продукции</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1560\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Наименование испытуемых групп объектов</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"2024\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Наименование испытательного оборудования, тип (марка)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1447\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Изготовитель (страна, наименование организации, год выпуска)</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"2908\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Основные технические характеристики</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1276\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Год ввода в эксплуатацию, заводской номер (при наличии), инвентарный номер или другая уникальная идентификация</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1367\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Дата и номер документа об аттестации ИО, срок его действия</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1560\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Право собственности или иное законное основание, предусматривающее право владения и пользования (реквизиты подтверждающих документов) (СФ – счет-фактура, ТН – товарная накладная, УПД – универсальный передаточный документ)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"758\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Место установки или хранения (Адрес: " + info.at(1) + ")</w:t></w:r></w:p></w:tc>"
                                   "</w:tr>"

                                   "<w:tr>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"424\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>1</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1560\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>2</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1560\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>3</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"2024\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>4</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1447\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>5</w:t></w:r></w:p></w:tc>"

                                   "<w:tc><w:tcPr><w:tcW w:w=\"2908\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>6</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1276\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>7</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1367\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>8</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1560\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>9</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"758\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>10</w:t></w:r></w:p></w:tc>"
                                   "</w:tr>";
                int kol_vo = 0;
                for (int i = 0; i < data_IO.size(); ++i) {
                    QString pov;
                    QStringList pov_list;
                    QStringList all_pov_list = data_IO.at(i).at(9).split("@");
                    pov = all_pov_list.last();
                    pov_list = pov.split(";");

                    bool found = false;
                    QDate arhDate = QDate::fromString(targetDate, "dd.MM.yyyy");

                    for (int j = all_pov_list.size()-1; j >= 0; j--) {
                        QStringList pov_parts = all_pov_list.at(j).split(";");
                        QDate startDate = QDate::fromString(pov_parts.at(0), "dd.MM.yyyy");
                        QDate endDate = QDate::fromString(pov_parts.at(3), "dd.MM.yyyy");
                        if (startDate <= arhDate && arhDate <= endDate) {
                            pov_list = pov_parts;
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        continue;
                    }

                    QString tecn = data_IO.at(i).at(12);
                    QString r_tecn = tecn.replace("\n", "<w:br/>");


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
                    kol_vo += 1;
                    documentContent +=
                        "<w:tr>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"424\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + QString::number(kol_vo) + "</w:t></w:r></w:p></w:tc>"
                                                   "<w:tc><w:tcPr><w:tcW w:w=\"1560\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + data_IO.at(i).at(11) + "</w:t></w:r></w:p></w:tc>"
                                                 "<w:tc><w:tcPr><w:tcW w:w=\"1560\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + data_IO.at(i).at(10) + "</w:t></w:r></w:p></w:tc>"
                                                 "<w:tc><w:tcPr><w:tcW w:w=\"2024\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + data_IO.at(i).at(2) + "</w:t></w:r></w:p></w:tc>"
                                                "<w:tc><w:tcPr><w:tcW w:w=\"1447\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-30\" w:right=\"-30\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + "<![CDATA[" + data_IO.at(i).at(5).split("@").at(0) + "]]>" + ",<w:br/>" + "<![CDATA[" + data_IO.at(i).at(5).split("@").at(1) + "]]>" + ",<w:br/>" + "<![CDATA[" + data_IO.at(i).at(5).split("@").at(2) + "]]>" + "</w:t></w:r></w:p></w:tc>"

                                                                                                                                                                                                                                           "<w:tc><w:tcPr><w:tcW w:w=\"2908\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + r_tecn + "</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1276\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + date_SI + "<w:br/>" + "зав. № " + data_IO.at(i).at(3) + "<w:br/>" + "инв. № " + data_IO.at(i).at(0) + "</w:t></w:r></w:p></w:tc>"
                                                                                                                                                             "<w:tc><w:tcPr><w:tcW w:w=\"1367\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + "Протокол №" + "<w:br/>" + pov_list.at(2) + "<w:br/>" + "от " + pov_list.at(0) + "<w:br/>" + "до " + pov_list.at(3) + "</w:t></w:r></w:p></w:tc>"
                                                                                                                                                "<w:tc><w:tcPr><w:tcW w:w=\"1560\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + data_IO.at(i).at(6) + "</w:t></w:r></w:p></w:tc>"
                                                "<w:tc><w:tcPr><w:tcW w:w=\"758\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + "Пом" + "<w:br/>" + "№" + data_IO.at(i).at(4) + "</w:t></w:r></w:p></w:tc>"
                                                                          "</w:tr>";
                }

                documentContent +=
                    "</w:tbl>"
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"240\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"20\"/>"
                    "<w:szCs w:val=\"20\"/>"
                    "</w:rPr>"
                    "<w:t><w:br/></w:t>"
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
                    "<w:gridCol w:w=\"4900\"/>"
                    "<w:gridCol w:w=\"500\"/>"
                    "<w:gridCol w:w=\"4492\"/>"
                    "<w:gridCol w:w=\"500\"/>"
                    "<w:gridCol w:w=\"4492\"/>"
                    "</w:tblGrid>" // 14884

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4900\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Руководитель ИЛ ПБ ООО «ПромПожТест»</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"500\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4492\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"500\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4492\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + current_ruk + "</w:t></w:r></w:p></w:tc>"
                            "</w:tr>"

                            "<w:tr>"
                            "<w:tc><w:tcPr><w:tcW w:w=\"4900\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>(должность уполномоченного лица)</w:t></w:r></w:p></w:tc>"
                            "<w:tc><w:tcPr><w:tcW w:w=\"500\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                            "<w:tc><w:tcPr><w:tcW w:w=\"4492\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>(подпись уполномоченного лица)</w:t></w:r></w:p></w:tc>"
                            "<w:tc><w:tcPr><w:tcW w:w=\"500\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                            "<w:tc><w:tcPr><w:tcW w:w=\"4492\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>(инициалы, фамилия уполномоченного лица)</w:t></w:r></w:p></w:tc>"
                            "</w:tr>"

                            "</w:tbl>";

                documentContent +=
                    "<w:sectPr>"
                    "<w:headerReference w:type=\"default\" r:id=\"rId1\"/>"
                    "<w:pgSz w:w=\"16838\" w:h=\"11909\" w:orient=\"landscape\" w:code=\"9\"/>"
                    "<w:pgMar w:top=\"851\" w:right=\"851\" w:bottom=\"851\" w:left=\"1134\" w:header=\"709\" w:footer=\"709\" w:gutter=\"0\"/>"
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
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/header\" Target=\"header1.xml\"/>"
            "</Relationships>"
        },

        // word/header1.xml
        {
            "word/header1.xml",
            [this]() {
                QString documentContent = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                                          "<w:hdr xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">"

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
                                          "<w:gridCol w:w=\"2977\"/>"
                                          "<w:gridCol w:w=\"8930\"/>"
                                          "<w:gridCol w:w=\"2977\"/>"
                                          "</w:tblGrid>" // 14884

                                          "<w:tr>"
                                          "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:b/><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Актуально на</w:t></w:r></w:p></w:tc>"
                                          "<w:tc><w:tcPr><w:tcW w:w=\"8930\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:b/><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Сведения об оснащенности испытательным оборудованием (ИО)<w:br/>" + info.at(0) + "</w:t></w:r></w:p></w:tc>"
                                                         "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/><w:vAlign w:val=\"center\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr>"

                                                         "<w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t xml:space=\"preserve\">Стр. </w:t></w:r>"
                                                         "<w:fldSimple w:instr=\" PAGE \"><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>1</w:t></w:r></w:fldSimple>"
                                                         "<w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t xml:space=\"preserve\"> из </w:t></w:r>"
                                                         "<w:fldSimple w:instr=\" NUMPAGES \"><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>1</w:t></w:r></w:fldSimple></w:p></w:tc>"
                                                         "</w:tr>"

                                                         "<w:tr>"
                                                         "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>"+ QDateTime::currentDateTime().toString("dd.MM.yyyy").toUtf8() + "</w:t></w:r></w:p></w:tc>"
                                                                                                           "<w:tc><w:tcPr><w:tcW w:w=\"8930\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                                                                                                           "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                                                                                                           "</w:tr>"

                                                                                                           "</w:tbl>"

                                                                                                           "<w:p>"
                                                                                                           "<w:r>"
                                                                                                           "<w:t></w:t>"
                                                                                                           "</w:r>"
                                                                                                           "</w:p>"
                                                                                                           "</w:hdr>";
                return documentContent.toUtf8();
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
            "<dc:creator>amogus</dc:creator>"
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

void forms::create_arh_form_VO_Docx(const QString &fileName) {

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
            "<Override PartName=\"/word/header1.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.header+xml\"/>"
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
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"240\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"20\"/>"
                    "<w:szCs w:val=\"20\"/>"
                    "</w:rPr>"
                    "<w:t>Форма 4. Оснащенность ИЛ вспомогательными оборудованием (ВО)</w:t>"
                    "</w:r>"
                    "</w:p>";

                documentContent +=
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
                    "<w:gridCol w:w=\"503\"/>"
                    "<w:gridCol w:w=\"4071\"/>"
                    "<w:gridCol w:w=\"2268\"/>"
                    "<w:gridCol w:w=\"1701\"/>"
                    "<w:gridCol w:w=\"2977\"/>"

                    "<w:gridCol w:w=\"1417\"/>"
                    "<w:gridCol w:w=\"1947\"/>"

                    "</w:tblGrid>" // 14884

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"503\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>N п/п</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4071\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Наименование</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"2268\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Изготовитель (страна, наименование организации, год выпуска)</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"1701\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Год ввода в эксплуатацию, заводской номер (при наличии), инвентарный номер или другая уникальная идентификация</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Назначение</w:t></w:r></w:p></w:tc>"

                    "<w:tc><w:tcPr><w:tcW w:w=\"1417\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Место установки или хранения (Адрес: " + info.at(1) + ")</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1947\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Право собственности или иное законное основание, предусматривающее право владения и пользования (реквизиты подтверждающих документов) (СФ – счет-фактура, ТН – товарная накладная, УПД – универсальный передаточный документ)</w:t></w:r></w:p></w:tc>"
                                   "</w:tr>"

                                   "<w:tr>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"503\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>1</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"4071\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>2</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"2268\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>3</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1701\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>4</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>5</w:t></w:r></w:p></w:tc>"

                                   "<w:tc><w:tcPr><w:tcW w:w=\"1417\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>6</w:t></w:r></w:p></w:tc>"
                                   "<w:tc><w:tcPr><w:tcW w:w=\"1947\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>7</w:t></w:r></w:p></w:tc>"
                                   "</w:tr>";

                int kol_vo = 0;
                for (int i = 0; i < data_VO.size(); ++i) {
                    QDate arhDate = QDate::fromString(targetDate, "dd.MM.yyyy");
                    QString date_SI = data_VO.at(i).at(6);
                    QStringList date_SI_List = date_SI.split("@");
                    bool found = false;

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

                    kol_vo += 1;
                    documentContent +=
                        "<w:tr>"
                        "<w:tc><w:tcPr><w:tcW w:w=\"503\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + QString::number(kol_vo) + "</w:t></w:r></w:p></w:tc>"
                                                   "<w:tc><w:tcPr><w:tcW w:w=\"4071\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + data_VO.at(i).at(1) + "</w:t></w:r></w:p></w:tc>"
                                                "<w:tc><w:tcPr><w:tcW w:w=\"2268\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + "<![CDATA[" + data_VO.at(i).at(4).split("@").at(0) + "]]>" + ",<w:br/>" + "<![CDATA[" + data_VO.at(i).at(4).split("@").at(1) + "]]>" + ",<w:br/>" + "<![CDATA[" + data_VO.at(i).at(4).split("@").at(2) + "]]>" + "</w:t></w:r></w:p></w:tc>"
                                                                                                                                                                                                                                           "<w:tc><w:tcPr><w:tcW w:w=\"1701\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + date_SI + "<w:br/>" + "зав. № " + data_VO.at(i).at(2) + "<w:br/>" + "инв. № " + data_VO.at(i).at(0) + "</w:t></w:r></w:p></w:tc>"
                                                                                                                                                             "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + data_VO.at(i).at(7) + "</w:t></w:r></w:p></w:tc>"

                                                "<w:tc><w:tcPr><w:tcW w:w=\"1417\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + "Пом" + "<w:br/>№" + data_VO.at(i).at(3) + "</w:t></w:r></w:p></w:tc>"
                                                                     "<w:tc><w:tcPr><w:tcW w:w=\"1947\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + data_VO.at(i).at(5) + "</w:t></w:r></w:p></w:tc>"
                                                "</w:tr>";
                }

                documentContent +=
                    "</w:tbl>"
                    "<w:p>"
                    "<w:pPr>"
                    "<w:pStyle w:val=\"a5\"/>"
                    "<w:spacing w:after=\"240\"/>"
                    "<w:jc w:val=\"left\"/>"
                    "</w:pPr>"
                    "<w:r>"
                    "<w:rPr>"
                    "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
                    "<w:sz w:val=\"20\"/>"
                    "<w:szCs w:val=\"20\"/>"
                    "</w:rPr>"
                    "<w:t><w:br/></w:t>"
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
                    "<w:gridCol w:w=\"4900\"/>"
                    "<w:gridCol w:w=\"500\"/>"
                    "<w:gridCol w:w=\"4492\"/>"
                    "<w:gridCol w:w=\"500\"/>"
                    "<w:gridCol w:w=\"4492\"/>"
                    "</w:tblGrid>" // 14884

                    "<w:tr>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4900\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Руководитель ИЛ ПБ ООО «ПромПожТест»</w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"500\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4492\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"500\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                    "<w:tc><w:tcPr><w:tcW w:w=\"4492\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>" + current_ruk + "</w:t></w:r></w:p></w:tc>"
                            "</w:tr>"

                            "<w:tr>"
                            "<w:tc><w:tcPr><w:tcW w:w=\"4900\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>(должность уполномоченного лица)</w:t></w:r></w:p></w:tc>"
                            "<w:tc><w:tcPr><w:tcW w:w=\"500\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                            "<w:tc><w:tcPr><w:tcW w:w=\"4492\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>(подпись уполномоченного лица)</w:t></w:r></w:p></w:tc>"
                            "<w:tc><w:tcPr><w:tcW w:w=\"500\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                            "<w:tc><w:tcPr><w:tcW w:w=\"4492\" w:type=\"dxa\"/><w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"000000\"/><w:left w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:bottom w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/><w:right w:val=\"single\" w:sz=\"4\" w:space=\"0\" w:color=\"FFFFFF\"/></w:tcBorders></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>(инициалы, фамилия уполномоченного лица)</w:t></w:r></w:p></w:tc>"
                            "</w:tr>"

                            "</w:tbl>";

                documentContent +=
                    "<w:sectPr>"
                    "<w:headerReference w:type=\"default\" r:id=\"rId1\"/>"
                    "<w:pgSz w:w=\"16838\" w:h=\"11909\" w:orient=\"landscape\" w:code=\"9\"/>"
                    "<w:pgMar w:top=\"851\" w:right=\"851\" w:bottom=\"851\" w:left=\"1134\" w:header=\"709\" w:footer=\"709\" w:gutter=\"0\"/>"
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
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/header\" Target=\"header1.xml\"/>"
            "</Relationships>"
        },

        // word/header1.xml
        {
            "word/header1.xml",
            [this]() {
                QString content = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                                  "<w:hdr xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">"

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
                                  "<w:gridCol w:w=\"2977\"/>"
                                  "<w:gridCol w:w=\"8930\"/>"
                                  "<w:gridCol w:w=\"2977\"/>"
                                  "</w:tblGrid>" // 14884

                                  "<w:tr>"
                                  "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:b/><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Актуально на</w:t></w:r></w:p></w:tc>"
                                  "<w:tc><w:tcPr><w:tcW w:w=\"8930\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:b/><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>Сведения об оснащенности вспомогательным оборудованием (ВО)<w:br/>" + info.at(0) + "</w:t></w:r></w:p></w:tc>"
                                                 "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/><w:vMerge w:val=\"restart\"/><w:vAlign w:val=\"center\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr>"

                                                 "<w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t xml:space=\"preserve\">Стр. </w:t></w:r>"
                                                 "<w:fldSimple w:instr=\" PAGE \"><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>1</w:t></w:r></w:fldSimple>"
                                                 "<w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t xml:space=\"preserve\"> из </w:t></w:r>"
                                                 "<w:fldSimple w:instr=\" NUMPAGES \"><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>1</w:t></w:r></w:fldSimple></w:p></w:tc>"
                                                 "</w:tr>"

                                                 "<w:tr>"
                                                 "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t>"+ QDateTime::currentDateTime().toString("dd.MM.yyyy").toUtf8() + "</w:t></w:r></w:p></w:tc>"
                                                                                                   "<w:tc><w:tcPr><w:tcW w:w=\"8930\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                                                                                                   "<w:tc><w:tcPr><w:tcW w:w=\"2977\" w:type=\"dxa\"/><w:vMerge/></w:tcPr><w:p><w:pPr><w:ind w:left=\"-10\" w:right=\"-10\"/><w:spacing w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr><w:r><w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"20\"/><w:szCs w:val=\"20\"/></w:rPr><w:t></w:t></w:r></w:p></w:tc>"
                                                                                                   "</w:tr>"

                                                                                                   "</w:tbl>"

                                                                                                   "<w:p>"
                                                                                                   "<w:r>"
                                                                                                   "<w:t></w:t>"
                                                                                                   "</w:r>"
                                                                                                   "</w:p>"
                                                                                                   "</w:hdr>";
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
            "<dc:creator>amogus</dc:creator>"
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

void forms::on_pushButton_arh_form_2_clicked()
{
    QString file_name;
    QString folderPath;
    targetDate = ui->dateEdit->date().toString("dd.MM.yyyy");
    current_ruk = getEmployeeOnPost(ruk, ui->dateEdit->date());
    folderPath = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку для выгрузки архивной формы 2",
        "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );
    if (folderPath != ""){
        file_name += folderPath + "/";
        file_name += "Ф-2 СИ " + targetDate + ".docx";
        create_arh_form_SI_Docx(file_name);
        ui->pushButton_arh_form_2->setText("Готово");
    }
}


void forms::on_pushButton_arh_form_3_clicked()
{
    QString file_name;
    QString folderPath;
    targetDate = ui->dateEdit->date().toString("dd.MM.yyyy");
    current_ruk = getEmployeeOnPost(ruk, ui->dateEdit->date());
    folderPath = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку для выгрузки архивной формы 3",
        "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );
    if (folderPath != ""){
        file_name += folderPath + "/";
        file_name += "Ф-3 ИО " + targetDate + ".docx";
        create_arh_form_IO_Docx(file_name);
        ui->pushButton_arh_form_3->setText("Готово");
    }
}


void forms::on_pushButton_arh_form_4_clicked()
{
    QString file_name;
    QString folderPath;
    targetDate = ui->dateEdit->date().toString("dd.MM.yyyy");
    current_ruk = getEmployeeOnPost(ruk, ui->dateEdit->date());
    folderPath = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку для выгрузки архивной формы 4",
        "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );
    if (folderPath != ""){
        file_name += folderPath + "/";
        file_name += "Ф-4 ВО " + targetDate + ".docx";
        create_arh_form_VO_Docx(file_name);
        ui->pushButton_arh_form_4->setText("Готово");
    }
}


void forms::on_dateEdit_userDateChanged(const QDate &date)
{
    if (date < QDate::currentDate()){
        ui->pushButton_arh_form_2->setText("Архивная Ф-2");
        ui->pushButton_arh_form_3->setText("Архивная Ф-3");
        ui->pushButton_arh_form_4->setText("Архивная Ф-4");
    } else if (date == QDate::currentDate()) {
        ui->pushButton_arh_form_2->setText("Актуальная Ф-2");
        ui->pushButton_arh_form_3->setText("Актуальная Ф-3");
        ui->pushButton_arh_form_4->setText("Актуальная Ф-4");
    } else {
        ui->pushButton_arh_form_2->setText("Будущая Ф-2");
        ui->pushButton_arh_form_3->setText("Будущая Ф-3");
        ui->pushButton_arh_form_4->setText("Будущая Ф-4");
    }
}

QString forms::getEmployeeOnPost(const QString &data, const QDate &targetDate) {
    QStringList employees = data.split('@', Qt::SkipEmptyParts);
    if (employees.isEmpty()) return QString();

    // Первый сотрудник (без даты)
    QString currentEmployee = employees[0].trimmed();
    if (employees.size() == 1) return currentEmployee;

    QList<QDate> dates{QDate(1, 1, 1)}; // Минимальная дата для первого сотрудника
    QStringList names{currentEmployee};

    // Обработка остальных сотрудников (с датами)
    for (int i = 1; i < employees.size(); ++i) {
        QStringList parts = employees[i].split(';', Qt::SkipEmptyParts);
        if (parts.size() != 2) continue;

        QDate date = QDate::fromString(parts[0].trimmed(), "dd.MM.yyyy");
        if (date.isValid()) {
            dates.append(date);
            names.append(parts[1].trimmed());
        }
    }

    // Поиск последней подходящей даты
    int resultIndex = 0;
    for (int i = 1; i < dates.size(); ++i) {
        if (dates[i] <= targetDate) {
            resultIndex = i;
        } else {
            break; // Даты должны быть отсортированы по возрастанию
        }
    }

    return names[resultIndex];
}
