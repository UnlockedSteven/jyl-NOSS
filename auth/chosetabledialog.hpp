#ifndef CHOSETABLE_DLG_X_HPP
#define CHOSETABLE_DLG_X_HPP

#include <QDialog>
#include <QProcess>
#include <QFile>
#include <QDir>

namespace Ui {
class choseTableDialog;
}

class choseTableDialog : public QDialog
{
    Q_OBJECT

public:
    ~choseTableDialog();
    explicit choseTableDialog(QWidget *parent = nullptr);

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

private:
    Ui::choseTableDialog *ui;
    QList<QString> tempLogList;
};

#endif