#include "depositor.h"

depositor::depositor() {
    device = new QSerialPort();
    status = "not ready";
}

depositor::~depositor() {

    if( device->isOpen() )
        device->close();

    delete device;
}

QSerialPort* depositor::get() {
    return device;
}

QString depositor::getStatus() {
   return status;
}

void depositor::setStatus( QString str ) {
    status = str;
}

int depositor::init() {
    static const quint16 vendorId = 9025, productId = 67;

    foreach( const QSerialPortInfo &info, QSerialPortInfo::availablePorts() ) {
        if( info.hasVendorIdentifier() && info.hasProductIdentifier() ) {
            if( info.vendorIdentifier() == vendorId && info.productIdentifier() == productId ) {

                device->setPortName(info.portName());
                device->setBaudRate(QSerialPort::Baud9600);
                device->setDataBits(QSerialPort::Data8);
                device->setParity(QSerialPort::NoParity);
                device->setStopBits(QSerialPort::OneStop);
                device->setFlowControl(QSerialPort::NoFlowControl);

                status = "found...";
                return INIT_SUCCESS;
            }
        }
    }
    return INIT_ERROR;
}

void depositor::openPort() {

    device->open(QSerialPort::ReadWrite);
    for( int i=0; i<2; i++ ) {
        write("init");
        QTest::qWait(500);
    }
}

void depositor::closePort() {
    device->close();
}

void depositor::handshake() {
    write("chk");
}

void depositor::write( QByteArray msg ) {
    device->write(msg);
    device->waitForBytesWritten();
}

void depositor::initSession() {
    write("sts");
    QTest::qWait(500);
}

void depositor::sendSession( QVector<Stage> session ) {
    status = "receiving session";
    foreach( Stage s, session ) {
        sendStage(s);
        QTest::qWait(50);
    }
    write("ens");
    QTest::qWait(500);
}

void depositor::sendStage( Stage s ) {
   QString cmd;
   switch(s.getType()) {
    case STAGE_FILL : cmd = "f:" + QString::number(s.getPump()) + "," + QString::number(s.getDur()) + ";"; break;
    case STAGE_WASH : cmd = "w:" + QString::number(s.getDur()) + ";"; break;
    case STAGE_DRY :  cmd = "d:" + QString::number(s.getDur()) + ";"; break;
   default : return;
   }
   write(cmd.toUtf8());
}

void depositor::execSession() {

    write("chk");
    QTest::qWait(500);
    status = "running";
    write("dep");
}


