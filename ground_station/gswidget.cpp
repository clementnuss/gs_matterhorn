#include "gswidget.h"
#include "worker.h"
#include <QString>
#include <QTime>
#include "ui_gswidget.h"
#include <iostream>

GSWidget::GSWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GSWidget),
    clockTimer(this)
{
    ui->setupUi(this);

    connect(&clockTimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    clockTimer.start((1.0/60.0) * 1000);

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
    std::cout << "The dummy slot was called. Time was: " << QTime::currentTime().toString().toStdString() << std::endl;
}

void GSWidget::updateTime(){
    ui->ground_time->setText(QTime::currentTime().toString());
}

void GSWidget::updateTelemetry(telemetry_t t){
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
