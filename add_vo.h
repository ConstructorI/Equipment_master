//add_vo.h
#ifndef ADD_VO_H
#define ADD_VO_H

#include <QDialog>
#include <QHBoxLayout>

namespace Ui {
class add_vo;
}

class add_vo : public QDialog
{
    Q_OBJECT

public:
    explicit add_vo(QWidget *parent = nullptr);
    ~add_vo();

    bool is_edit;
    int VOId;
    QString inv;
    QString naim;
    QString zav;
    QString pom;
    QString izg;
    QString pravo;
    QString date;
    QString pred;
    QString PO;
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
    void on_pushButton_edit_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_delete_clicked();
    void on_pushButton_add_clicked();

    void on_pushButton_add_to_clicked();

    void on_pushButton_rem_to_clicked();

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::add_vo *ui;
    void collect_info();
    QList<QHBoxLayout*> layouts_rem_List;
    QString get_Text(QHBoxLayout *hLayout) const;
};

#endif // ADD_VO_H
