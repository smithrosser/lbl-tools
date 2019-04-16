#ifndef DEPOSITOR_H
#define DEPOSITOR_H

#include "lbl.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTest>

#define INIT_SUCCESS 0
#define INIT_ERROR 1
#define HANDSHAKE_SUCCESS 0
#define HANDSHAKE_ERROR 1

class depositor
{
private:
    QSerialPort* device;
    QString status;

public:
    depositor();
    ~depositor();

    QSerialPort* get();
    QString getStatus();
    void setStatus( QString str );

    int init();
    void handshake();
    void openPort();
    void closePort();
    void write( QByteArray msg );

    void initSession();
    void sendSession( QVector<Stage> session );
    void sendStage( Stage s );
    void execSession();
};

#endif // DEPOSITOR_H
