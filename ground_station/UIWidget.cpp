#include "UIWidget.h"
#include "ui_gswidget.h"
#include "UI/Colors.h"
#include <iostream>
#include <Utilities/TimeUtils.h>

GSWidget::GSWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::GSWidget),
        plot1_{new QCustomPlot(this)},
        plot2_{new QCustomPlot(this)},
        plotVector_{plot1_, plot2_},
        clockTimer(this),
        lastGraphUpdate_{chrono::system_clock::now()},
        userItems_{std::vector<std::tuple<QCPAbstractItem *, QCPAbstractItem *>>()},
        lastRemoteTime_{-1000},
        autoPlay_{true} {
    ui->setupUi(this);

    graphWidgetSetup();
    connectComponents();
    clockTimer.start(std::lround((1.0 / 60.0) * 1000));
}

GSWidget::~GSWidget() {
    delete ui;
}

void GSWidget::connectComponents() {
    connect(&clockTimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    connect(ui->graph_clear_items_button, &QPushButton::clicked, this, &GSWidget::clearAllGraphItems);
    connect(ui->graph_autoplay_button, &QPushButton::clicked, this, &GSWidget::updateAutoPlay);
    connect(ui->graph_sync_button, &QPushButton::clicked, this, &GSWidget::updatePlotSync);

    // Connect components related to graphs
    for (auto plot : plotVector_) {
        connect(plot, SIGNAL(plottableClick(QCPAbstractPlottable * , int, QMouseEvent * )), this,
                SLOT(graphClicked(QCPAbstractPlottable * , int)));
        connect(plot, SIGNAL(mouseWheel(QWheelEvent * )), this, SLOT(mouseWheelOnPlot()));
    }
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

        if (autoPlay_) {
            auto elapsed = usecsBetween(lastGraphUpdate_, chrono::system_clock::now());
            if (elapsed > UIConstants::GRAPH_DATA_INTERVAL_USECS) {
                double elapsedSeconds = elapsed / 1'000'000.0;

                for (int g_idx = 0; g_idx < plotVector_.size(); g_idx++) {
                    QCPGraph *g = plotVector_[g_idx]->graph();
                    g->keyAxis()->setRange(lastRemoteTime_ + elapsedSeconds,
                                           UIConstants::GRAPH_XRANGE_SECS,
                                           Qt::AlignRight);
                    plotVector_[g_idx]->replot();
                };

            }
        }
        return;
    }

    lastGraphUpdate_ = chrono::system_clock::now();
    lastRemoteTime_ = d.last().key;

    QCPGraph *g = plotVector_[static_cast<int>(feature)]->graph();

    // Clear any eventual datapoint ahead of current time point
    g->data()->removeAfter(d.last().key);

    g->data()->add(d);

    g->data()->removeBefore(d.last().key - UIConstants::GRAPH_MEMORY_SECS);

    if (autoPlay_) {
        g->keyAxis()->setRange(d.last().key, UIConstants::GRAPH_XRANGE_SECS, Qt::AlignRight);
        g->valueAxis()->rescale(true);
        g->valueAxis()->scaleRange(UIConstants::GRAPH_RANGE_MARGIN_RATIO);
    }

    plotVector_[static_cast<int>(feature)]->replot();
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

void GSWidget::graphWidgetSetup() {
    QWidget *plotContainer = ui->plot_container;

    plotSetup(plot1_, QStringLiteral("Altitude [m]"), QColor(180, 0, 0));
    plotSetup(plot2_, QStringLiteral("Acceleration [G]"), QColor(0, 180, 0));

    QVBoxLayout *layout = new QVBoxLayout(plotContainer);
    layout->addWidget(plot1_);
    layout->addWidget(plot2_);

    updatePlotSync(ui->graph_sync_button->isChecked());
    // Check if the number of graphs corresponds to the number of available features
    //assert(plot1_->graphCount() == static_cast<int>(GraphFeature::Count));

}

void GSWidget::plotSetup(QCustomPlot *plot, QString title, QColor color) {
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems);
    plot->plotLayout()->clear();

    // TODO: check if needed on RaspberryPi3
    //plot->setOpenGl(true);

    QFont titleFont = QFont("sans", 10, QFont::Bold);

    QCPTextElement *titleText = new QCPTextElement(plot, title, titleFont);

    auto axisRect = new QCPAxisRect(plot);

    axisRect->setRangeDrag(Qt::Horizontal);
    axisRect->setupFullAxesBox(true);

    plot->plotLayout()->addElement(0, 0, titleText);
    plot->plotLayout()->addElement(1, 0, axisRect);

    QFont font;
    font.setPointSize(12);
    axisRect->axis(QCPAxis::atLeft, 0)->setTickLabelFont(font);
    axisRect->axis(QCPAxis::atBottom, 0)->setTickLabelFont(font);
//    This may be useful when implementing event, so as to display them with 2 digits precision on the graph.
//    topAxisRect->axis(QCPAxis::atBottom, 0)->setNumberFormat("f");
//    topAxisRect->axis(QCPAxis::atBottom, 0)->setNumberPrecision(UserIfaceConstants::PRECISION);
    axisRect->axis(QCPAxis::atLeft, 0)->setTickLabelFont(font);
    axisRect->axis(QCPAxis::atBottom, 0)->setTickLabelFont(font);

    QPen pen;
    pen.setWidth(1);
    pen.setColor(color);

    QList<QCPAxis *> allAxes;
    allAxes << axisRect->axes();

            foreach (QCPAxis *axis, allAxes) {
            axis->setLayer("axes");
            axis->grid()->setLayer("grid");
        }

    QCPGraph *g1 = plot->addGraph(axisRect->axis(QCPAxis::atBottom), axisRect->axis(QCPAxis::atLeft));

    g1->setPen(pen);
}

