#ifndef DIALOGADD_H
#define DIALOGADD_H

#include <QDialog>

namespace Ui {
class dialogAdd;
}

class dialogAdd : public QDialog
{
    Q_OBJECT

public:
    explicit dialogAdd(QWidget *parent = nullptr);
    ~dialogAdd();

    int getType() { return type; }
    int getPump() { return pump; }
    int getDur() { return dur; }

private slots:
    void on_comboType_currentIndexChanged(int index);

    void on_comboPump_currentIndexChanged(int index);

    void on_editDur_textEdited(const QString &arg1);

private:
    Ui::dialogAdd *ui;
    int type, pump, dur;
};

#endif // DIALOGADD_H
