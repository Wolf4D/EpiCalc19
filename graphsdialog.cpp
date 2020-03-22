#include "graphsdialog.h"
#include "ui_graphsdialog.h"
#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>

///////////////////////////////////////////////////////////////////////////////
GraphsDialog::GraphsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphsDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window);
    preparePlots();
}
///////////////////////////////////////////////////////////////////////////////
GraphsDialog::~GraphsDialog()
{
    delete ui;
}
///////////////////////////////////////////////////////////////////////////////
void GraphsDialog::getHospitalData(QPolygonF &hospitalPoly)
{
    ui->hospitalPlot->setAxisScale( QwtPlot::xBottom, 0, hospitalPoly.last().x() +1 );
    hospitalCurve->setSamples( hospitalPoly );
    ui->hospitalPlot->replot();
}
///////////////////////////////////////////////////////////////////////////////
void GraphsDialog::setInfectionThempoData(QList<double> &infectedActive,
                                          QList<double> &infectedPassive,
                                          QList<double> &infectedTerminal,
                                          QList<double> &uninfected)
{

    QVector< QVector<double> > series;
    for ( int i = 0; i < uninfected.count(); i++ )
    {
        QVector<double> values;
        values += infectedPassive[i];
        values += infectedActive[i];
        values += infectedTerminal[i];
        values += uninfected[i];
        series += values;
    }

    ui->illPlot->setAxisScale( QwtPlot::xBottom, 0, uninfected.count() +1 );

    ui->illPlot->plotLayout()->setAlignCanvasToScales( false );
    ui->illPlot->plotLayout()->setAlignCanvasToScales( true );

    illnessBars->setSamples( series );
    ui->illPlot->replot();
}
///////////////////////////////////////////////////////////////////////////////
void GraphsDialog::setDieData(QList<double> &diedDaily, QList<double> &curedDaily)
{

    QVector<QwtIntervalSample> samplesDied( diedDaily.count() );
    QVector<QwtIntervalSample> samplesCured( diedDaily.count() );

    for ( int i = 0; i < diedDaily.count(); i++ )
    {
        QwtInterval interval( double( i*2 ), double( i*2+2 ) );
        interval.setBorderFlags( QwtInterval::ExcludeMaximum );

        samplesDied[i] = QwtIntervalSample( diedDaily[i], interval );
        samplesCured[i] = QwtIntervalSample( curedDaily[i], interval );
    }

    ui->diePlot->setAxisScale( QwtPlot::xBottom, 0, diedDaily.count()*2 );

    histogramDeathCount->setData( new QwtIntervalSeriesData( samplesDied ) );
    histogramCureCount->setData( new QwtIntervalSeriesData( samplesCured ) );

    ui->diePlot->replot();

}
///////////////////////////////////////////////////////////////////////////////
void GraphsDialog::setEnvData(QPolygonF &envDaily,
                              QPolygonF &socDaily,
                              QPolygonF &panicDaily)
{
    // Единица добавляется, чтобы вывелись цифры последней риски, и
    // не было "слипания" внутренней части с границами
    ui->sitPlot->setAxisScale( QwtPlot::xBottom, 0, envDaily.last().x()+1);
    ui->sitPlot->setAxisScale( QwtPlot::yLeft, 0, 3);

    envCurve->setSamples( envDaily );

    socCurve->setSamples( socDaily );

    panicCurve->setSamples( panicDaily );

    ui->sitPlot->replot();
}
///////////////////////////////////////////////////////////////////////////////
void GraphsDialog::setMainData(QPolygonF &tinfectedCurve, QPolygonF &tuninfectedCurve, QPolygonF &tdiedCurve,
                               int verticalMax)
{
    ui->totalPlot->setAxisScale( QwtPlot::xBottom, 0, tinfectedCurve.last().x() +1);
    ui->totalPlot->setAxisScale( QwtPlot::yLeft, 0, verticalMax);

    infectedCurve->setSamples( tinfectedCurve );

    uninfectedCurve->setSamples( tuninfectedCurve );

    diedCurve->setSamples( tdiedCurve );

    ui->totalPlot->replot();
}
///////////////////////////////////////////////////////////////////////////////
/*
void GraphsDialog::showItem(const QVariant &itemInfo, bool on )
{

    // TODO: Переделать!
    QwtPlotItem *plotItem;
    plotItem = ui->illPlot->infoToItem( itemInfo );
    if ( plotItem )
    {
        plotItem->setVisible( on );
        ui->illPlot->replot();
        //ui->illPlot->setAxisScale( QwtPlot::yLeft, 0, maxPeople );

    }
}
*/
///////////////////////////////////////////////////////////////////////////////
void GraphsDialog::preparePlots()
{
    configPlot(ui->totalPlot, palette().color( QPalette::Button ), "Течение эпидемии");

    configPlot(ui->illPlot, QColor( 165, 193, 228 ), "Динамика заболевания");
    configPlot(ui->diePlot, QColor( 219, 162, 152 ), "Смертность / выздоровление");
    configPlot(ui->sitPlot, QColor( 167, 225, 150 ), "Обстановка");
    configPlot(ui->hospitalPlot, QColor( 154, 225, 225 ), "Больницы");
    ui->sitPlot->setAxisTitle(QwtPlot::yLeft, "у.е." );

    ui->illPlot->setMinimumSize(100, 200);
    ui->diePlot->setMinimumSize(100, 200);
    ui->sitPlot->setMinimumSize(100, 200);
    ui->hospitalPlot->setMinimumSize(100, 200);

    ui->totalPlot->setMinimumSize(100, 250);

    /////////////////////

    // Динамика заболевания - это гистограммы

    illnessBars = new QwtPlotMultiBarChart( "" );
    illnessBars->setLayoutPolicy( QwtPlotMultiBarChart::AutoAdjustSamples );
    illnessBars->setSpacing( 1 );
    //illnessBars->setMargin( 3 );
    illnessBars->setStyle( QwtPlotMultiBarChart::Stacked );
    illnessBars->attach( ui->illPlot );

    static const char *colors[] = { "Blue", "Red", "Black", "Green" };

    for ( int i = 0; i < 4; i++ )
    {
        QwtColumnSymbol *symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
        symbol->setLineWidth( 2 );
        symbol->setFrameStyle( QwtColumnSymbol::Raised );
        symbol->setPalette( QPalette( colors[i] ) );
        illnessBars->setSymbol( i, symbol );
    }

    QList<QwtText> titles;
    titles += QString("Инкубационный период");
    titles += QString("Активно болеющие");
    titles += QString("Терминальная стадия");
    titles += QString("Здоровые");

    illnessBars->setBarTitles( titles );
    illnessBars->setLegendIconSize( QSize( 10, 14 ) );

    /////////////////////

    hospitalCurve = new QwtPlotCurve();
    hospitalCurve->setTitle( "Занято койкомест" );
    hospitalCurve->setPen( Qt::blue, 4 );
    hospitalCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse, QBrush( Qt::yellow ), QPen( Qt::red, 1 ), QSize( 4, 4 ) );
    hospitalCurve->setSymbol( symbol );
    hospitalCurve->attach( ui->hospitalPlot );
    ///////////////////////

    histogramDeathCount = new QwtPlotHistogram( "Погибло за день" );
    QColor tc = QColor(Qt::darkMagenta);
    tc.setAlpha(180);
    histogramDeathCount->setStyle( QwtPlotHistogram::Columns );
    histogramDeathCount->setBrush(tc);
    histogramDeathCount->attach( ui->diePlot );

    histogramCureCount = new QwtPlotHistogram( "Выздоровело за день" );
    tc = QColor(Qt::darkCyan);
    tc.setAlpha(120);
    histogramCureCount->setStyle( QwtPlotHistogram::Columns );
    histogramCureCount->setBrush(tc);
    histogramCureCount->attach( ui->diePlot );

    ///////////////////////

    envCurve = new QwtPlotCurve();
    envCurve->setTitle( "Заразность окружения" );
    envCurve->setPen( Qt::darkBlue, 2 );
    envCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    envCurve->attach( ui->sitPlot );

    panicCurve = new QwtPlotCurve();
    panicCurve->setTitle( "Паника" );
    panicCurve->setPen( Qt::red, 3 );
    panicCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    panicCurve->attach( ui->sitPlot );

    socCurve = new QwtPlotCurve();
    socCurve->setTitle( "Социальная активность" );
    socCurve->setPen( Qt::cyan, 2 );
    socCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    socCurve->attach( ui->sitPlot );

    ///////////////////////


    infectedCurve = new QwtPlotCurve();
    infectedCurve->setTitle( "Заражённые" );
    infectedCurve->setPen( Qt::red, 3 );
    infectedCurve->setSymbol( symbol );
    infectedCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    infectedCurve->attach( ui->totalPlot );


    uninfectedCurve = new QwtPlotCurve();
    uninfectedCurve->setTitle( "Незаражённые" );
    uninfectedCurve->setPen( Qt::green, 3 );
    QwtSymbol * usymbol = new QwtSymbol( QwtSymbol::Diamond, QBrush( Qt::red ), QPen( Qt::cyan, 2 ), QSize( 5, 4 ) );
    uninfectedCurve->setSymbol( usymbol );

    uninfectedCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    uninfectedCurve->attach( ui->totalPlot );


    QwtSymbol *xsymbol = new QwtSymbol( QwtSymbol::XCross, QBrush( Qt::yellow ), QPen( Qt::black, 2 ), QSize( 4, 4 ) );

    diedCurve = new QwtPlotCurve();
    diedCurve->setTitle( "Погибшие" );
    diedCurve->setPen( Qt::black, 2 );
    diedCurve->setSymbol( xsymbol );

    diedCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    diedCurve->attach( ui->totalPlot );

    ///////////////////////
}

