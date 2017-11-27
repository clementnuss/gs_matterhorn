#include "UIWidget.h"
#include "ui_gswidget.h"
#include "UI/Colors.h"
#include "../cmake-build-debug/qcustomplot-src/qcustomplot.h"
#include <iostream>
#include <cassert>
#include <Utilities/TimeUtils.h>

GSWidget::GSWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::GSWidget),
        plot1_{new QCustomPlot(this)},
        plot2_{new QCustomPlot(this)},
        clockTimer(this),
        lastGraphUpdate_{chrono::system_clock::now()},
        userItems_{std::vector<std::tuple<QCPAbstractItem *, QCPAbstractItem *>>()},
        lastRemoteTime_{-1000} {
    ui->setupUi(this);

    graphSetup();

    connect(&clockTimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    //connect(ui->graph_widget, SIGNAL(plottableClick(QCPAbstractPlottable * , int, QMouseEvent * )), this,
    //        SLOT(graphClicked(QCPAbstractPlottable * , int)));
    //connect(ui->graph_clear_items_button, &QPushButton::clicked, this, &GSWidget::clearAllGraphItems);

    clockTimer.start(std::lround((1.0 / 60.0) * 1000));

}

GSWidget::~GSWidget() {
    delete ui;
}

void GSWidget::dummySlot(bool b) {
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

        auto elapsed = usecsBetween(lastGraphUpdate_, chrono::system_clock::now());
        if (elapsed > UIConstants::GRAPH_DATA_INTERVAL_USECS) {
            double elapsedSeconds = elapsed / 1'000'000.0;

            for (int g_idx = 0; g_idx < static_cast<int>(GraphFeature::Count); g_idx++) {
                QCPGraph *g = plot1_->graph(g_idx);
                g->keyAxis()->setRange(lastRemoteTime_ + elapsedSeconds,
                                       UIConstants::GRAPH_XRANGE_SECS,
                                       Qt::AlignRight);
            };

            plot1_->replot();
        }
        return;
    }

    lastGraphUpdate_ = chrono::system_clock::now();
    lastRemoteTime_ = d.last().key;

    QCPGraph *g = plot1_->graph(static_cast<int>(feature));

    // Clear any eventual datapoint ahead of current time point
    g->data()->removeAfter(d.last().key);

    g->data()->add(d);

    g->data()->removeBefore(d.last().key - UIConstants::GRAPH_XRANGE_SECS);

    g->keyAxis()->setRange(d.last().key, UIConstants::GRAPH_XRANGE_SECS, Qt::AlignRight);
    g->valueAxis()->rescale(true);
    g->valueAxis()->scaleRange(UIConstants::GRAPH_RANGE_MARGIN_RATIO);

    plot1_->replot();
}


void GSWidget::updateTelemetry(TelemetryReading t) {
    ui->telemetry_altitude_value->setText(QString::number(t.altitude_, 'f', UIConstants::PRECISION));
    ui->telemetry_speed_value->setText(QString::number(t.air_speed_, 'f', UIConstants::PRECISION));
    ui->telemetry_acceleration_value->setText(QString::number(t.acceleration_.norm(), 'f', UIConstants::PRECISION));
    ui->telemetry_pressure_value->setText(QString::number(t.pressure_, 'f', UIConstants::PRECISION));
    ui->telemetry_temperature_value->setText(QString::number(t.temperature_, 'f', UIConstants::PRECISION));
    ui->telemetry_yaw_value->setText(QString::number(t.magnetometer_.x_, 'f', UIConstants::PRECISION));
    ui->telemetry_pitch_value->setText(QString::number(t.magnetometer_.y_, 'f', UIConstants::PRECISION));
    ui->telemetry_roll_value->setText(QString::number(t.magnetometer_.z_, 'f', UIConstants::PRECISION));
}

void GSWidget::updateLoggingStatus(bool enabled) {
    QLabel *label = ui->status_logging;
    QPalette palette = label->palette();
    palette.setColor(label->backgroundRole(), enabled ? UIColors::GREEN : UIColors::RED);
    label->setPalette(palette);
}

void GSWidget::updateLinkStatus(HandlerStatus status) {
    QColor statusColor;

    switch (status) {
        case HandlerStatus::NOMINAL:
            statusColor = UIColors::GREEN;
            break;
        case HandlerStatus::LOSSY:
            statusColor = UIColors::YELLOW;
            break;
        case HandlerStatus::DOWN:
            statusColor = UIColors::RED;
            break;
        default:
            statusColor = UIColors::RED;
            break;
    }

    QLabel *label = ui->status_radio1;
    QPalette palette = label->palette();
    palette.setColor(label->backgroundRole(), statusColor);
    label->setPalette(palette);

}