void GSWidget::graphClicked(QCPAbstractPlottable *plottable, int dataIndex) {

    //TODO: Add the items to a list so we can reaccess them later

    // since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
    // usually it's better to first check whether interface1D() returns non-zero, and only then use it.
    double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
    double dataKey = plottable->interface1D()->dataMainKey(dataIndex);
    QString message = QString("(%1 , %2)").arg(dataKey).arg(dataValue);


    QCPItemText *textLabel = new QCPItemText(plottable->parentPlot());
    textLabel->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
    textLabel->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
    textLabel->position->setCoords(dataKey, 0.0); // place position at center/top of axis rect

    textLabel->setFont(QFont(font().family(), 8)); // make font a bit larger
    //textLabel->setPen(QPen(Qt::black)); // show black border around text
    textLabel->setText(message);

    // add the arrow:
    QCPItemLine *arrow = new QCPItemLine(plottable->parentPlot());
    arrow->start->setParentAnchor(textLabel->bottom);
    arrow->end->setCoords(dataKey, dataValue);

    userItems_.push_back(std::make_tuple<QCPAbstractItem *, QCPAbstractItem *>(textLabel, arrow));


#ifdef DEBUG
    std::cout << "Added item to plot: " << textLabel->text().toStdString() << std::endl;
#endif
}

void GSWidget::mouseWheelOnPlot() {
    autoPlay_ = false;
    ui->graph_autoplay_button->setChecked(false);

    plot1_->axisRect()->setRangeZoom(
            (Qt::ShiftModifier & QGuiApplication::keyboardModifiers()) ? Qt::Horizontal : Qt::Vertical);
}

void GSWidget::updateAutoPlay(bool checked) {
    autoPlay_ = checked;
}

/**
 * Connects or disconnects the x-axis of all plots
 *
 * @param checked the boolean value indicating synchronisation
 */
void GSWidget::updatePlotSync(bool checked) {
    if (checked) {
        for (int i = 0; i < plotVector_.size(); i++) {
            for (int j = 0; j < plotVector_.size(); j++) {
                if (i == j) {
                    continue;
                } else {
                    connect(plotVector_[i]->xAxis, SIGNAL(rangeChanged(QCPRange)), plotVector_[j]->xAxis,
                            SLOT(setRange(QCPRange)));
                }
            }
        }
    } else {
        for (int i = 0; i < plotVector_.size(); i++) {
            for (int j = 0; j < plotVector_.size(); j++) {
                if (i == j) {
                    continue;
                } else {
                    disconnect(plotVector_[i]->xAxis, SIGNAL(rangeChanged(QCPRange)), plotVector_[j]->xAxis,
                               SLOT(setRange(QCPRange)));
                }
            }
        }
    }
}

void GSWidget::clearAllGraphItems(bool checked) {

#ifdef DEBUG
    std::cout << "Cleared all graph items" << std::endl;
#endif
    for (auto plot : plotVector_) {
        plot->clearItems();
    }
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