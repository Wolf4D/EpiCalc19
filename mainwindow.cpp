#include "mainwindow.h"
#include "ui_mainwindow.h"

///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    gd = new GraphsDialog (this);

    buildMeasureList();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_pressed()
{
    gd->show();

    // TODO: Нормальные массивы!
    QPolygonF hospitalCurve;
    QPolygonF uninfectedCurve;
    QPolygonF infectedTotalCurve;
    QPolygonF diedTotalCurve;

    QList<double> uninfectedBar;
    QList<double> terminalInfectedBar;
    QList<double> infectedPassiveBar;
    QList<double> infectedActiveBar;
    QList<double> diedDailyBar;
    QList<double> curedDailyBar;
    QPolygonF envCurve;
    QPolygonF panicCurve;
    QPolygonF socCurve;



    EpiSim e(ui->checkBox_moreMath->isChecked());

    e.stammContagabilityA = ui->spinBox_contAct->value() / 100.0;
    e.stammContagabilityP = ui->spinBox_contPass->value() / 100.0;
    e.lethalityCured = ui->spinBox_lethalityCured->value() / 100.0;
    e.selfCureability = ui->spinBox_cureNonHospProbability->value() / 100.0;
    e.lethalityUnCured = ui->spinBox_lethalityUncured->value() / 100.0;
    e.cureability = ui->spinBox_cureHospProbability->value() / 100.0;
    e.detectPercent = ui->spinBox_detection->value() / 100.0;
    e.uninfectedInCity = ui->spinBox_peopleCount->value();
    e.territory = ui->spinBox_square->value();
    e.hospitalCapacity = ui->spinBox_hospitalCapacity->value();
    e.incomingDensity = ui->spinBox_illIncoming->value();

    bool gotMaxPanic = false;
    int startUI = e.uninfectedInCity;
    for(int i=0; i<ui->daysSpinBox->value(); i+=2)
    {
        for(int j=0; j<15; j++)
            if (measures[j]->isEnabled)
                if (i>=measures[j]->onDay)
                    activateMeasure(&e, static_cast<MeasureNumbers>(j));

        int diedDaily=e.died;
        int curedDaily=e.cured;

        //qDebug() << "------------------------------------";
        //qDebug() << "День " << QString::number(i);
        //qDebug() << "[08:00]:";
        e.tick();
        hospitalCurve << QPointF( i+ 0.5, e.inHospital );
        uninfectedCurve<< QPointF( i+ 0.5,e.uninfectedInCity );
        terminalInfectedBar .push_back( e.infectedTerminal );
        infectedActiveBar .push_back( e.infectedActive );
        infectedPassiveBar .push_back( e.infectedPassive );
        uninfectedBar.push_back(e.uninfectedInCity);
        socCurve << QPointF( i+ 0.25, e.firstSoc );
        socCurve << QPointF( i+ 0.5, e.secondSoc );
        envCurve << QPointF( i+ 0.25, e.firstEnv );
        envCurve << QPointF( i+ 0.5, e.secondEnv );
        panicCurve << QPointF( i+ 0.5, e.panic );
        diedTotalCurve << QPointF(i+0.5, e.died);
        infectedTotalCurve << QPointF( i+ 0.5, e.infectedTerminal + e.infectedActive + e.infectedPassive + e.inHospital );

        //qDebug() << "[20:00]:";
        e.tick();
        hospitalCurve << QPointF( i+ 1.0, e.inHospital );
        uninfectedCurve << QPointF( i+ 1.0, e.uninfectedInCity );
        terminalInfectedBar .push_back( e.infectedTerminal );
        infectedActiveBar .push_back( e.infectedActive );
        infectedPassiveBar .push_back( e.infectedPassive );
        uninfectedBar.push_back(e.uninfectedInCity);
        socCurve << QPointF( i+ 0.75, e.firstSoc );
        socCurve << QPointF( i+ 1.0, e.secondSoc );
        envCurve << QPointF( i+ 0.75, e.firstEnv );
        envCurve << QPointF( i+ 1.0, e.secondEnv );
        panicCurve << QPointF( i+ 1.0, e.panic );
        diedTotalCurve << QPointF(i+1.0, e.died);
        infectedTotalCurve << QPointF( i+ 1.0, e.infectedTerminal + e.infectedActive + e.infectedPassive + e.inHospital );

        //qDebug() << "Смертей:" << e.died << ", летальность:" << QString::number(100.0*double(e.died)/double(e.cured+e.died+e.infectedActive+e.infectedPassive+e.infectedTerminal), 'f', 2) << "%";
        diedDailyBar.push_back(e.died - diedDaily);
        curedDailyBar.push_back(e.cured - curedDaily);

        if (!gotMaxPanic)
        {
            e.panic+= ui->spinBox_panicGrow->value() / 100.0;
            if (e.panic>ui->spinBox_panicLimit->value() / 100.0)
            {
                e.panic = ui->spinBox_panicLimit->value()/ 100.0;
                gotMaxPanic = true;
            }
        }
        else
        {
            e.panic -=ui->spinBox_panicGrow->value() / 100.0;
            if (e.panic<0) e.panic=0;
        }
    }

    gd->getHospitalData(hospitalCurve);
    gd->setInfectionThempoData(infectedActiveBar, infectedPassiveBar, terminalInfectedBar, uninfectedBar);
    gd->setDieData(diedDailyBar, curedDailyBar);
    gd->setEnvData(envCurve, socCurve, panicCurve);
    gd->setMainData(infectedTotalCurve, uninfectedCurve, diedTotalCurve, startUI);

}