void GSWidget::updateGroundStatus(float temperature, float pressure) {
    ui->ground_temperature_value->setText(QString::number(temperature, 'f', UIConstants::PRECISION));
    ui->ground_temperature_value->setText(QString::number(pressure, 'f', UIConstants::PRECISION));
}

void GSWidget::graphSetup() {
    QWidget *plotContainer = ui->plot_container;

    QVBoxLayout *layout = new QVBoxLayout(plotContainer);

    layout->addWidget(plot1_);

    plot1_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems);

    plot1_->plotLayout()->clear();

    // TODO: check if needed on RaspberryPi3
    //customPlot->setOpenGl(true);

    QFont titleFont = QFont("sans", 10, QFont::Bold);

    QCPTextElement *topTitle = new QCPTextElement(plot1_, "Altitude (m)", titleFont);
    QCPTextElement *bottomTitle = new QCPTextElement(plot1_, "Acceleration (g)", titleFont);

    auto topAxisRect = new QCPAxisRect(plot1_);
    auto bottomAxisRect = new QCPAxisRect(plot1_);

    topAxisRect->setRangeDrag(Qt::Horizontal);
    bottomAxisRect->setRangeDrag(Qt::Horizontal);

    topAxisRect->setupFullAxesBox(true);
    bottomAxisRect->setupFullAxesBox(true);

    plot1_->plotLayout()->addElement(0, 0, topTitle);
    plot1_->plotLayout()->addElement(1, 0, topAxisRect);
    plot1_->plotLayout()->addElement(2, 0, bottomTitle);
    plot1_->plotLayout()->addElement(3, 0, bottomAxisRect);

    QFont font;
    font.setPointSize(12);
    topAxisRect->axis(QCPAxis::atLeft, 0)->setTickLabelFont(font);
    topAxisRect->axis(QCPAxis::atBottom, 0)->setTickLabelFont(font);
//    This may be useful when implementing event, so as to display them with 2 figits precicion on the graph.
//    topAxisRect->axis(QCPAxis::atBottom, 0)->setNumberFormat("f");
//    topAxisRect->axis(QCPAxis::atBottom, 0)->setNumberPrecision(UserIfaceConstants::PRECISION);
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

    QCPGraph *g1 = plot1_->addGraph(topAxisRect->axis(QCPAxis::atBottom), topAxisRect->axis(QCPAxis::atLeft));
    QCPGraph *g2 = plot1_->addGraph(bottomAxisRect->axis(QCPAxis::atBottom), bottomAxisRect->axis(QCPAxis::atLeft));

    g1->setPen(penFeature1);
    g2->setPen(penFeature2);

    // Check if the number of graphs corresponds to the number of available features
    assert(plot1_->graphCount() == static_cast<int>(GraphFeature::Count));

}


void GSWidget::graphClicked(QCPAbstractPlottable *plottable, int dataIndex) {

    //TODO: Add the items to a list so we can reaccess them later

    // since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
    // usually it's better to first check whether interface1D() returns non-zero, and only then use it.
    double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
    double dataKey = plottable->interface1D()->dataMainKey(dataIndex);
    QString message = QString("(%1 , %2)").arg(dataKey).arg(dataValue);


    QCPItemText *textLabel = new QCPItemText(plot1_);
    textLabel->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
    textLabel->position->setType(QCPItemPosition::ptPlotCoords);
    textLabel->position->setAxisRect(plottable->keyAxis()->axisRect());
    textLabel->position->setCoords(dataKey, dataValue); // place position at center/top of axis rect

    textLabel->setFont(QFont(font().family(), 8)); // make font a bit larger
    //textLabel->setPen(QPen(Qt::black)); // show black border around text
    textLabel->setText(message);

    // add the arrow:
    QCPItemLine *arrow = new QCPItemLine(plot1_);
    arrow->start->setParentAnchor(textLabel->bottom);
    arrow->end->setCoords(dataKey, dataValue);

    userItems_.push_back(std::make_tuple<QCPAbstractItem *, QCPAbstractItem *>(textLabel, arrow));


#ifdef DEBUG
    std::cout << "Added item to plot: " << textLabel->text().toStdString() << std::endl;
#endif
}

void GSWidget::clearAllGraphItems(bool checked) {

#ifdef DEBUG
    std::cout << "Cleared all graph items" << std::endl;
#endif

    plot1_->clearItems();
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