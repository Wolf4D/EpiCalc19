#ifndef COUNTERMEASURE_H
#define COUNTERMEASURE_H

#include <QFrame>

namespace Ui {
class CounterMeasure;
}

class CounterMeasure : public QFrame
{
    Q_OBJECT

public:
    explicit CounterMeasure(QWidget *parent = nullptr);
    ~CounterMeasure();
    void setText(QString text);
    void setState(bool state);
    void setValue(int day);

    bool isEnabled;
    int onDay;
private Q_SLOTS:
    void on_checkBox_toggled(bool state);
    void on_spinBox_valueChanged(int value);


private:
    Ui::CounterMeasure *ui;
};

#endif // COUNTERMEASURE_H
