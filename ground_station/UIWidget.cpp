#include "UIWidget.h"
#include "ui_gswidget.h"
#include <iostream>
#include <cassert>

GSWidget::GSWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GSWidget),
    clockTimer(this)
{
    ui->setupUi(this);

    graphSetup();

    connect(&clockTimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    clockTimer.start(std::lround((1.0 / 60.0) * 1000));

}

GSWidget::~GSWidget()
{
    delete ui;
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

void GSWidget::updateEvents(vector<RocketEvent> &events) {
    if (events.size() >= 1) {
        for (RocketEvent e : events) {
            int seconds = e.timestamp_ / TimeConstants::MSECS_IN_SEC;
            int minutes = seconds / TimeConstants::SECS_IN_MINUTE;

            stringstream ss;
            ss << "T+"
               << setw(TimeConstants::SECS_AND_MINS_WIDTH) << setfill('0') << minutes % TimeConstants::MINUTES_IN_HOUR
               << ":"
               << setw(TimeConstants::SECS_AND_MINS_WIDTH) << setfill('0') << seconds % TimeConstants::SECS_IN_MINUTE
               << ":"
               << setw(TimeConstants::MSECS_WIDTH) << setfill('0') << e.timestamp_ % TimeConstants::MSECS_IN_SEC
               << "    " << (e.description);

            ui->event_log->appendPlainText(QString::fromStdString(ss.str()));
        }
    }
}

//TODO: only use qvectors or only use vectors
void GSWidget::updateGraphData(QVector<QCPGraphData> &d, GraphFeature feature) {

    if (d.isEmpty()) {
        return;
    }

    QCPGraph *g = ui->graph_widget->graph(static_cast<int>(feature));

    g->data()->add(d);

    int sizeDiff = g->data()->size() - DataConstants::MAX_DATA_VECTOR_SIZE;
    if (sizeDiff > 0) {
        g->data()->removeBefore(
                (g->data()->at(DataConstants::DELETION_FACTOR * (sizeDiff + DataConstants::MAX_DATA_VECTOR_SIZE))->key)
        );
    }

    g->keyAxis()->setRange(d.last().key, UIConstants::GRAPH_XRANGE_MSECS, Qt::AlignRight);
    g->valueAxis()->rescale(true);
    g->valueAxis()->scaleRange(UIConstants::GRAPH_RANGE_MARGIN_RATIO);

    ui->graph_widget->replot();
}

void GSWidget::updateTelemetry(TelemetryReading t) {
    ui->telemetry_altitude_value->setText(QString::number(t.altitude_, 'f', UIConstants::PRECISION));
//    ui->telemetry_speed_value->setText(QString::number(t.speed, 'f', UIConstants::PRECISION));
    ui->telemetry_acceleration_value->setText(QString::number(t.acceleration_.norm(), 'f', UIConstants::PRECISION));
    ui->telemetry_pressure_value->setText(QString::number(t.pressure_, 'f', UIConstants::PRECISION));
    ui->telemetry_temperature_value->setText(QString::number(t.temperature_, 'f', UIConstants::PRECISION));
    ui->telemetry_yaw_value->setText(QString::number(t.acceleration_.x_, 'f', UIConstants::PRECISION));
    ui->telemetry_pitch_value->setText(QString::number(t.acceleration_.y_, 'f', UIConstants::PRECISION));
    ui->telemetry_roll_value->setText(QString::number(t.acceleration_.z_, 'f', UIConstants::PRECISION));
}

void GSWidget::updateLinkStatus(bool radioStatus, bool videoStatus){

}

void GSWidget::updateGroundStatus(float temperature, float pressure){
    ui->ground_temperature_value->setText(QString::number(temperature, 'f', UIConstants::PRECISION));
    ui->ground_temperature_value->setText(QString::number(pressure, 'f', UIConstants::PRECISION));
}

void GSWidget::graphSetup() {
    QCustomPlot *customPlot = ui->graph_widget;

    // TODO: check if needed on RaspberryPi3
    //customPlot->setOpenGl(true);

    customPlot->plotLayout()->clear();

    QCPAxisRect *topAxisRect = new QCPAxisRect(customPlot);
    topAxisRect->setupFullAxesBox(true);
    customPlot->plotLayout()->addElement(0, 0, topAxisRect);
    QCPAxisRect *bottomAxisRect = new QCPAxisRect(customPlot);
    bottomAxisRect->setupFullAxesBox(true);
    customPlot->plotLayout()->addElement(1, 0, bottomAxisRect);

    QFont font;
    font.setPointSize(12);
    topAxisRect->axis(QCPAxis::atLeft, 0)->setTickLabelFont(font);
    topAxisRect->axis(QCPAxis::atBottom, 0)->setTickLabelFont(font);
    bottomAxisRect->axis(QCPAxis::atLeft, 0)->setTickLabelFont(font);
    bottomAxisRect->axis(QCPAxis::atBottom, 0)->setTickLabelFont(font);

    QPen penFeature1;
    QPen penFeature2;
    penFeature1.setWidth(1);
    penFeature2.setWidth(1);
    penFeature1.setColor(QColor(180, 0, 0));
    penFeature2.setColor(QColor(0, 180, 0));

    QList<QCPAxis *> allAxes;
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


bool GSWidget::event(QEvent *event) {
    if (event->type() == QEvent::KeyPress) {

        QKeyEvent *ke = static_cast<QKeyEvent *>(event);

        if (ke->key() == Qt::Key_Space) {
            emit toggleLogging();
            return true;
        }

    }

    return QWidget::event(event);
}