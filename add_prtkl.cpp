#include "add_prtkl.h"
#include "ui_add_prtkl.h"

add_Prtkl::add_Prtkl(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::add_Prtkl)
{
    ui->setupUi(this);
}

add_Prtkl::~add_Prtkl()
{
    delete ui;
}
