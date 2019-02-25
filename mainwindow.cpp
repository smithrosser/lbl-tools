#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("LbL Tools " + version);
    initEdit();
}

MainWindow::~MainWindow() {
    delete ui;
}

// MENU BAR METHODS
void MainWindow::on_action_New_triggered() {
    session.clear();
    ui->sessionList->clear();
}

void MainWindow::on_action_Open_triggered() {
    int type = -1, dur = -1, pump = -1;			// temporary Stage members
    QString data, cmd;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open existing session..."), "",
                                                     "LbL session (*.lbl);;All files (*.*)");	// open file dialog
    QFile inputFile(fileName);
    if(fileName.isEmpty() && fileName.isNull())
         return;
    else if (inputFile.open(QIODevice::ReadOnly))
    {
        ui->sessionList->clear();
        session.clear();
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();		// read file line by line
            cmd.clear();

            // PARSING ALGORITHM: syntax for deposition stage is <type>:<pump>,<duration>; or <type>:<duration>;
            // Examples: f:1,30; w:30; d:25; (each command on new line!)

            for( int i = 0; i < line.size(); i++ ) {	// goes through characters of line
                if( line.at(i) == ':' ) {				// colon indicates start of command arguments
                    if(cmd == "f")
                        type = FILL;
                    else if(cmd == "w")
                        type = WASH;
                    else if(cmd == "d")
                        type = DRY;
                    else
                        type = -1;
                    cmd.clear();
                } else if( line.at(i) == ',' ) {		// arguments are separated by commas
                    pump = cmd.toInt();
                    cmd.clear();
                } else if( line.at(i) == ';' ) {		// semicolon means end of arguments
                    dur = cmd.toInt();
                    switch(type) {
                    case FILL : session.push_back(Stage(FILL, dur, pump)); break;
                    case WASH : session.push_back(Stage(WASH, dur)); break;
                    case DRY : session.push_back(Stage(DRY, dur)); break;
                    default :
                    QMessageBox::critical(this, tr("Error"), tr("Unable to parse file (check syntax!)")); return;
                }
                cmd.clear();
                } else
                    cmd.append(line.at(i));		// anything between ;:, chars are stored in 'cmd'
            }
       }
       inputFile.close();
       listUpdate();
       ui->sessionList->setCurrentRow(0);
       editPaneUpdate();
    }
}

void MainWindow::on_actionSave_As_triggered() {
    QString line;
    QString fileName = QFileDialog::getSaveFileName(this, "Save session as...", ".lbl", "LbL session (*.lbl);; All files (*.*)");
    QFile file(fileName);
    if(fileName.isEmpty() && fileName.isNull())
         return;
    else if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot save file: ", file.errorString());
        return;
    }
    file.resize(0);
    QTextStream out(&file);
    for(int i=0; i<session.size(); i++) {
        switch(session[i].getType()) {
        case FILL : line = "f:" + QString::number(session[i].getPump()) + "," + QString::number(session[i].getDur()) + ";"; break;
        case WASH : line = "w:" + QString::number(session[i].getDur()) + ";"; break;
        case DRY :  line = "d:" + QString::number(session[i].getDur()) + ";"; break;
        default : break;
        }
        out << line << endl;
    }
    file.close();
}

void MainWindow::on_actionExit_triggered() {
    QApplication::quit();
}

// BUTTON METHODS
void MainWindow::on_buttonUp_clicked() {
    if( session.isEmpty() )
        return;
    int pos = ui->sessionList->currentRow();
    if( pos > 0 ) {
        session.move(pos, pos-1);
        listUpdate();
        ui->sessionList->setCurrentRow(pos-1);
    }
    editPaneUpdate();
}

void MainWindow::on_buttonDown_clicked() {
    if( session.isEmpty() )
        return;
    int pos = ui->sessionList->currentRow();
    if( pos < session.size()-1 ) {
        session.move(pos, pos+1);
        listUpdate();
        ui->sessionList->setCurrentRow(pos+1);
    }
    editPaneUpdate();
}

