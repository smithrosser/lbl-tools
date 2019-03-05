#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("LbL Tools " + version);
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
    updateSave();
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
        currentFile = fileName;
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
       statusBar()->showMessage("Opened: " + fileName);
       editPaneUpdate();
       updateSave();
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
    currentFile = fileName;
    statusBar()->showMessage("Saved: " + fileName);
    updateSave();
}

void MainWindow::updateSave() {
    ui->action_Save->setEnabled(false);
    if(!currentFile.isEmpty())
        ui->action_Save->setEnabled(true);

    QString cFName;
    QFileInfo cF(QFile(currentFile).fileName());
    if(cF.fileName().isEmpty())
        cFName = "new file";
    else
        cFName = cF.fileName();
    MainWindow::setWindowTitle(cFName + " - LbL Tools " + version);
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
    if(session[pos].getType() == FILL) {
        ui->comboPump->setCurrentIndex(session[pos].getPump());
        ui->comboPump->setEnabled(true);
    } else {
        ui->comboPump->setEnabled(false);
        ui->comboType->setCurrentIndex(session[pos].getType()-1);
        ui->editDur->setText(QString::number(session[pos].getDur()));
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
        case FILL : item = "Fill\t" + QString::number(s.getDur()) + " sec\tPump " + QString::number(s.getPump()); break;
        case WASH : item = "Wash\t" + QString::number(s.getDur()) + " sec"; break;
        case DRY : item = "Dry\t" + QString::number(s.getDur()) + " sec"; break;
        default : break;
    }
    ui->sessionList->insertItem(index, item);
    updateSessionTime();
}

void MainWindow::listUpdate() {
    ui->sessionList->clear();
    for(int i=0; i<session.size(); i++)
        listInsert(i, session[i]);
}

void MainWindow::updateSessionTime() {
    int sum = 0;

    for(int i=0; i<session.size(); i++)
        sum += session[i].getDur();

    QString time = QString::number(sum/60) + " min " + QString::number(sum % 60) + " sec";
    ui->labelSessionTime->setText(time);

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
                    statusBar()->showMessage("Found a compatible device on " + devicePortName + "...");
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
        ui->labelDevice->setText("waiting for response...");
        device->open(QSerialPort::ReadWrite);
        QTest::qWait(500);
        device->write("hnd");
        QTest::qWait(500);
        device->write("hnd");
    } else {
        statusBar()->showMessage("No compatible device found");
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
            statusBar()->showMessage("Device on " + devicePortName + " ready");
        } else {
            ui->labelDevice->setText("handshake failed");
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
        statusBar()->showMessage("Session transfer done");
        ui->labelDevice->setText("ready");
        isStart = false;
    }
    if( serialBuffer.size() > 2 && serialBuffer.contains("bsy") ) {
        serialBuffer = "";
        ui->labelDevice->setText("session running");
        ui->buttonStart->setEnabled(false);
        ui->buttonDetect->setEnabled(false);
    }
    if( serialBuffer.size() > 2 && serialBuffer.contains("dne")) {
        serialBuffer = "";
        ui->labelDevice->setText("ready");
        ui->buttonStart->setEnabled(true);
        ui->buttonDetect->setEnabled(true);
        device->close();
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
    statusBar()->showMessage("Waiting for device on " + devicePortName + "...");
    device->open(QSerialPort::ReadWrite);
    QTest::qWait(500);
    device->write("snd");
    QTest::qWait(500);
    device->write("snd");
}

void MainWindow::sendSession() {
    ui->labelDevice->setText("receiving data");
    statusBar()->showMessage("Transferring session data...");

    QString cmd;
    for(int i=0; i<session.size(); i++) {
        switch(session[i].getType()) {
        case FILL : cmd = "f:" + QString::number(session[i].getPump()) + "," + QString::number(session[i].getDur()) + ";"; break;
        case WASH : cmd = "w:" + QString::number(session[i].getDur()) + ";"; break;
        case DRY :  cmd = "d:" + QString::number(session[i].getDur()) + ";"; break;
        default : break;
        }
        QTest::qWait(50);
        device->write(cmd.toUtf8());
    }
    device->write("end;");
}

