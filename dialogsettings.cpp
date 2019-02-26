#include "dialogsettings.h"
#include "ui_dialogsettings.h"

dialogSettings::dialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogSettings)
{
    ui->setupUi(this);
    if(autoScan)
        ui->chkAutoScan->setCheckState(Qt::CheckState::Checked);
    else {
        ui->chkAutoScan->setCheckState(Qt::CheckState::Unchecked);
    }
}

dialogSettings::~dialogSettings()
{
    delete ui;
}

void dialogSettings::on_chkAutoScan_stateChanged(int arg1) {
    if(arg1 == 2)
        autoScan = true;
    else
        autoScan = false;
}
