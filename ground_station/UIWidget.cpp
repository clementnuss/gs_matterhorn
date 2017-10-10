#include "UIWidget.h"
#include "ui_gswidget.h"
#include <iostream>
#include <iomanip>
#include <qcustomplot.h>

GSWidget::GSWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GSWidget),
    clockTimer(this)
{
    ui->setupUi(this);

    displayGraph();

    connect(&clockTimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    clockTimer.start(std::lround((1.0 / 60.0) * 1000));

    worker = new Worker;
    worker->moveToThread(&workerThread);
    connect(worker, SIGNAL(dummySignal()), this, SLOT(dummySlot()));
    connect(&workerThread, SIGNAL(started()), worker, SLOT(run()));
    workerThread.start();
}

GSWidget::~GSWidget()
{
    delete ui;
    workerThread.requestInterruption();
    workerThread.quit();
    workerThread.wait();
}

void GSWidget::dummySlot(){
    std::cout << "The dummy slot was called. Time was: "
              << QTime::currentTime().toString().toStdString()
              << "." << std::setw(3) << std::setfill('0')
              << QTime::currentTime().msec() << std::endl;
}

void GSWidget::updateTime(){
    ui->ground_time->setText(QTime::currentTime().toString());
}

void GSWidget::displayGraph() {
    QCustomPlot* plot_frame = ui->plot_frame;

    QVector<double> x(101), y(101); // initialize with entries 0..100
    for (int i=0; i<101; ++i)
    {
        x[i] = i/50.0 - 1; // x goes from -1 to 1
        y[i] = x[i]*x[i];  // let's plot a quadratic function
    }
    // create graph and assign data to it:
    plot_frame->addGraph();
    plot_frame->graph(0)->setData(x, y);
    // give the axes some labels:
    plot_frame->xAxis->setLabel("x");
    plot_frame->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    plot_frame->xAxis->setRange(-1, 1);
    plot_frame->yAxis->setRange(0, 1);

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
