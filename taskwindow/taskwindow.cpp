#include "taskwindow.h"
#include "ui_taskwindow.h"
//#include "choicewindow.h"
#include <QMessageBox>

//choicewindow *ct;

taskwindow::taskwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::taskwindow)
{
    ui->setupUi(this);

    QFile file("StudyPlannerTask.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            ui->tasklist->addItem(line); // Add each line to the list widget
        }
        file.close();
    }

    istaskediting = false;

    connect(ui->addtask, &QPushButton::clicked, this, &taskwindow::addtaskbutton);
    connect(ui->edittask, &QPushButton::clicked, this, &taskwindow::edittaskbutton);
    connect(ui->removetask, &QPushButton::clicked, this, &taskwindow::removetaskbutton);
    connect(ui->tasklist, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updatetasklist()));

    //connect(ui->tochoicet, SIGNAL(clicked()), this, SLOT(tochoicet()));
}

taskwindow::~taskwindow()
{
    delete ui;
}

void taskwindow::addtaskbutton()
{
    QString tName = ui->taskname->text();
    QString tDescription = ui->taskdescription->toPlainText();

    if (tName.isEmpty() || tDescription.isEmpty()) {
            QMessageBox::warning(this, "Error", "Study plan name and description cannot be empty.");
            return;
        }

    QString tasktext = QString::number(ui->tasklist->count() + 1) + ". " + tName + " - " + tDescription;

    ui->tasklist->addItem(tasktext);

    QFile file("StudyPlannerTask.txt");
    if(file.open(QIODevice::Append | QIODevice::Text)){
        QTextStream out(&file);
        out << tasktext << "\n";

        file.close();
    }

    ui->taskname->clear();
    ui->taskdescription->clear();

    updatetasklist();
}


void taskwindow::edittaskbutton()
{
    QListWidgetItem *selectedItem = ui->tasklist->currentItem();
    if(!selectedItem){
        QMessageBox::warning(this, "Error", "Please select a task to edit.");
        return;
    }

    if(!istaskediting){
        QString tasktext = selectedItem->text();
        QString studyContent = tasktext.mid(tasktext.indexOf(".") + 2);
        ui->taskname->setText(tasktext.split(" - ")[0]);
        ui->taskdescription->setText(tasktext.split(" - ")[1].split(" (")[0]);
        istaskediting = true;
        ui->edittask->setText("SAVE");
    } else {
        // Exit edit mode: Save the edited task back to the list
        QString newName = ui->taskname->text();
        QString newDescription = ui->taskdescription->toPlainText();

        if (newName.isEmpty() || newDescription.isEmpty()) {
            QMessageBox::warning(this, "Error", "Task name and description cannot be empty.");
            return;
        }

        // Update the selected task with the new name and description
        QString updatedTask = newName + " - " + newDescription;
        selectedItem->setText(updatedTask);

        QFile file("StudyPlannerTask.txt");
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QStringList lines;
            QTextStream in(&file);
            while (!in.atEnd()) {
                lines << in.readLine(); // Read all lines from the file
            }

                    // Replace the old study plan with the updated one
            int index = ui->tasklist->row(selectedItem);
            if (index >= 0 && index < lines.size()) {
                lines[index] = updatedTask;
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
        ui->taskname->clear();
        ui->taskdescription->clear();
        istaskediting = false; // Switch back to non-edit mode
        ui->edittask->setText("EDIT"); // Change button text back to "Edit"
    }

    updatetasklist();
}


void taskwindow::removetaskbutton()
{
    QListWidgetItem *selectedItem = ui->tasklist->currentItem();
    if(selectedItem){
        int index = ui->tasklist->row(selectedItem);
        delete selectedItem;

        QFile file("StudyPlannerTask.txt");
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

    updatetasklist();
}


void taskwindow::updatetasklist()
{
    for (int i = 0; i < ui->tasklist->count(); ++i) {
        QListWidgetItem *item = ui->tasklist->item(i);
        QString itemText = item->text();

            // Remove the existing number prefix (e.g., "1. ")
        QString content = itemText.mid(itemText.indexOf(".") + 2);

            // Add the new number prefix
        item->setText(QString::number(i + 1) + ". " + content);
    }
}


//void taskwindow::tochoicet()
//{
//    hide();
//    ct = new choicewindow;
//    ct->show();
//}

