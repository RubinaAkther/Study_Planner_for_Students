#ifndef ASSIGNMENTWINDOW_H
#define ASSIGNMENTWINDOW_H

#include <QDialog>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QTextStream>

namespace Ui {
class assignmentwindow;
}

class assignmentwindow : public QDialog
{
    Q_OBJECT

public:
    explicit assignmentwindow(QWidget *parent = nullptr);
    ~assignmentwindow();

    void checkassignmentReminders();

private slots:
    void addassignment();
    void editassignment();
    void removeassignment();
    void updateassignmentlist();

    void tochoicea();

private:
    Ui::assignmentwindow *ui;

    bool isassignmentediting;
};

#endif // ASSIGNMENTWINDOW_H
