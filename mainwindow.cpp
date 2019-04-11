#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("LbL Tools beta" + version);
    initEdit();
    device = new QSerialPort;

    loadSettings();
    if(settingAutoScan)
        initDevice();
    updateSave();
}

MainWindow::~MainWindow() {
    if (device->isOpen())
        device->close();
    delete device;
    delete ui;
}

// SETTINGS METHODS
void MainWindow::loadSettings() {
    QSettings setting("UoN","lbl-tools");
    setting.beginGroup("MainWindow");
    settingAutoScan = setting.value("AutoScan", true).toBool();
    setting.endGroup();
}

void MainWindow::saveSettings() {
    QSettings setting("UoN","lbl-tools");
    setting.beginGroup("MainWindow");
    setting.setValue("AutoScan", settingAutoScan);
    setting.endGroup();
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

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open existing session..."), "",
                                                     "LbL session (*.lbl);;All files (*.*)");	// open file dialog
    if(fileName.isEmpty() && fileName.isNull())
         return;

    currentFile = fileName;
    session.clear();
    openSession(fileName);
}

void MainWindow::openSession(QString fileName) {
    if( fileName.isEmpty() || fileName.isNull() )
        return;

    QFile inputFile(fileName);
    if( inputFile.open(QIODevice::ReadOnly) ) {

        QTextStream in(&inputFile);
        QString line;
        Stage parsed;

        int i = 0;
        while( !in.atEnd() ) {
           i++;
           parsed.initStage();
           line = in.readLine();
           if( parseStageFromFile( line, parsed ) == PARSE_SUCCESS ) {
               session.push_back( Stage(parsed) );
           }
           else {
               updateConsole("Parse error on line " + QString::number(i) + " of \"" + getCurrentFileName() + "\"");
               return;
           }
        }
        updateConsole("Opened LbL session \"" + getCurrentFileName() + "\"");
        updateSave();

        listUpdate();
        ui->sessionList->setCurrentRow(0);
        editPaneUpdate();

        inputFile.close();
    }
}

void MainWindow::on_actionSave_As_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save session as...", ".lbl", "LbL session (*.lbl);; All files (*.*)");
    saveSession(fileName);
}

void MainWindow::on_action_Save_triggered() {
    if(!currentFile.isEmpty())
        saveSession(currentFile);
}

void MainWindow::saveSession(QString fileName) {
    QString line;
    QFile file(fileName);
    if(fileName.isEmpty() && fileName.isNull())
         return;
    else if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot save session: ", file.errorString());
        return;
    }
    file.resize(0);
    QTextStream out(&file);
    for(int i=0; i<session.size(); i++) {
        switch(session[i].getType()) {
        case STAGE_FILL : line = "fill p:" + QString::number(session[i].getPump()) + " t:" + QString::number(session[i].getDur()); break;
        case STAGE_WASH : line = "wash t:" + QString::number(session[i].getDur()); break;
        case STAGE_DRY :  line = "dry t:" + QString::number(session[i].getDur()); break;
        default : break;
        }
        out << line << endl;
    }
    file.close();
    currentFile = fileName;
    updateConsole("Saved LbL session \"" + getCurrentFileName() + "\"");
    updateSave();
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

