#ifndef ADD_TYPE_SI_H
#define ADD_TYPE_SI_H

#include <QDialog>
#include <QHBoxLayout>

namespace Ui {
class add_type_si;
}

class add_type_si : public QDialog
{
    Q_OBJECT

public:
    explicit add_type_si(QWidget *parent = nullptr);
    ~add_type_si();

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

private slots:
    void on_pushButton_add_d_clicked();
    void on_pushButton_rem_d_clicked();
    void on_pushButton_add_p_clicked();
    void on_pushButton_rem_p_clicked();
    void on_pushButton_add_clicked();

    void on_pushButton_close_clicked();

private:
    Ui::add_type_si *ui;

    QList<QHBoxLayout*> layouts_d_List;
    QList<QHBoxLayout*> layouts_p_List;
    QString get_d_Text(QHBoxLayout *hLayout) const;
    void collect_info();
};

#endif // ADD_TYPE_SI_H
