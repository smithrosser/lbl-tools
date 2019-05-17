#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lbl.h"
#include "file.h"
#include "depositor.h"
#include "parser.h"
#include "dialogadd.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QTextCursor>
#include <QVector>
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

    // FILE HANDLERS
    void openSession();
    void saveSession( QString fileName );
    QString getCurrentFileName();
    void updateSave();

    // MENU BAR METHODS
    void on_action_New_triggered();
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_actionSave_As_triggered();
    void on_actionExit_triggered();

    // BUTTON METHODS
    void on_buttonUp_clicked();
    void on_buttonDown_clicked();
    void on_buttonAdd_clicked();
    void on_buttonRemove_clicked();
    void on_buttonCopy_clicked();
    void on_buttonDetect_clicked();
    void on_buttonStart_clicked();

    // LIST WIDGET METHODS
    void on_sessionList_itemClicked();
    void on_sessionList_activated();
    void listInsert(int index, Stage s);
    void listUpdate();

    // EDIT PANE METHODS
    void initEdit();
    void editPaneUpdate();
    void on_comboType_activated(int index);
    void on_comboPump_activated(int index);
    void on_editDur_textEdited(const QString &arg1);

    // CONSOLE METHODS
    void updateConsole( QString str );

    // GENERAL UI METHODS
    void updateSessionInfo();
    void updateDeviceInfo();
    void updateButtons();
    void setEnableUi(bool state);

    // DEVICE METHODS
    void setupDevice();
    void deviceRead();
    void sendSession();

    void on_buttonFlush_clicked();

    void on_buttonDrain_clicked();

private:
    Ui::MainWindow *ui;

    QString version = "0.3.3";
    QString currentFile;
    QVector<Stage> session;

    depositor device;
    QByteArray serialBuffer = "";

    bool isDeviceAvailable = false, isHandshake = false, isDeviceReady = false, isStart = false;
    bool isSelect = false;
    bool isRunning = false;

    int stageCount = 0;
};

#endif // MAINWINDOW_H
