#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("LbL Tools beta" + version);

    device.init();

    initEdit();
    updateSave();
    updateButtons();
}

MainWindow::~MainWindow() {
    delete ui;
}

// FILE HANDLERS

void MainWindow::openSession() {

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open existing session..."), "",
                                                     "LbL session (*.lbl);;All files (*.*)");	// open file dialog

    if( fileName.isEmpty() || fileName.isNull() )
        return;

    currentFile = fileName;
    int open = openSessionFile(fileName, session);
    if( open == OPEN_SUCCESS ) {

        updateConsole("Opened LbL session \"" + getCurrentFileName() + "\"");

        listUpdate();
        ui->sessionList->setCurrentRow(0);
        editPaneUpdate();

    }
    else {
        updateConsole( (open < 0) ? "Couldn't open file"
                                  : "Syntax error at line " + QString::number(open) + " of \"" + getCurrentFileName() + "\"" );
        currentFile = "";
    }

    listUpdate();
    updateSave();
    updateButtons();
}

void MainWindow::saveSession( QString fileName ) {

    if( fileName != nullptr )
        fileName = currentFile;
    else
        fileName = QFileDialog::getSaveFileName(this, "Save session as...", ".lbl", "LbL session (*.lbl);; All files (*.*)");

    int save = saveSessionFile(fileName, session);
    if( save == SAVE_SUCCESS ) {
        currentFile = fileName;
        updateConsole("Saved LbL session \"" + getCurrentFileName() + "\"");
        updateSave();
    }
    else
        updateConsole("Couldn't save file");
}

void MainWindow::updateSave() {
    ui->action_Save->setEnabled(false);
    if(!currentFile.isEmpty())
        ui->action_Save->setEnabled(true);

    MainWindow::setWindowTitle(getCurrentFileName() + " - LbL Tools beta " + version);
}

QString MainWindow::getCurrentFileName() {
    QFileInfo cF(QFile(currentFile).fileName());
    return ( cF.fileName().isEmpty() ) ? "new session" : cF.fileName();
}

// MENU BAR METHODS

void MainWindow::on_action_New_triggered() {
    session.clear();
    ui->sessionList->clear();
    currentFile.clear();

    updateConsole("Created new LbL session");
    updateSave();
}

void MainWindow::on_action_Open_triggered() {

    openSession();
}

void MainWindow::on_actionSave_As_triggered() {
    saveSession( nullptr );
}

