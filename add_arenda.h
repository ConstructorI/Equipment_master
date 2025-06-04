#ifndef ADD_ARENDA_H
#define ADD_ARENDA_H

#include <QDialog>
#include <QHBoxLayout>

namespace Ui {
class add_Arenda;
}

class add_Arenda : public QDialog
{
    Q_OBJECT

public:
    explicit add_Arenda(QWidget *parent = nullptr);
    ~add_Arenda();

    bool is_edit;

    int state;

    int ArendaId;

    QString inv;
    QString naim;
    QString type;
    QString zav;
    QString param;
    QString reg_num;
    QString izg;
    QString sobst;
    QString diap;
    QString pov_att;
    QString dates;

    void set_edit();
    void set_data();

private slots:
    void on_radioButton_SI_clicked();
    void on_radioButton_VO_clicked();
    void on_radioButton_IO_clicked();
    void on_checkBox_stateChanged(int arg1);
    void on_pushButton_cancel_clicked();
    void on_pushButton_add_d_clicked();
    void on_pushButton_rem_d_clicked();
    void on_pushButton_add_p_clicked();
    void on_pushButton_rem_p_clicked();

    void on_pushButton_save_clicked();

    void on_pushButton_edit_clicked();

    void on_pushButton_delete_clicked();

private:
    Ui::add_Arenda *ui;

    QList<QHBoxLayout*> layouts_d_List;
    QList<QHBoxLayout*> layouts_p_List;
    QString get_d_Text(QHBoxLayout *hLayout) const;
    QString get_p_Text(QHBoxLayout *hLayout) const;
    void collect_info();
};

#endif // ADD_ARENDA_H
