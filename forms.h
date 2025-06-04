//forms.h
#ifndef FORMS_H
#define FORMS_H

#include <QDialog>

namespace Ui {
class forms;
}

class forms : public QDialog
{
    Q_OBJECT

public:
    explicit forms(QWidget *parent = nullptr);
    ~forms();

private slots:
    void on_pushButton_arh_form_2_clicked();

    void on_pushButton_arh_form_3_clicked();

    void on_pushButton_arh_form_4_clicked();

    void on_dateEdit_userDateChanged(const QDate &date);

private:
    Ui::forms *ui;

    int num_obr;
    QStringList info;
    QString ruk;
    QString current_ruk;
    QString getEmployeeOnPost(const QString &data, const QDate &targetDate);
    QVector<QStringList> data_SI;
    QVector<QStringList> data_IO;
    QVector<QStringList> data_VO;
    static const quint32 crc32_table[];
    quint32 calculate_crc32(const QByteArray &data);
    quint32 datetime_to_dos(const QDateTime &dt);
    quint32 write_local_file(QDataStream &zip,
                             const QString &name,
                             const QByteArray &content);
    void write_central_directory(QDataStream &zip,
                                 const QList<QPair<QString, QByteArray>> &files,
                                 const QList<quint32> &offsets);
    QString targetDate;
    void create_arh_form_SI_Docx(const QString &fileName);
    void create_arh_form_IO_Docx(const QString &fileName);
    void create_arh_form_VO_Docx(const QString &fileName);

};

#endif // FORMS_H
