#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lbl.h"
#include "dialogadd.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QTextStream>
#include <QVector>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_New_triggered();
    void on_action_Open_triggered();
    void on_actionSave_As_triggered();
    void on_actionExit_triggered();

    void on_buttonUp_clicked();
    void on_buttonDown_clicked();
    void on_buttonAdd_clicked();
    void on_buttonRemove_clicked();
    void on_buttonCopy_clicked();

    void listInsert(int index, Stage s);
    void listUpdate();
    void initEdit();
    void editPaneUpdate();
    void updateSessionTime();

    void on_sessionList_itemClicked(QListWidgetItem *item);
    void on_sessionList_activated(const QModelIndex &index);

    void on_comboType_activated(int index);
    void on_comboPump_activated(int index);
    void on_editDur_textEdited(const QString &arg1);

    void setEnableUi(bool state);

    void initDevice();
    void deviceRead();
    void sendSession();

    void on_buttonDetect_clicked();

    void on_buttonStart_clicked();

private:
    Ui::MainWindow *ui;

    bool isSelect = false;
    QString version = "0.1.1";
    QVector<Stage> session;

    QSerialPort *device;
    QByteArray serialBuffer = "";
    QString devicePortName;
    bool isDeviceAvailable = false, isHandshake = false, isDeviceReady = false, isStart = false;
};

#endif // MAINWINDOW_H
