//add_io.h
#ifndef ADD_IO_H
#define ADD_IO_H

#include "database.h"

#include <QDialog>
#include <QHBoxLayout>

namespace Ui {
class add_io;
}

class add_io : public QDialog
{
    Q_OBJECT

public:
    explicit add_io(QWidget *parent = nullptr);
    ~add_io();

    bool is_edit;
    int IOId;
    QString inv;
    QString naim;
    QString krnaim;
    QString zav;
    QString pom;
    QString izg;
    QString pravo;
    QString date;
    QString pasp;
    QString att;
    QString obj;
    QString type;
    QString khar;
    QString temp;
    QString vlag;
    QString davl;
    QString napr;
    QString chast;
    QString tecn;
    QString clean;
    QString remont;

    void set_edit();
    void set_data();

private slots:
    void on_pushButton_cancel_clicked();
    void on_pushButton_add_clicked();
    void on_pushButton_att_add_clicked();
    void on_pushButton_att_rem_clicked();
    void on_pushButton_delete_clicked();
    void on_pushButton_edit_clicked();

    void on_pushButton_add_to_clicked();

    void on_pushButton_rem_to_clicked();

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::add_io *ui;
    QList<QHBoxLayout*> layouts_att_List;
    QList<QHBoxLayout*> layouts_rem_List;
    QString get_att_Text(QHBoxLayout *hLayout) const;
    void collect_info();
};

#endif // ADD_IO_H
