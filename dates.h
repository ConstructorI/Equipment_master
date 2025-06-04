//dates.h
#ifndef DATES_H
#define DATES_H

#include "database.h"

#include <QtSql>
#include <QDialog>
#include <QHBoxLayout>
#include <QTextBlock>
#include <QDateTime>
#include <QDataStream>
#include <QtEndian>

namespace Ui {
class Dates;
}

class Dates : public QDialog
{
    Q_OBJECT

public:
    explicit Dates(QWidget *parent = nullptr);
    ~Dates();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_to_file_clicked();

    void on_dateEdit_year_userDateChanged(const QDate &date);

    void on_pushButton_graf_pov_clicked();

private:
    Ui::Dates *ui;
    int year;
    static const quint32 crc32_table[];

    QVector<bool> existsSI;
    QVector<QDate> datespov;
    QVector<QDate> datespov_in_month;
    QStringList inv_pov_in_month;
    QStringList names_pov_in_month;

    QVector<bool> existsIO;
    QVector<QDate> datesatt;
    QVector<QDate> datesatt_in_month;
    QStringList inv_att_in_month;
    QStringList names_att_in_month;

    QList<QHBoxLayout*> layouts_pov_List;
    QList<QHBoxLayout*> layouts_att_List;

    void fill_layout(const QVector<QDate> &dates_in_month,
                     const QStringList &inv_in_month,
                     const QStringList &names_in_month,
                     QList<QHBoxLayout*> &layouts_List);

    void clean_layout(QList<QHBoxLayout*> &layouts_List);

    quint32 calculate_crc32(const QByteArray &data);
    quint32 datetime_to_dos(const QDateTime &dt);
    quint32 write_local_file(QDataStream &zip, const QString &name, const QByteArray &content);
    void write_central_directory(QDataStream &zip,
                                 const QList<QPair<QString, QByteArray>> &files,
                                 const QList<quint32> &offsets);
    void create_graf_pov(const QString &fileName);
    void create_graf_pov_dop(const QString &fileName);
    void create_graf_att(const QString &fileName);
    void create_graf_att_dop(const QString &fileName);
    int dop_num;
    QString date_dop;
    QStringList info;
    QString metr;
    QString ruk;
    QString gen;
    QString current_metr;
    QString current_ruk;
    QString current_gen;
    QVector<QStringList> data_SI;
    QVector<QStringList> data_IO;
    QVector<QStringList> data_VO;
    QString getEmployeeOnPost(const QString &data, const QDate &targetDate);
};

#endif // DATES_H
