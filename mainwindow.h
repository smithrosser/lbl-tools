#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lbl.h"
#include "parser.h"

#include "dialogadd.h"
#include "dialogsettings.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QTextStream>
#include <QTextCursor>
#include <QTimer>
#include <QVector>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>
#include <QDebug>
#include <QTest>

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
    void loadSettings();
    void saveSettings();

    void on_action_New_triggered();
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_actionSave_As_triggered();
    void on_action_Settings_triggered();
    void on_actionExit_triggered();

    void openSession(QString fileName);
    void saveSession(QString fileName);
    QString getCurrentFileName();
    void updateSave();

    void on_buttonUp_clicked();
    void on_buttonDown_clicked();
    void on_buttonAdd_clicked();
    void on_buttonRemove_clicked();
    void on_buttonCopy_clicked();

    void listInsert(int index, Stage s);
    void listUpdate();
    void initEdit();
    void editPaneUpdate();
    void updateSessionInfo();

    void on_sessionList_itemClicked();
    void on_sessionList_activated();

    void on_comboType_activated(int index);
    void on_comboPump_activated(int index);
    void on_editDur_textEdited(const QString &arg1);

    void setEnableUi(bool state);

    void initDevice();
    void deviceRead();
    void sendSession();

    void on_buttonDetect_clicked();
    void on_buttonStart_clicked();

    void updateConsole( QString str );


private:
    Ui::MainWindow *ui;

    QString version = "0.2.4";
    QString currentFile;
    QVector<Stage> session;

    QSerialPort *device;
    QByteArray serialBuffer = "";
    QString devicePortName;

    bool isDeviceAvailable = false, isHandshake = false, isDeviceReady = false, isStart = false;
    bool isSelect = false;
    bool isRunning = false;

    bool settingAutoScan;

    int stageCount = 0;
};

#endif // MAINWINDOW_H
