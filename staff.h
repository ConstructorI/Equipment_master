//staff.h
#ifndef STAFF_H
#define STAFF_H

#include "database.h"

#include <QtSql>
#include <QDialog>
#include <QHBoxLayout>
#include <QTextBlock>

namespace Ui {
class staff;
}

class staff : public QDialog
{
    Q_OBJECT

public:
    explicit staff(QWidget *parent = nullptr);
    ~staff();

    void set_data();

private slots:
    void on_pushButton_metr_add_clicked();

    void on_pushButton_metr_rem_clicked();

    void on_pushButton_ruk_add_clicked();

    void on_pushButton_ruk_rem_clicked();

    void on_pushButton_gen_add_clicked();

    void on_pushButton_gen_rem_clicked();

    void on_pushButton_save_clicked();

private:
    Ui::staff *ui;
    QString name;
    QString adress;
    QString metr;
    QString ruk;
    QString gen;
    QList<QHBoxLayout*> layouts_metr_List;
    QList<QHBoxLayout*> layouts_ruk_List;
    QList<QHBoxLayout*> layouts_gen_List;
    QString get_Text(QHBoxLayout *hLayout) const;
    void collect_info();
};

#endif // STAFF_H
