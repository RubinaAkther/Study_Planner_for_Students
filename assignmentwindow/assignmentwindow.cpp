#include "assignmentwindow.h"
#include "ui_assignmentwindow.h"
//#include "choicewindow.h"
#include <QMessageBox>
#include <QTimer>
#include <QDebug>

//choicewindow *ca;

assignmentwindow::assignmentwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::assignmentwindow)
{
    ui->setupUi(this);

    QFile file("StudyPlannerAssignment.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            ui->listwidget_assignmentlist->addItem(line); // Add each line to the list widget
        }
        file.close();
    }

    isassignmentediting = false;

    QTimer *areminderTimer = new QTimer(this);
    connect(areminderTimer, &QTimer::timeout, this, &assignmentwindow::checkassignmentReminders);
    areminderTimer->start(60000);

    connect(ui->button_assignmentadd, &QPushButton::clicked, this, &assignmentwindow::addassignment);
    connect(ui->button_assignmentedit, &QPushButton::clicked, this, &assignmentwindow::editassignment);
    connect(ui->button_assignmentremove, &QPushButton::clicked, this, &assignmentwindow::removeassignment);
    connect(ui->listwidget_assignmentlist, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updateassignmentlist()));

    //connect(ui->tochoicea, SIGNAL(clicked()), this, SLOT(tochoicea()));
}

assignmentwindow::~assignmentwindow()
{
    delete ui;
}

void assignmentwindow::addassignment()
{
    QString aname = ui->lineedit_assignmentname->text();
    QString adescription = ui->textedit_assignmentdes->toPlainText();
    QDateTime adueDateTime = ui->datetime_assignment->dateTime();

    if (aname.isEmpty() || adescription.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter both a name and a description.");
        return;
    }

        // Format the assignment details
    QString assignmentText = QString::number(ui->listwidget_assignmentlist->count() + 1) + ". " + aname + " - " + adescription + " (Due: " + adueDateTime.toString("dd.MM.yyyy hh : mm AP") + ")";

        // Add the assignment to the list widg
    ui->listwidget_assignmentlist->addItem(assignmentText);

    QFile file("StudyPlannerAssignment.txt");
    if(file.open(QIODevice::Append | QIODevice::Text)){
        QTextStream out(&file);
        out << assignmentText << "\n";

        file.close();
    }

        // Clear input fields
    ui->lineedit_assignmentname->clear();
    ui->textedit_assignmentdes->clear();
    ui->datetime_assignment->setDateTime(QDateTime::currentDateTime());

    updateassignmentlist();
}


void assignmentwindow::editassignment()
{
    QListWidgetItem *selectedItem = ui->listwidget_assignmentlist->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "Error", "Please select an assignment to edit.");
        return;
    }

    if (!isassignmentediting) {
            // Enter edit mode: Load the selected assignment into the input fields
        QString assignmentText = selectedItem->text();
        QStringList parts = assignmentText.split(" - ");
        QString aname = parts[0];
        QString adescription = parts[1].split(" (Due: ")[0];
        QString adueDateTimeString = parts[1].split(" (Due: ")[1].remove(")");
        QString studyContent = assignmentText.mid(assignmentText.indexOf(".") + 2);

        ui->lineedit_assignmentname->setText(aname);
        ui->textedit_assignmentdes->setText(adescription);
        ui->datetime_assignment->setDateTime(QDateTime::fromString(adueDateTimeString, "dd.MM.yyyy hh : mm AP"));

        isassignmentediting = true; // Switch to edit mode
        ui->button_assignmentedit->setText("SAVE"); // Change button text to "Save"
    } else {
            // Exit edit mode: Save the edited assignment back to the list
        QString aname = ui->lineedit_assignmentname->text().trimmed();
        QString adescription = ui->textedit_assignmentdes->toPlainText().trimmed();
        QDateTime adueDateTime = ui->datetime_assignment->dateTime();


        if (aname.isEmpty() || adescription.isEmpty()) {
            QMessageBox::warning(this, "Error", "Please enter both a name and a description.");
            return;
        }

            // Format the updated assignment details
        QString updatedAssignmentText = aname + " - " + adescription + " (Due: " + adueDateTime.toString("dd.MM.yyyy hh : mm AP") + ")";

            // Update the selected assignment
        selectedItem->setText(updatedAssignmentText);

        QFile file("StudyPlannerAssignment.txt");
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QStringList lines;
            QTextStream in(&file);
            while (!in.atEnd()) {
                lines << in.readLine(); // Read all lines from the file
            }

                    // Replace the old study plan with the updated one
            int index = ui->listwidget_assignmentlist->row(selectedItem);
            if (index >= 0 && index < lines.size()) {
                lines[index] = updatedAssignmentText;
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
        ui->lineedit_assignmentname->clear();
        ui->textedit_assignmentdes->clear();
        ui->datetime_assignment->setDateTime(QDateTime::currentDateTime());
        isassignmentediting = false; // Switch back to non-edit mode
        ui->button_assignmentedit->setText("EDIT"); // Change button text back to "Edit"
    }

    updateassignmentlist();
}


void assignmentwindow::removeassignment()
{
    QListWidgetItem *currentItem = ui->listwidget_assignmentlist->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "Remove Assignment", "No assignment selected.");
        return;
    }

    // Get the index of the selected item
    int index = ui->listwidget_assignmentlist->row(currentItem);

    // Remove the item from the list widget
    delete currentItem;

    QFile file("StudyPlannerAssignment.txt"); // Replace with your file name
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QStringList lines;
        QTextStream in(&file);

        // Read all lines from the file
        while (!in.atEnd()) {
            lines << in.readLine();
        }

        // Remove the line corresponding to the deleted assignment
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

    // Update the assignment list (re-number the items)
    updateassignmentlist();
}


void assignmentwindow::checkassignmentReminders()
{
    QDateTime now = QDateTime::currentDateTime();
    for (int i = 0; i < ui->listwidget_assignmentlist->count(); ++i)
    {
        QString assignmentText = ui->listwidget_assignmentlist->item(i)->text();
        QString dueDateStr = assignmentText.split(" - ").last().trimmed();
        QDateTime dueDate = QDateTime::fromString(dueDateStr, "dd.MM.yyyy hh : mm");

            // Calculate 24 hours before the due time
        QDateTime reminderTime = dueDate.addSecs(-86400); // Subtract 24 hours (86400 seconds)

            // Check if the current time is within 24 hours before the due time
        if (now >= reminderTime && now < dueDate)
        {
            QMessageBox::information(this, "Assignment Reminder", "Reminder: " + assignmentText + " is due in 24 hours.");
            ui->listwidget_assignmentlist->item(i)->setText(assignmentText + " (Due in 24 hours)");
        }
            // Check if the assignment is already due
        else if (now >= dueDate)
        {
            QMessageBox::information(this, "Assignment Due", "Reminder: " + assignmentText + " is due now.");
            ui->listwidget_assignmentlist->item(i)->setText(assignmentText + " (Due)");
        }
    }
}

void assignmentwindow::updateassignmentlist()
{
    for (int i = 0; i < ui->listwidget_assignmentlist->count(); ++i) {
        QListWidgetItem *item = ui->listwidget_assignmentlist->item(i);
        QString itemText = item->text();

            // Remove the existing number prefix (e.g., "1. ")
        QString content = itemText.mid(itemText.indexOf(".") + 2);

            // Add the new number prefix
        item->setText(QString::number(i + 1) + ". " + content);
    }
}


//void assignmentwindow::tochoicea()
//{
//    hide();
//    ca = new choicewindow;
//    ca->show();
//}

