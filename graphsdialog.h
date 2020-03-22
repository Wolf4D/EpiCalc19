#ifndef GRAPHSDIALOG_H
#define GRAPHSDIALOG_H

#include <QDialog>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_multi_barchart.h>
#include <qwt_plot_histogram.h>

namespace Ui {
class GraphsDialog;
}

class GraphsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GraphsDialog(QWidget *parent = 0);
    ~GraphsDialog();

    void getHospitalData(QPolygonF& hospitalPoly);
    void setInfectionThempoData(QList<double> &infectedActive,
                                QList<double> &infectedPassive,
                                QList<double> &infectedTerminal,
                                QList<double> &uninfected);

    void setDieData(QList<double> &diedDaily, QList<double> &curedDaily);

    void setEnvData(QPolygonF &envDaily, QPolygonF &socDaily, QPolygonF &panicDaily);
    void setMainData(QPolygonF &tinfectedCurve, QPolygonF &tuninfectedCurve, QPolygonF &tdiedCurve,
                     int verticalMax);
//private Q_SLOTS:
//    void showItem( const QVariant &, bool on );


private:
    QwtPlotCurve *hospitalCurve;
    QwtPlotCurve *envCurve;
    QwtPlotCurve *socCurve;
    QwtPlotCurve *panicCurve;


    QwtPlotCurve *infectedCurve;
    QwtPlotCurve *uninfectedCurve;
    QwtPlotCurve *diedCurve;
    QwtPlotMultiBarChart *illnessBars;

    QwtPlotHistogram *histogramDeathCount;
    QwtPlotHistogram *histogramCureCount;

    //Histogram *histogramActive;
    //Histogram *histogramPassive;
    //Histogram *histogramTerminal;

    void preparePlots();
    void configPlot(QwtPlot * plot, QColor backdrop, QString plotName, bool legendCheckable = false);
    Ui::GraphsDialog *ui;
};

#endif // GRAPHSDIALOG_H
