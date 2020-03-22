#include "countermeasure.h"
#include "ui_countermeasure.h"
#include <QDebug>

CounterMeasure::CounterMeasure(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CounterMeasure)
{
    ui->setupUi(this);
    on_checkBox_toggled(false);
    onDay=1;
}

CounterMeasure::~CounterMeasure()
{
    delete ui;
}

void CounterMeasure::setText(QString text)
{
    ui->checkBox->setText(text);
}

void CounterMeasure::setState(bool state)
{
    ui->checkBox->setChecked(state);
    on_checkBox_toggled(state);
}

void CounterMeasure::setValue(int day)
{
    ui->spinBox->setValue(day);
    on_spinBox_valueChanged(day);
}

void CounterMeasure::on_checkBox_toggled(bool state)
{
    ui->line->setVisible(state);
    ui->spinBox->setVisible(state);
    ui->label->setVisible(state);
    ui->label_2->setVisible(state);
    isEnabled = state;

}

void CounterMeasure::on_spinBox_valueChanged(int value)
{
    onDay=value;
}
