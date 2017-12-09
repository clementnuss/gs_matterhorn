#include "UIWidget.h"
#include "ui_gswidget.h"
#include "UI/Colors.h"
#include <Qt3DInput/QInputAspect>
#include <cassert>
#include <iostream>

GSWidget::GSWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::GSWidget),
        clockTimer(this),
        currentTrace_{nullptr},
        rootEntity3D_{nullptr}
{
    ui->setupUi(this);
    graphSetup();

    connect(&clockTimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    clockTimer.start(std::lround((1.0 / 60.0) * 1000));


    Qt3DExtras::Qt3DWindow *view = new Qt3DExtras::Qt3DWindow();
    //view->defaultFrameGraph()->setClearColor(QColor("steelblue"));
    QWidget *container = QWidget::createWindowContainer(view);
    container->setMinimumSize(QSize(200, 100));

    Qt3DInput::QInputAspect *input = new Qt3DInput::QInputAspect;

    view->registerAspect(input);


    rootEntity3D_ = new RootEntity(view, nullptr);
    view->setRootEntity(rootEntity3D_);


    ui->stackedWidget->removeWidget(ui->stackedWidget->widget(1));
    ui->stackedWidget->addWidget(container);

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

    // Clear any eventual datapoint ahead of current time point
    g->data()->removeAfter(d.last().key);

    g->data()->add(d);

    int sizeDiff = g->data()->size() - DataConstants::MAX_DATA_VECTOR_SIZE;
    if (sizeDiff > 0) {
        g->data()->removeBefore(
                (g->data()->at(static_cast<int>(
                                       DataConstants::DELETION_FACTOR *
                                       (sizeDiff + DataConstants::MAX_DATA_VECTOR_SIZE)))->key)
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


void GSWidget::register3DPoints(const QVector<QVector3D> &positions) {
    rootEntity3D_->updateRocketTracker(positions);
}

void GSWidget::graphSetup() {
    QCustomPlot *customPlot = ui->graph_widget;
    customPlot->plotLayout()->clear();

    // TODO: check if needed on RaspberryPi3
    //customPlot->setOpenGl(true);

    QFont titleFont = QFont("sans", 10, QFont::Bold);

    QCPTextElement *topTitle = new QCPTextElement(customPlot, "Altitude", titleFont);
    QCPTextElement *bottomTitle = new QCPTextElement(customPlot, "Acceleration", titleFont);

    QCPAxisRect *topAxisRect = new QCPAxisRect(customPlot);
    QCPAxisRect *bottomAxisRect = new QCPAxisRect(customPlot);

    topAxisRect->setupFullAxesBox(true);
    bottomAxisRect->setupFullAxesBox(true);

    customPlot->plotLayout()->addElement(0, 0, topTitle);
    customPlot->plotLayout()->addElement(1, 0, topAxisRect);
    customPlot->plotLayout()->addElement(2, 0, bottomTitle);
    customPlot->plotLayout()->addElement(3, 0, bottomAxisRect);

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
        } else if (ke->key() == Qt::Key_Control) {
            ui->stackedWidget->setCurrentIndex((ui->stackedWidget->currentIndex() + 1) % ui->stackedWidget->count());
            return true;
        }

    }

    return QWidget::event(event);
}


GSWidget::~GSWidget() {
    delete ui;
    delete currentTrace_;
}
