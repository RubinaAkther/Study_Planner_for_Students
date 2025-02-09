#include "studyplan.h"
#include "ui_studyplan.h"
//#include "choicewindow.h"
#include <QMessageBox>

//choicewindow *cs;

studyplan::studyplan(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::studyplan)
{
    ui->setupUi(this);

    isstudyediting = false;

    connect(ui->button_addstudy, &QPushButton::clicked, this, &studyplan::addstudybutton);
    connect(ui->button_studyedit, &QPushButton::clicked, this, &studyplan::editstudybutton);
    connect(ui->button_studyremove, &QPushButton::clicked, this, &studyplan::removestudybutton);
    connect(ui->listwidget_studylist, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updatestudylist()));

    //connect(ui->tochoices, SIGNAL(clicked()), this, SLOT(tochoices()));
}

studyplan::~studyplan()
{
    delete ui;
}


void studyplan::addstudybutton()
{
    QString sName = ui->linedit_studyname->text();
    QString sDescription = ui->textedit_studydescription->toPlainText();

    QString studyplantext = sName + " - " + sDescription;

    ui->listwidget_studylist->addItem(studyplantext);

    ui->linedit_studyname->clear();
    ui->textedit_studydescription->clear();

    updatestudylist();
}


void studyplan::editstudybutton()
{
    QListWidgetItem *selectedItem = ui->listwidget_studylist->currentItem();
    if(!selectedItem){
        QMessageBox::warning(this, "Error", "Please select a study plan to edit.");
        return;
    }

    if(!isstudyediting){
        QString studytext = selectedItem->text();
        ui->linedit_studyname->setText(studytext.split(" - ")[0]);
        ui->textedit_studydescription->setText(studytext.split(" - ")[1].split(" (")[0]);
        isstudyediting = true;
        ui->button_studyedit->setText("Save");
    } else {
        // Exit edit mode: Save the edited task back to the list
        QString newName = ui->linedit_studyname->text().trimmed();
        QString newDescription = ui->textedit_studydescription->toPlainText().trimmed();

        if (newName.isEmpty() || newDescription.isEmpty()) {
            QMessageBox::warning(this, "Error", "Study plan name and description cannot be empty.");
            return;
        }

        // Update the selected task with the new name and description
        QString updatedplan = newName + " - " + newDescription;
        selectedItem->setText(updatedplan);

        // Clear input fields and reset button state
        ui->linedit_studyname->clear();
        ui->textedit_studydescription->clear();
        isstudyediting = false; // Switch back to non-edit mode
        ui->button_studyedit->setText("Edit"); // Change button text back to "Edit"
    }

    updatestudylist();
}


void studyplan::removestudybutton()
{
    QListWidgetItem *selectedItem = ui->listwidget_studylist->currentItem();
    if(selectedItem){
        delete selectedItem;
    }

    updatestudylist();
}


void studyplan::updatestudylist()
{
    ui->listwidget_studylist->sortItems();
}


//void planwindow::tochoices()
//{
//    hide();
//    cs = new choicewindow;
//    cs->show();
//}
