#include "UIWidget.h"
#include "ui_gswidget.h"
#include <iostream>
#include <iomanip>
#include <c++/cassert>

GSWidget::GSWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GSWidget),
    clockTimer(this)
{
    ui->setupUi(this);

    graphSetup();

    connect(&clockTimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    clockTimer.start(std::lround((1.0 / 60.0) * 1000));

    worker = new Worker;
    worker->moveToThread(&workerThread);

    qRegisterMetaType<QVector<QCPGraphData>>("QVector<QCPGraphData>&");
    qRegisterMetaType<GraphFeature>("GraphFeature");

    connect(worker,
            SIGNAL(dummySignal()),
            this,
            SLOT(dummySlot()));
    connect(worker,
            SIGNAL(graphDataReady(QVector<QCPGraphData> & , GraphFeature)),
            this,
            SLOT(updateGraphData(QVector<QCPGraphData> & , GraphFeature)));
    connect(&workerThread,
            SIGNAL(started()),
            worker,
            SLOT(run()));

    workerThread.start();
}

GSWidget::~GSWidget()
{
    delete ui;
    workerThread.requestInterruption();
    workerThread.quit();
    workerThread.wait();
}

void GSWidget::graphSetup() {
    QCustomPlot* customPlot = ui->graph_widget;

    customPlot->plotLayout()->clear();


    QCPAxisRect *topAxisRect = new QCPAxisRect(customPlot);
    customPlot->plotLayout()->addElement(0, 0, topAxisRect);
    QCPAxisRect *bottomAxisRect = new QCPAxisRect(customPlot);
    customPlot->plotLayout()->addElement(1, 0, bottomAxisRect);

    QPen penFeature1;
    QPen penFeature2;
    penFeature1.setWidth(2);
    penFeature1.setColor(QColor(180, 0, 0));
    penFeature2.setColor(QColor(0, 180, 0));

    QList<QCPAxis*> allAxes;
    allAxes << bottomAxisRect->axes() << topAxisRect->axes();
            foreach (QCPAxis *axis, allAxes) {
            axis->setLayer("axes");
            axis->grid()->setLayer("grid");
        }

    QCPGraph *g1 = customPlot->addGraph(topAxisRect->axis(QCPAxis::atBottom), topAxisRect->axis(QCPAxis::atLeft));
    QCPGraph *g2 = customPlot->addGraph(bottomAxisRect->axis(QCPAxis::atBottom), bottomAxisRect->axis(QCPAxis::atLeft));

    g1->setPen(penFeature1);
    g2->setPen(penFeature2);

    // Check if the number of graphs corresponds to the number of available features
    assert(ui->graph_widget->graphCount() == static_cast<int>(GraphFeature::Count));

}

void GSWidget::dummySlot() {
    std::cout << "The dummy slot was called. Time was: "
              << QTime::currentTime().toString().toStdString()
              << "." << std::setw(3) << std::setfill('0')
              << QTime::currentTime().msec() << std::endl;
}

void GSWidget::updateTime() {
    ui->ground_time->setText(QTime::currentTime().toString());
}

void GSWidget::updateGraphData(QVector<QCPGraphData> &d, GraphFeature feature) {
    QCPGraph *g = ui->graph_widget->graph(static_cast<int>(feature));

    g->data()->set(d);
    g->keyAxis()->setRange(d.last().key, UIConstants::MSECS_GRAPH_XRANGE, Qt::AlignRight);
    g->valueAxis()->rescale(true);

    ui->graph_widget->replot();
}

void GSWidget::updateTelemetry(TelemetryReading t) {
    ui->telemetry_altitude_value->setText(QString::number(t.altitude.value));
    ui->telemetry_speed_value->setText(QString::number(t.speed.value));
    ui->telemetry_acceleration_value->setText(QString::number(t.acceleration.value));
    ui->telemetry_pressure_value->setText(QString::number(t.pressure.value));
    ui->telemetry_temperature_value->setText(QString::number(t.temperature.value));

    QString yprStr = "" +
            QString::number(t.ypr.yaw) + "/" +
            QString::number(t.ypr.pitch) + "/" +
            QString::number(t.ypr.roll);

    ui->telemetry_ypr_value->setText(yprStr);
}

void GSWidget::updateLinkStatus(bool radioStatus, bool videoStatus){
    ui->link_radio_status->setText((radioStatus) ? UP : DOWN);
    ui->link_video_status->setText((videoStatus) ? UP : DOWN);
}

void GSWidget::updateGroundStatus(float temperature, float pressure){
    ui->ground_temperature_value->setText(QString::number(temperature));
    ui->ground_temperature_value->setText(QString::number(pressure));
}
