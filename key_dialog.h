#ifndef KEY_DIALOG_H
#define KEY_DIALOG_H

#include <QDialog>

namespace Ui {
class key_dialog;
}

class key_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit key_dialog(QWidget *parent = nullptr);
    ~key_dialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::key_dialog *ui;
};

#endif // KEY_DIALOG_H