void GraphsDialog::configPlot(QwtPlot *plot, QColor backdrop, QString plotName, bool legendCheckable)
{
    plot->setTitle(plotName);
    plot->setCanvasBackground( Qt::white );
    plot->setAutoFillBackground( true );
    plot->setPalette( QPalette( backdrop ) );
    plot->plotLayout()->setAlignCanvasToScales( true );

    QwtLegend *legend = new QwtLegend;
    plot->insertLegend( legend,  QwtPlot::BottomLegend  );

    QwtPlotGrid *grid = new QwtPlotGrid();
    //grid->enableX( false );
    grid->setMajorPen( Qt::black, 0, Qt::DotLine );
    grid->attach( plot );

    plot->setAxisTitle(QwtPlot::xBottom, "Дни" );
    plot->setAxisTitle(QwtPlot::yLeft, "Чел." );

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setLineWidth( 1 );
    canvas->setFrameStyle( QFrame::Box | QFrame::Plain );
    canvas->setBorderRadius( 2 );

    QPalette canvasPalette( Qt::white );
    //canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
    canvas->setPalette( canvasPalette );

    plot->setCanvas( canvas );

    // panning with the left mouse button
    ( void ) new QwtPlotPanner( canvas );

    // zoom in/out with the wheel
    ( void ) new QwtPlotMagnifier( canvas );

}
///////////////////////////////////////////////////////////////////////////////
