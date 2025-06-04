//add_si.h
#ifndef ADD_SI_H
#define ADD_SI_H

#include <QDialog>
#include <QHBoxLayout>

namespace Ui {
class add_si;
}

class add_si : public QDialog
{
    Q_OBJECT

public:
    explicit add_si(QWidget *parent = nullptr);
    ~add_si();

    bool is_edit;
    bool is_pov;
    int SIId;
    QString inv;
    QString naim;
    QString mod;
    QString reestr;
    QString zav;
    QString pom;
    QString sost;
    QString PO;
    QString izg;
    QString pravo;
    QString date;
    QString diap;
    QString pov;
    QString fgis;
    QString temp;
    QString vlag;
    QString davl;
    QString napr;
    QString chast;
    QString tecn;
    QString clean;
    QString remont;
    QString dop_info;

    void set_edit();
    void set_data();
    void set_pov(bool is_pov);

private slots:
    void on_pushButton_add_d_clicked();
    void on_pushButton_rem_d_clicked();
    void on_pushButton_add_p_clicked();
    void on_pushButton_rem_p_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_add_clicked();
    void on_pushButton_delete_clicked();
    void on_pushButton_edit_clicked();

    void on_checkBox_pov_stateChanged(int arg1);

    void on_pushButton_add_to_clicked();

    void on_pushButton_rem_to_clicked();

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::add_si *ui;
    QList<QHBoxLayout*> layouts_d_List;
    QList<QHBoxLayout*> layouts_p_List;
    QList<QHBoxLayout*> layouts_rem_List;
    QString get_d_Text(QHBoxLayout *hLayout) const;
    QString get_p_Text(QHBoxLayout *hLayout) const;
    void collect_info();
};

#endif // ADD_SI_H