void MainWindow::on_action_Save_triggered() {
    if(!currentFile.isEmpty())
        saveSession( currentFile );
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
        if( aDialog.getType() == STAGE_FILL ) {
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

void MainWindow::on_buttonDetect_clicked() {
    setupDevice();
}

void MainWindow::on_buttonStart_clicked() {

    isStart = true;
    setEnableUi(false);
    updateConsole("Waiting for device on " + device.get()->portName() + "...");
    sendSession();
}

void MainWindow::on_sessionList_itemClicked() {
    editPaneUpdate();
}

// EDIT PANE METHODS

void MainWindow::initEdit() {
    ui->comboType->addItem("Fill");
    ui->comboType->addItem("Wash");
    ui->comboType->addItem("Dry");

    ui->comboPump->addItem("Pump 0");
    ui->comboPump->addItem("Pump 1");
    ui->comboPump->addItem("Pump 2");

    ui->editDur->setValidator( new QIntValidator(1, 1000, this) );
}

void MainWindow::editPaneUpdate() {
    int pos = ui->sessionList->currentRow();
    ui->comboType->setCurrentIndex(session[pos].getType());
    ui->editDur->setText(QString::number(session[pos].getDur()));

    if(session[pos].getType() == STAGE_FILL) {
        ui->comboPump->setCurrentIndex(session[pos].getPump());
        ui->comboPump->setEnabled(true);
    } else {
        ui->comboPump->setEnabled(false);
           }
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

// CONSOLE METHODS

void MainWindow::updateConsole( QString str ) {
    QTextCursor tc(ui->console->textCursor());
    tc.setPosition(ui->console->document()->characterCount()-1);

    ui->console->appendPlainText(str);
    ui->console->setTextCursor(tc);
}

// GENERAL UI METHODS

void MainWindow::setEnableUi(bool state) {
    ui->verticalLayoutWidget->setEnabled(state);
    ui->groupBox->setEnabled(state);
    ui->groupBox_3->setEnabled(state);
}

void MainWindow::updateSessionInfo() {
    int sum = 0;
    int count = 0;
    int volume[3];
    volume[0] = volume[1] = volume[2] = 0;

    for(int i=0; i<session.size(); i++) {
        sum += session[i].getDur();
        count++;

        if(session[i].getType() == STAGE_FILL)
            volume[session[i].getPump()]++;
    }

    QString time = QString::number(sum/60) + " min " + QString::number(sum % 60) + " sec";
    ui->labelSessionTime->setText(time);
    ui->labelSessionCount->setText(QString::number(count));
    ui->labelUsage0->setText(QString::number(volume[0])+" ml");
    ui->labelUsage1->setText(QString::number(volume[1])+" ml");
    ui->labelUsage2->setText(QString::number(volume[2])+" ml");

    updateButtons();
}

void MainWindow::updateDeviceInfo() {
        ui->labelPort->setText(device.get()->portName());
        ui->labelDevice->setText(device.getStatus());
}

void MainWindow::updateButtons() {
    ui->buttonStart->setEnabled( isDeviceReady && session.size() > 0 );
    ui->buttonFlush->setEnabled( isDeviceReady );
    ui->buttonDrain->setEnabled( isDeviceReady );
}

// LIST WIDGET METHODS

void MainWindow::listInsert(int index, Stage s) {
    QString item;
    switch(s.getType()) {
        case STAGE_FILL : item = "Fill\t" + QString::number(s.getDur()) + " sec\tPump " + QString::number(s.getPump()); break;
        case STAGE_WASH : item = "Wash\t" + QString::number(s.getDur()) + " sec"; break;
        case STAGE_DRY : item = "Dry\t" + QString::number(s.getDur()) + " sec"; break;
        default : break;
    }
    ui->sessionList->insertItem(index, item);
    updateSessionInfo();
}

void MainWindow::listUpdate() {
    ui->sessionList->clear();
    for(int i=0; i<session.size(); i++)
        listInsert(i, session[i]);
}

void MainWindow::on_sessionList_activated() {
    editPaneUpdate();
}

// DEVICE METHODS

void MainWindow::setupDevice() {
    ui->labelPort->setText("detecting");

    if( device.init() == INIT_SUCCESS ) {
        updateDeviceInfo();
        updateConsole("Found compatible device on " + device.get()->portName() + "...");

        QObject::connect(device.get(), SIGNAL(readyRead()), this, SLOT(deviceRead()));
        device.openPort();
        device.handshake();
    }
    else {
        updateConsole("Unable to find compatible device");
        ui->labelPort->setText("unknown");
        ui->labelDevice->setText("not ready");
    }
}

void MainWindow::deviceRead() {

    // reads device API responses, and performs actions based on them

    if( device.get()->isOpen() )
        serialBuffer += device.get()->readAll();

    if( serialBuffer.size() > 2 ) {

        if( serialBuffer.contains("idl") ) {
            isDeviceReady = true;

            device.setStatus("ready");
            updateConsole("Device on " + device.get()->portName() + " ready");
            ui->buttonDetect->setEnabled(true);

            device.closePort();
            serialBuffer = "";
        }
        else if( serialBuffer.contains("err")
                 || serialBuffer.contains("rdy") ) {
            serialBuffer = "";
        }
        else if( serialBuffer.contains("bsy") ) {
            updateConsole("Depositor running...");
            setEnableUi(false);
            serialBuffer = "";
        }
        else if( serialBuffer.contains("dne") ) {
            updateConsole("Complete");
            setEnableUi(true);
            serialBuffer = "";
            device.closePort();
        }
        updateDeviceInfo();
    }
    updateButtons();
}

void MainWindow::sendSession() {

    device.openPort();
    device.initSession();
    updateConsole("Transferring session data...");
    device.sendSession( session );
    device.execSession();
}

void MainWindow::on_buttonFlush_clicked()
{
   device.openPort();
   device.write("fsh");
}

void MainWindow::on_buttonDrain_clicked()
{
   device.openPort();
   device.write("drn");
}
