//mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "database.h"
#include <QMainWindow>
#include <QtSql>
#include <QHBoxLayout>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_SI_clicked();
    void on_pushButton_IO_clicked();
    void on_pushButton_VO_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_pushButton_pov_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_pushButton_file_clicked();

    void on_pushButton_staff_clicked();

    void on_pushButton_arenda_clicked();

    void on_pushButton_prtkl_clicked();

    void on_pushButton_form_clicked();

    void on_pushButton_type_clicked();

    void on_pushButton_2_clicked();

private:
    int table;
    void set_table_columns();
    Ui::MainWindow *ui;
    QSqlQueryModel *sqlmodel;
    QStandardItemModel *model;
    void set_table(const QString table_name);
    void create_directory_for_eqipment(QVector <QStringList> &equipmentList, int type);

    QVector <QStringList> SIList;
    QVector <QStringList> IOList;
    QVector <QStringList> VOList;

    QList<QHBoxLayout*> layouts_SI_List;
    QList<QHBoxLayout*> layouts_SI_List_center;
    QList<QHBoxLayout*> layouts_SI_List_bottom;

    QList<QHBoxLayout*> layouts_IO_List;
    QList<QHBoxLayout*> layouts_IO_List_center;
    QList<QHBoxLayout*> layouts_IO_List_bottom;

    QList<QHBoxLayout*> layouts_VO_List;
    QList<QHBoxLayout*> layouts_VO_List_center;
    QList<QHBoxLayout*> layouts_VO_List_bottom;

    QList<QHBoxLayout*> layouts_Arenda_List;

    void fill_layout_SI(const QStringList data, QList<QHBoxLayout*> &layouts_List);
    void fill_layout_IO(const QStringList data, QList<QHBoxLayout*> &layouts_List);
    void fill_layout_VO(const QStringList data, QList<QHBoxLayout*> &layouts_List);
    void fill_layout_Arenda(const QStringList data, QList<QHBoxLayout*> &layouts_List);
    void clean_layout();

};

// ClickableLabel.h
#include <QLabel>
#include <QWidget>
#include <QMouseEvent>

class ClickableLabel : public QLabel {
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

protected:
    void mousePressEvent(QMouseEvent* event) override;
};

#endif // MAINWINDOW_H
