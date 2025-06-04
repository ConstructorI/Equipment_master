#ifndef ADD_PRTKL_H
#define ADD_PRTKL_H

#include <QDialog>

namespace Ui {
class add_Prtkl;
}

class add_Prtkl : public QDialog
{
    Q_OBJECT

public:
    explicit add_Prtkl(QWidget *parent = nullptr);
    ~add_Prtkl();

private:
    Ui::add_Prtkl *ui;
};

#endif // ADD_PRTKL_H
