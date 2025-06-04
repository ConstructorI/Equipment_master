//file_menu.h
#ifndef FILE_MENU_H
#define FILE_MENU_H

#include "database.h"
#include <QtSql>
#include <QDialog>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QTextBlock>
#include <QStandardItemModel>


namespace Ui {
class file_menu;
}

class file_menu : public QDialog
{
    Q_OBJECT

public:
    explicit file_menu(QWidget *parent = nullptr);
    ~file_menu();

private slots:
    void on_radioButton_SI_clicked();
    void on_radioButton_IO_clicked();
    void on_radioButton_VO_clicked();

    void on_pushButton_birki_clicked();
    void on_pushButton_karts_clicked();

    void on_listView_allinv_entered(const QModelIndex &index);

private:
    int table;
    Ui::file_menu *ui;
    QStandardItemModel *model;
    static const quint32 crc32_table[];
    quint32 calculate_crc32(const QByteArray &data);
    quint32 datetime_to_dos(const QDateTime &dt);
    quint32 write_local_file(QDataStream &zip,
                             const QString &name,
                             const QByteArray &content);
    void write_central_directory(QDataStream &zip,
                                 const QList<QPair<QString, QByteArray>> &files,
                                 const QList<quint32> &offsets);
    bool setFileTimes(const QString &filePath, const QDateTime &creationTime, const QDateTime &modificationTime);
    QDateTime addRandomTimeToDate(const QDate &date);
    QDate last_modified;
    int num_of_changes;

    void create_birks_SI_Docx(const QString &fileName);
    void create_birks_IO_Docx(const QString &fileName);
    void create_birks_VO_Docx(const QString &fileName);

    void create_kart_SI_Docx(const QString &fileName);
    void create_kart_IO_Docx(const QString &fileName);
    void create_kart_VO_Docx(const QString &fileName);

    QStringList list_inv_nums_SI;
    QStringList list_inv_nums_IO;
    QStringList list_inv_nums_VO;

    QStringList cur_data_SI;
    QStringList cur_data_IO;
    QStringList cur_data_VO;

    QVector<QStringList> data_SI;
    QVector<QStringList> data_IO;
    QVector<QStringList> data_VO;
    QString data_metr;
    QString SI_dop_info;
    QStringList data_info;

    QString getEmployeeOnPost(const QString &data, const QDate &targetDate);
};

#endif // FILE_MENU_H