void MainWindow::on_buttonAdd_clicked() {
    int pos = ui->sessionList->currentRow();
    dialogAdd aDialog;
    aDialog.setModal(true);
    if( aDialog.exec() == QDialog::Accepted ) {
        if( aDialog.getType() == FILL ) {
           session.insert(pos+1, Stage(aDialog.getType(),aDialog.getDur(),aDialog.getPump()));
           listInsert(pos+1, Stage(aDialog.getType(),aDialog.getDur(),aDialog.getPump()));
        } else {
           session.insert(pos+1, Stage(aDialog.getType(),aDialog.getDur()));
           listInsert(pos+1, Stage(aDialog.getType(),aDialog.getDur()));
        }
        ui->sessionList->setCurrentRow(pos+1);
        editPaneUpdate();
    }
}

void MainWindow::on_buttonRemove_clicked() {
    if(!session.isEmpty()) {
        int pos = ui->sessionList->currentRow();
        session.removeAt(ui->sessionList->currentRow());
        listUpdate();
        if(pos < 1)
            ui->sessionList->setCurrentRow(0);
        else {
            ui->sessionList->setCurrentRow(pos-1);
            editPaneUpdate();
        }
    }
}

void MainWindow::on_buttonCopy_clicked() {
    if(!session.isEmpty()) {
        int pos = ui->sessionList->currentRow();
        session.insert(pos+1, session[pos]);
        listUpdate();
        ui->sessionList->setCurrentRow(pos+1);
    }
}

void MainWindow::on_sessionList_itemClicked(QListWidgetItem *item) {
    editPaneUpdate();
}

// EDIT PANE METHODS
void MainWindow::editPaneUpdate() {
    int pos = ui->sessionList->currentRow();
    if(session[pos].getType() == FILL) {
        ui->comboPump->setCurrentIndex(session[pos].getPump());
        ui->comboPump->setEnabled(true);
    } else
        ui->comboPump->setEnabled(false);
        ui->comboType->setCurrentIndex(session[pos].getType()-1);
        ui->editDur->setText(QString::number(session[pos].getDur()));
}

void MainWindow::on_comboType_activated(int index) {
    int pos = ui->sessionList->currentRow();
    if(session.size() > 0) {
        session[pos].setType(index + 1);
        if(session[pos].getPump() < 0)
            session[pos].setPump(0);
        listUpdate();
        ui->sessionList->setCurrentRow(pos);
        editPaneUpdate();
     }
}

void MainWindow::on_comboPump_activated(int index) {
    int pos = ui->sessionList->currentRow();
    if(session.size() > 0) {
        session[pos].setPump(index);
        listUpdate();
        ui->sessionList->setCurrentRow(pos);
        editPaneUpdate();
    }
}

void MainWindow::on_editDur_textEdited(const QString &arg1) {
    int pos = ui->sessionList->currentRow();
    if(session.size() > 0) {
        session[pos].setDur(arg1.toInt());
        listUpdate();
        ui->sessionList->setCurrentRow(pos);
    }

}

// OTHER METHODS
void MainWindow::listInsert(int index, Stage s) {
    QString item;
    switch(s.getType()) {
        case FILL : item = "Fill\t" + QString::number(s.getDur()) + " sec\tPump " + QString::number(s.getPump()); break;
        case WASH : item = "Wash\t" + QString::number(s.getDur()) + " sec"; break;
        case DRY : item = "Dry\t" + QString::number(s.getDur()) + " sec"; break;
        default : break;
    }
    ui->sessionList->insertItem(index, item);
}

void MainWindow::listUpdate() {
    ui->sessionList->clear();
    for(int i=0; i<session.size(); i++)
        listInsert(i, session[i]);
}

void MainWindow::initEdit() {
    ui->comboType->addItem("Fill");
    ui->comboType->addItem("Wash");
    ui->comboType->addItem("Dry");

    ui->comboPump->addItem("Pump 0");
    ui->comboPump->addItem("Pump 1");
    ui->comboPump->addItem("Pump 2");

    ui->editDur->setValidator( new QIntValidator(1, 1000, this) );
}

void MainWindow::on_sessionList_activated(const QModelIndex &index) {
    editPaneUpdate();
}
