#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>

namespace Ui {
class dialogSettings;
}

class dialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit dialogSettings(QWidget *parent = nullptr);
    ~dialogSettings();

    bool getAutoScan() { return autoScan; }
    void setAutoScan(bool state) { autoScan = state; }

private slots:

    void on_chkAutoScan_stateChanged(int arg1);

private:
    Ui::dialogSettings *ui;
    bool autoScan;
};

#endif // DIALOGSETTINGS_H
