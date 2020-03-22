#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include "episim.h"
#include "graphsdialog.h"
#include "countermeasure.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum MeasureNumbers
    {
        IncomingQuarantine =0,
        TempChecks =1,
        CloseBorders =2,
        CancelConcerts =3,
        CloseSchools =4,
        CloseMassed =5,
        Desinfection =6,
        StopTraffic =7,
        FullQuarantine =8,
        IntensiveHealing = 9,
        AdvancedHealing = 10,
        DoubleBeds = 11,
        MoreDoubleBeds = 12,
        VirusTests = 13,
        Vaccine = 14
    };
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public Q_SLOTS: // где-то в qwt есть no_keywords?
    void on_pushButton_pressed();
    void on_pushButton_noMeasures_pressed();
    void on_pushButton_lateMeasures_pressed();
    void on_pushButton_middleMeasures_pressed();
    void on_pushButton_hardMeasures_pressed();


private:
    void activateMeasure(EpiSim *e, MeasureNumbers measure);
    void buildMeasureList();
    Ui::MainWindow *ui;
    GraphsDialog * gd;
    QList<CounterMeasure*> measures;


};

#endif // MAINWINDOW_H