void MainWindow::on_pushButton_noMeasures_pressed()
{
    for(int i=0; i<15; i++)
    {
        measures[i]->setState(false);
    }
}

void MainWindow::on_pushButton_lateMeasures_pressed()
{
    for(int i=0; i<15; i++)
        measures[i]->setState(true);

    measures[TempChecks]->setState(false);
    measures[MoreDoubleBeds]->setState(false);
    measures[CancelConcerts]->setState(false);
    measures[CloseMassed]->setState(false);
    measures[StopTraffic]->setState(false);
    measures[Desinfection]->setState(false);

    measures[IncomingQuarantine]->setValue(20);
    measures[CloseBorders]->setValue(30);
    measures[CloseSchools]->setValue(45);
    measures[DoubleBeds]->setValue(50);
    measures[FullQuarantine]->setValue(55);
    measures[IntensiveHealing]->setValue(60);
    measures[VirusTests]->setValue(80);
    measures[AdvancedHealing]->setValue(85);
    measures[Vaccine]->setValue(90);
}

void MainWindow::on_pushButton_middleMeasures_pressed()
{
    for(int i=0; i<15; i++)
        measures[i]->setState(true);


    measures[StopTraffic]->setState(false);
    measures[FullQuarantine]->setState(false);
    measures[MoreDoubleBeds]->setState(false);


    measures[IncomingQuarantine]->setValue(5);
    measures[TempChecks]->setValue(7);
    measures[CancelConcerts]->setValue(10);
    measures[IntensiveHealing]->setValue(18);
    measures[DoubleBeds]->setValue(20);
    measures[CloseSchools]->setValue(25);
    measures[CloseMassed]->setValue(30);
    measures[Desinfection]->setValue(35);
    measures[VirusTests]->setValue(45);
    measures[AdvancedHealing]->setValue(65);
    measures[Vaccine]->setValue(90);
}

void MainWindow::on_pushButton_hardMeasures_pressed()
{
    for(int i=0; i<15; i++)
        measures[i]->setState(true);

    measures[IncomingQuarantine]->setValue(5);
    measures[TempChecks]->setValue(7);
    measures[CancelConcerts]->setValue(8);
    measures[IntensiveHealing]->setValue(12);
    measures[DoubleBeds]->setValue(17);
    measures[CloseSchools]->setValue(15);
    measures[CloseMassed]->setValue(17);
    measures[Desinfection]->setValue(22);
    measures[StopTraffic]->setValue(25);
    measures[FullQuarantine]->setValue(30);
    measures[MoreDoubleBeds]->setValue(30);
    measures[VirusTests]->setValue(45);
    measures[AdvancedHealing]->setValue(65);
    measures[Vaccine]->setValue(90);
}

void MainWindow::activateMeasure(EpiSim *e, MainWindow::MeasureNumbers measure)
{
    switch (measure)
    {
    case IncomingQuarantine: e->isIncomingCarantine = true; break;
    case TempChecks: e->isThermalChecks = true; break;
    case CloseBorders: e->isBorderLocked = true; break;
    case CancelConcerts: e->isSomeClosed = true; break;
    case CloseSchools: e->isSchoolClosed = true; break;
    case CloseMassed: e->isMassClosed = true; break;
    case Desinfection: e->isDesinfection = true; break;
    case StopTraffic: e->carantineState = EpiSim::Lockdown; break;
    case FullQuarantine:
    {   e->isSchoolClosed = true;
        e->isMassClosed = true;
        e->isSomeClosed = true;
        e->isBorderLocked = true;
        e->isThermalChecks = false;
        e->carantineState = EpiSim::FullCarantine; break;
    }
    case IntensiveHealing: e->therapyLevel = EpiSim::Intense; break;
    case AdvancedHealing: e->therapyLevel = EpiSim::Advanced; break;
    case DoubleBeds: e->isHospitalDoubled = true; break;
    case MoreDoubleBeds: e->isHospitalQuadred = true; break;
    case VirusTests: e->isVirusTests = true; break;
    case Vaccine: e->isVaccine = true; break;
    }

}

void MainWindow::buildMeasureList()
{
    for(int i=0; i<15; i++)
    {
        CounterMeasure * m;
        m = new CounterMeasure(this);
        ui->verticalLayout_measures->addWidget(m);
        measures.push_back(m);
    }

    measures[IncomingQuarantine]->setText("Карантин для приезжающих");
    measures[TempChecks]->setText("Проверки температуры");
    measures[CloseBorders]->setText("Закрытие границы");
    measures[CancelConcerts]->setText("Отмена концертов");
    measures[CloseSchools]->setText("Закрытие школ и ВУЗов");
    measures[CloseMassed]->setText("Закрытие кафе-клубов-музеев");
    measures[Desinfection]->setText("Повсеместная дезинфекция");
    measures[StopTraffic]->setText("Остановка транспорта");
    measures[FullQuarantine]->setText("Полный всеобщий (домашний) карантин");
    measures[IntensiveHealing]->setText("Интенсивное лечение больных");
    measures[AdvancedHealing]->setText("Открытие успешного лечения");
    measures[DoubleBeds]->setText("Удвоение койкомест в больницах");
    measures[MoreDoubleBeds]->setText("Ещё удвоение койкомест");
    measures[VirusTests]->setText("Вирусные тесты");
    measures[Vaccine]->setText("Разработка вакцины");


}
