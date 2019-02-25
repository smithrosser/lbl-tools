#include "dialogadd.h"
#include "ui_dialogadd.h"

dialogAdd::dialogAdd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogAdd)
{
    ui->setupUi(this);
    setWindowTitle("Add deposition stage");

    ui->comboType->addItem("Fill");
    ui->comboType->addItem("Wash");
    ui->comboType->addItem("Dry");

    ui->comboPump->addItem("Pump 0");
    ui->comboPump->addItem("Pump 1");
    ui->comboPump->addItem("Pump 2");

    ui->editDur->setValidator( new QIntValidator(1, 1000, this) );
}

dialogAdd::~dialogAdd() {
    delete ui;
}

void dialogAdd::on_comboType_currentIndexChanged(int index) {
    type = index + 1;
    if( index > 0)
        ui->comboPump->setEnabled(false);
    else
        ui->comboPump->setEnabled(true);
}

void dialogAdd::on_comboPump_currentIndexChanged(int index) {
    pump = index;
}

void dialogAdd::on_editDur_textEdited(const QString &arg1) {
    dur = arg1.toInt();
}
