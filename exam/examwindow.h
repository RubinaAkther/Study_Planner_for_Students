#ifndef EXAMWINDOW_H
#define EXAMWINDOW_H

#include <QDialog>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QTextStream>

namespace Ui {
class examwindow;
}

class examwindow : public QDialog
{
    Q_OBJECT

public:
    explicit examwindow(QWidget *parent = nullptr);
    ~examwindow();

    void checkexamReminders();

private slots:
    void addexambutton();
    void editexambutton();
    void removeexambutton();
    void updateexamlist();

    void tochoicee();

private:
    Ui::examwindow *ui;

    bool isexamediting;
};

#endif // EXAMWINDOW_H