void MainWindow::on_action_Settings_triggered() {
    dialogSettings sDialog;
    sDialog.setModal(true);
    sDialog.setAutoScan(settingAutoScan);
    if( sDialog.exec() == QDialog::Accepted ) {
        settingAutoScan = sDialog.getAutoScan();
        saveSettings();
    }
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

void MainWindow::on_sessionList_itemClicked() {
    editPaneUpdate();
}

// EDIT PANE METHODS
void MainWindow::setEnableUi(bool state) {
    ui->verticalLayoutWidget->setEnabled(state);
    ui->groupBox->setEnabled(state);
    ui->groupBox_2->setEnabled(state);
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

void MainWindow::initEdit() {
    ui->comboType->addItem("Fill");
    ui->comboType->addItem("Wash");
    ui->comboType->addItem("Dry");

    ui->comboPump->addItem("Pump 0");
    ui->comboPump->addItem("Pump 1");
    ui->comboPump->addItem("Pump 2");

    ui->editDur->setValidator( new QIntValidator(1, 1000, this) );
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

   }

void MainWindow::on_sessionList_activated() {
    editPaneUpdate();
}

// DEVICE METHODS
void MainWindow::initDevice() {
    static const quint16 arduinoVendorId = 9025;
    static const quint16 arduinoProductId = 67;

    isDeviceAvailable = false;
    isDeviceReady = false;

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()) {
            if( serialPortInfo.vendorIdentifier() == arduinoVendorId ) {
                if( serialPortInfo.productIdentifier() == arduinoProductId ) {
                    devicePortName = serialPortInfo.portName();
                    isDeviceAvailable = true;
                    updateConsole("Found a compatible device on " + devicePortName + "...");
                    ui->labelPort->setText(devicePortName);
                }
            }
        }

    }
    if(isDeviceAvailable) {
        device->setPortName(devicePortName);
        device->setBaudRate(QSerialPort::Baud9600);
        device->setDataBits(QSerialPort::Data8);
        device->setParity(QSerialPort::NoParity);
        device->setStopBits(QSerialPort::OneStop);
        device->setFlowControl(QSerialPort::NoFlowControl);
        QObject::connect(device, SIGNAL(readyRead()), this, SLOT(deviceRead()));

        isHandshake = true;
        ui->labelDevice->setText("awaiting response");
        device->open(QSerialPort::ReadWrite);
        QTest::qWait(500);
        device->write("hnd");
        QTest::qWait(500);
        device->write("hnd");
    } else {
        updateConsole("No compatible device found");
        ui->labelPort->setText("unknown");
        ui->labelDevice->setText("not ready");
        ui->buttonStart->setEnabled(false);
    }

}

void MainWindow::deviceRead() {
    serialBuffer += device->readAll();
    if( isHandshake && serialBuffer.size() > 2 ) {
        if( serialBuffer.contains("shk") ) {
            isDeviceReady = true;
            ui->labelDevice->setText("ready");
            updateConsole("Device on " + devicePortName + " ready");
        }
        else {
            ui->labelDevice->setText("handshake failed");
            updateConsole("Device handshake failed");
        }
        device->close();
        serialBuffer = "";
        isHandshake = false;
        ui->buttonStart->setEnabled(true);
    }
    else if( isStart && serialBuffer.size() > 2 && serialBuffer.contains("rcv") ) {
        serialBuffer = "";
        sendSession();
    }
    else if( isStart && serialBuffer.size() > 2 && serialBuffer.contains("rdy") ) {
        serialBuffer = "";
        setEnableUi(true);
        updateConsole("Session transfer done");
        isStart = false;
    }
    if( serialBuffer.size() > 2 && serialBuffer.contains("bsy") ) {
        serialBuffer = "";

        ui->labelDevice->setText("session running");
        updateConsole("Session running...");

        ui->buttonStart->setEnabled(false);
        ui->buttonDetect->setEnabled(false);

        isRunning = true;
    }
    if( serialBuffer.size() > 2 && serialBuffer.contains("dne")) {
        serialBuffer = "";

        ui->labelDevice->setText("ready");
        updateConsole("Session complete");

        ui->buttonStart->setEnabled(true);
        ui->buttonDetect->setEnabled(true);

        device->close();
        isRunning = false;
    }
}

void MainWindow::on_buttonDetect_clicked() {
   initDevice();
}

void MainWindow::on_buttonStart_clicked() {
    if(session.isEmpty()) {
        QMessageBox::warning(this, "Error", "Session is empty");
        return;
    }
    isStart = true;
    setEnableUi(false);
    updateConsole("Waiting for device on " + devicePortName + "...");
    device->open(QSerialPort::ReadWrite);
    QTest::qWait(500);
    device->write("snd");
    QTest::qWait(500);
    device->write("snd");
}

void MainWindow::sendSession() {
    ui->labelDevice->setText("receiving data");
    updateConsole("Transferring session data...");

    QString cmd;
    for(int i=0; i<session.size(); i++) {
        switch(session[i].getType()) {
        case STAGE_FILL : cmd = "f:" + QString::number(session[i].getPump()) + "," + QString::number(session[i].getDur()) + ";"; break;
        case STAGE_WASH : cmd = "w:" + QString::number(session[i].getDur()) + ";"; break;
        case STAGE_DRY :  cmd = "d:" + QString::number(session[i].getDur()) + ";"; break;
        default : break;
        }
        QTest::qWait(50);
        device->write(cmd.toUtf8());
    }
    device->write("end;");
}

// CONSOLE METHODS

void MainWindow::updateConsole( QString str ) {
    QTextCursor tc(ui->console->textCursor());
    tc.setPosition(ui->console->document()->characterCount()-1);

    ui->console->appendPlainText(str);
    ui->console->setTextCursor(tc);
}
