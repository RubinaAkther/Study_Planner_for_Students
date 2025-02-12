#include "examwindow.h"
#include "ui_examwindow.h"
#include "choicewindow.h"
#include <QMessageBox>
#include <QTimer>

choicewindow *ce;

examwindow::examwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::examwindow)
{
    ui->setupUi(this);

    QFile file("StudyPlannerExam.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            ui->listwidget_examlist->addItem(line); // Add each line to the list widget
        }
        file.close();
    }

    isexamediting = false;

    QTimer *ereminderTimer = new QTimer(this);
    connect(ereminderTimer, &QTimer::timeout, this, &examwindow::checkexamReminders);
    ereminderTimer->start(60000);

    connect(ui->button_examadd, &QPushButton::clicked, this, &examwindow::addexambutton);
    connect(ui->button_examedit, &QPushButton::clicked, this, &examwindow::editexambutton);
    connect(ui->button_examremove, &QPushButton::clicked, this, &examwindow::removeexambutton);
    connect(ui->listwidget_examlist, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updateexamlist()));

    connect(ui->tochoicee, SIGNAL(clicked()), this, SLOT(tochoicee()));
}

examwindow::~examwindow()
{
    delete ui;
}

void examwindow::addexambutton()
{
    QString ename = ui->lineedit_examname->text();
    QString edescription = ui->textedit_examdes->toPlainText();
    QDateTime edueDateTime = ui->datetime_exam->dateTime();

    if (ename.isEmpty() || edescription.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter both a name and a description.");
        return;
    }

        // Format the assignment details
    QString examText = QString::number(ui->listwidget_examlist->count() + 1) + ". " + ename + " - " + edescription + " (Due: " + edueDateTime.toString("dd.MM.yyyy hh : mm AP") + ")";

        // Add the assignment to the list widg
    ui->listwidget_examlist->addItem(examText);

    QFile file("StudyPlannerExam.txt");
    if(file.open(QIODevice::Append | QIODevice::Text)){
        QTextStream out(&file);
        out << examText << "\n";

        file.close();
    }

        // Clear input fields
    ui->lineedit_examname->clear();
    ui->textedit_examdes->clear();
    ui->datetime_exam->setDateTime(QDateTime::currentDateTime());

    updateexamlist();
}


void examwindow::editexambutton()
{
    QListWidgetItem *selectedItem = ui->listwidget_examlist->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "Error", "Please select an exam study plan to edit.");
        return;
    }

    if (!isexamediting) {
            // Enter edit mode: Load the selected assignment into the input fields
        QString examText = selectedItem->text();
        QStringList parts = examText.split(" - ");
        QString ename = parts[0];
        QString edescription = parts[1].split(" (Due: ")[0];
        QString edueDateTimeString = parts[1].split(" (Due: ")[1].remove(")");
        QString studyContent = examText.mid(examText.indexOf(".") + 2);

        ui->lineedit_examname->setText(ename);
        ui->textedit_examdes->setText(edescription);
        ui->datetime_exam->setDateTime(QDateTime::fromString(edueDateTimeString, "dd.MM.yyyy hh : mm AP"));

        isexamediting = true; // Switch to edit mode
        ui->button_examedit->setText("SAVE"); // Change button text to "Save"
    } else {
            // Exit edit mode: Save the edited assignment back to the list
        QString ename = ui->lineedit_examname->text().trimmed();
        QString edescription = ui->textedit_examdes->toPlainText().trimmed();
        QDateTime edueDateTime = ui->datetime_exam->dateTime();


        if (ename.isEmpty() || edescription.isEmpty()) {
            QMessageBox::warning(this, "Error", "Please enter both a name and a description.");
            return;
        }

            // Format the updated assignment details
        QString updatedexamText = ename + " - " + edescription + " (Due: " + edueDateTime.toString("dd.MM.yyyy hh : mm AP") + ")";

            // Update the selected assignment
        selectedItem->setText(updatedexamText);

        QFile file("StudyPlannerExam.txt");
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QStringList lines;
            QTextStream in(&file);
            while (!in.atEnd()) {
                lines << in.readLine(); // Read all lines from the file
            }

                    // Replace the old study plan with the updated one
            int index = ui->listwidget_examlist->row(selectedItem);
            if (index >= 0 && index < lines.size()) {
                lines[index] = updatedexamText;
            }

                    // Write the updated lines back to the file
            file.resize(0); // Clear the file content
            QTextStream out(&file);
            for (const QString &line : lines) {
                out << line << "\n";
            }
            file.close();
        }

            // Clear input fields and reset button state
        ui->lineedit_examname->clear();
        ui->textedit_examdes->clear();
        ui->datetime_exam->setDateTime(QDateTime::currentDateTime());
        isexamediting = false; // Switch back to non-edit mode
        ui->button_examedit->setText("EDIT"); // Change button text back to "Edit"
    }

    updateexamlist();
}


void examwindow::removeexambutton()
{
    QListWidgetItem *selectedItem = ui->listwidget_examlist->currentItem();
    if (selectedItem)
    {
        int index = ui->listwidget_examlist->row(selectedItem);
        delete selectedItem;

        QFile file("StudyPlannerExam.txt");
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QStringList lines;
            QTextStream in(&file);
            while (!in.atEnd()) {
                lines << in.readLine(); // Read all lines from the file
            }

                    // Remove the deleted study plan from the file
            if (index >= 0 && index < lines.size()) {
                lines.removeAt(index);
            }

                    // Write the updated lines back to the file
            file.resize(0); // Clear the file content
            QTextStream out(&file);
            for (const QString &line : lines) {
                out << line << "\n";
            }
            file.close();
        } 
    }

    updateexamlist();
}

void examwindow::checkexamReminders()
{
    QDateTime now = QDateTime::currentDateTime();
    for (int i = 0; i < ui->listwidget_examlist->count(); ++i)
    {
        QString examText = ui->listwidget_examlist->item(i)->text();
        QString edueDateStr = examText.split(" - ").last().trimmed();
        QDateTime edueDate = QDateTime::fromString(edueDateStr, "dd.MM.yyyy hh : mm : ss");

        if (now >= edueDate)
        {
            QMessageBox::information(this, "Exam Due", "Reminder: " + examText);
            ui->listwidget_examlist->item(i)->setText(examText + " (Due)");
        }
    }
}


void examwindow::updateexamlist()
{
    for (int i = 0; i < ui->listwidget_examlist->count(); ++i) {
        QListWidgetItem *item = ui->listwidget_examlist->item(i);
        QString itemText = item->text();

            // Remove the existing number prefix (e.g., "1. ")
        QString content = itemText.mid(itemText.indexOf(".") + 2);

            // Add the new number prefix
        item->setText(QString::number(i + 1) + ". " + content);
    }
}


void examwindow::tochoicee()
{
    hide();
    ce = new choicewindow;
    ce->show();
}

