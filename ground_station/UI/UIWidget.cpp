#include "UIWidget.h"
#include "UI/Colors.h"
#include "Readers/FileReader.h"
#include <Qt3DInput/QInputAspect>
#include <Utilities/ReaderUtils.h>

/**
 * GSMainwindow is the main user interface class. It communicate through Qt SLOTS system with the
 * main worker thread of the application in order to constantly display fresh data.
 */
GSMainwindow::GSMainwindow() :
        ui(new Ui::GS_mainWindow),
        clockTimer(this),
        currentTrace_{nullptr},
        rootEntity3D_{nullptr},
        plot1_{new QCustomPlot(this)},
        plot2_{new QCustomPlot(this)},
        plotMargin_{nullptr},
        plotVector_{plot1_, plot2_},
        lastGraphUpdate_{chrono::system_clock::now()},
        userItems_{std::vector<std::tuple<QCPAbstractItem *, QCPAbstractItem *>>()},
        lastRemoteTime_{-1000},
        animationTriggerTime_{},
        autoPlay_{true},
        replayMode_{false},
        replayPlaybackSpeed_{1},
        prevInfoHighlight_{0},
        playbackReversed_{false},
        traceData_{} {

    ui->setupUi(this);

    setupGraphWidgets();
    connectComponents();
    clockTimer.start(std::lround((1.0 / 60.0) * 1000));

    ui->graph_clear_items_button->setIcon(QIcon(":/UI/icons/remove.png"));
    ui->graph_sync_button->setIcon(QIcon(":/UI/icons/sync.png"));
    ui->graph_autoplay_button->setIcon(QIcon(":/UI/icons/play.png"));

#if USE_3D_MODULE
    setup3DModule();
#endif
}

GSMainwindow::~GSMainwindow() {
    delete ui;
    delete currentTrace_;
}

/**
 * Connects the UI components such as buttons and plots to appropriate UI slots
 */
void GSMainwindow::connectComponents() {
    connect(&clockTimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    connect(ui->graph_clear_items_button, &QPushButton::clicked, this, &GSMainwindow::clearAllGraphItems);
    connect(ui->graph_autoplay_button, &QPushButton::clicked, this, &GSMainwindow::updateAutoPlay);
    connect(ui->graph_sync_button, &QPushButton::clicked, this, &GSMainwindow::updatePlotSync);
    connect(ui->time_unfolding_increase, &QPushButton::clicked, this, &GSMainwindow::increaseSpeed);
    connect(ui->time_unfolding_decrease, &QPushButton::clicked, this, &GSMainwindow::decreaseSpeed);
    connect(ui->time_unfolding_reset, &QPushButton::clicked, this, &GSMainwindow::resetPlayback);
    connect(ui->time_unfolding_reverse_time, &QPushButton::clicked, this, &GSMainwindow::reversePlayback);
    connect(ui->actionREPLAY_select_file, &QAction::triggered, this, &GSMainwindow::changeToReplayModeAction);
    connect(ui->actionREAL_TIME, &QAction::triggered, this, &GSMainwindow::changeToRealTimeModeAction);

    // Connect components related to graphs
    applyToAllPlots(
            [this](QCustomPlot *p) {
                connect(p, SIGNAL(plottableClick(QCPAbstractPlottable * , int, QMouseEvent * )), this,
                        SLOT(graphClicked(QCPAbstractPlottable * , int)));
                connect(p, SIGNAL(mouseWheel(QWheelEvent * )), this, SLOT(mouseWheelOnPlot()));
                connect(p, SIGNAL(mousePress(QMouseEvent * )), this, SLOT(mousePressOnPlot()));
            }
    );
}


/**
 * Qt SLOT for updating the watch of the user interface
 */
void GSMainwindow::updateTime() {
    ui->ground_time->setText(QTime::currentTime().toString());
}


/**
 * Qt SLOT for receiving telemetry data and displaying it
 *
 * @param t The Telemetry object from which to extract data to update the display
 */
void GSMainwindow::receiveSensorData(const SensorsPacket t) {
    ui->telemetry_altitude_value->setText(QString::number(t.altitude_, 'f', UIConstants::PRECISION));
    ui->telemetry_speed_value->setText(QString::number(t.air_speed_, 'f', UIConstants::PRECISION));
    ui->telemetry_acceleration_value->setText(QString::number(t.acceleration_.norm(), 'f', UIConstants::PRECISION));
    ui->telemetry_pressure_value->setText(QString::number(t.pressure_, 'f', UIConstants::PRECISION));
    ui->telemetry_temperature_value->setText(QString::number(t.temperature_, 'f', UIConstants::PRECISION));
    ui->telemetry_yaw_value->setText(QString::number(t.magnetometer_.x_, 'f', UIConstants::PRECISION));
    ui->telemetry_pitch_value->setText(QString::number(t.magnetometer_.y_, 'f', UIConstants::PRECISION));
    ui->telemetry_roll_value->setText(QString::number(t.magnetometer_.z_, 'f', UIConstants::PRECISION));
}


/**
 * Qt SLOT to receive and display events related to the rocket and payload
 *
 * @param event
 */
void GSMainwindow::receiveEventData(const EventPacket event) {

    int seconds = event.timestamp_ / TimeConstants::MSECS_IN_SEC;
    int minutes = seconds / TimeConstants::SECS_IN_MINUTE;

    stringstream ss;
    ss << "At "
       << setw(TimeConstants::SECS_AND_MINS_WIDTH) << setfill('0') << minutes % TimeConstants::MINUTES_IN_HOUR
       << ":"
       << setw(TimeConstants::SECS_AND_MINS_WIDTH) << setfill('0') << seconds % TimeConstants::SECS_IN_MINUTE
       << ":"
       << setw(TimeConstants::MSECS_WIDTH) << setfill('0') << event.timestamp_ % TimeConstants::MSECS_IN_SEC
       << "    " << (event.description_);

    ui->event_log->appendPlainText(QString::fromStdString(ss.str()));

}


/**
 * Qt SLOT to receive and display gps data from the rocket
 *
 * @param gpsData
 */
void GSMainwindow::receiveGPSData(const GPSPacket gpsData) {

    ui->gps_sats_value->setText(QString::number(gpsData.satsCount_));
    ui->gps_rssi_value->setText(QString::number(gpsData.hdop_, 'f', UIConstants::PRECISION));
    ui->gps_latitude_value->setText(QString::number(gpsData.latitude_, 'f', UIConstants::PRECISION));
    ui->gps_longitude_value->setText(QString::number(gpsData.longitude_, 'f', UIConstants::PRECISION));
}


/**
 * Qt SLOT for adding QCPGraphData objects to a given plot
 *
 * @param d A vector containing all the data points
 * @param feature A GraphFeature enumerated value to indicate to which plot to add the data points
 */
//TODO: only use qvectors or only use vectors
void GSMainwindow::receiveGraphData(QVector<QCPGraphData> &d, GraphFeature feature) {

    if (d.isEmpty()) {

        if (autoPlay_ | playbackReversed_) {
            auto elapsed = usecsBetween(lastGraphUpdate_, chrono::system_clock::now());
            if (elapsed > UIConstants::GRAPH_DATA_INTERVAL_USECS) {
                lastGraphUpdate_ = chrono::system_clock::now();
                double elapsedSeconds = replayPlaybackSpeed_ * elapsed / 1'000'000.0;
                if (playbackReversed_) {
                    lastRemoteTime_ -= elapsedSeconds;
                }
                for (auto &g_idx : plotVector_) {
                    QCPGraph *g = g_idx->graph();
                    if (playbackReversed_) {
                        if (!g->data()->isEmpty()) {
                            g->data()->removeAfter(lastRemoteTime_);
                        }
                        if (autoPlay_) {
                            g->keyAxis()->setRange(lastRemoteTime_,
                                                   UIConstants::GRAPH_XRANGE_SECS,
                                                   Qt::AlignRight);
                        }
                    } else {
                        g->keyAxis()->setRange(lastRemoteTime_ + elapsedSeconds,
                                               UIConstants::GRAPH_XRANGE_SECS,
                                               Qt::AlignRight);
                    }
                    g_idx->replot();

                };

            }
        }
        return;
    }

    QCPGraph *g = plotVector_[static_cast<int>(feature)]->graph();

    auto elapsed = usecsBetween(lastGraphUpdate_, chrono::system_clock::now());
    double elapsedSeconds = replayPlaybackSpeed_ * elapsed / 1'000'000.0;
    lastGraphUpdate_ = chrono::system_clock::now();

    if (playbackReversed_) {
        lastRemoteTime_ -= elapsedSeconds;
    } else { lastRemoteTime_ = d.last().key; }

    // Clear any eventual datapoint ahead of current time point
    if (!replayMode_) { g->data()->removeAfter(d.last().key); }

    g->data()->add(d, /*alreadySorted*/ true);
    if (playbackReversed_) {
        g->data()->removeAfter(lastRemoteTime_);
    } else {
        g->data()->removeBefore(lastRemoteTime_ - UIConstants::GRAPH_MEMORY_SECS);
    }

    if (autoPlay_) {
        g->keyAxis()->setRange(lastRemoteTime_, UIConstants::GRAPH_XRANGE_SECS, Qt::AlignRight);
        g->valueAxis()->rescale(true);
        g->valueAxis()->scaleRange(UIConstants::GRAPH_RANGE_MARGIN_RATIO);
    }

    plotVector_[static_cast<int>(feature)]->replot();
}


/**
 * Qt SLOT for updating the color indicator on the logging label
 *
 * @param enabled True if logging is enabled, false otherwise
 */
void GSMainwindow::updateLoggingStatus(bool enabled) {
    QLabel *label = ui->status_logging;
    QPalette palette = label->palette();
    palette.setColor(label->backgroundRole(), enabled ? UIColors::GREEN : UIColors::RED);
    label->setPalette(palette);
}

void GSMainwindow::updateLinkStatus(HandlerStatus status) {
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
    }

    QLabel *label = ui->status_radio1;
    QPalette palette = label->palette();
    palette.setColor(label->backgroundRole(), statusColor);
    label->setPalette(palette);

}

void GSMainwindow::updateGroundStatus(float temperature, float pressure) {
    ui->ground_temperature_value->setText(QString::number(temperature, 'f', UIConstants::PRECISION));
    ui->ground_temperature_value->setText(QString::number(pressure, 'f', UIConstants::PRECISION));
}

void GSMainwindow::registerStatus(QVector<QVector3D> &positions, const QVector3D &speed) {
    rootEntity3D_->updateRocketTracker(positions, speed);
}

void GSMainwindow::registerEvent(const EventPacket &event) {
    rootEntity3D_->registerEvent(event);
}

void GSMainwindow::registerInfoString(const QString &s) {
    ui->visualisation_info_textedit->textCursor().insertText(s);
}

void GSMainwindow::highlightLine(int lineNumber, bool highlighted, QTextCursor &cursor) {
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNumber);
    cursor.select(QTextCursor::LineUnderCursor);

    QTextCharFormat format{};
    format.setFontWeight(highlighted ? 87 : 50);
    format.setFontUnderline(highlighted);
    format.setFontOverline(highlighted);

    cursor.setCharFormat(format);
}

void GSMainwindow::highlightInfoString(int lineNumber) {

    QTextCursor cursor = ui->visualisation_info_textedit->textCursor();

    highlightLine(prevInfoHighlight_, false, cursor);
    highlightLine(lineNumber, true, cursor);

    cursor.clearSelection();
    prevInfoHighlight_ = lineNumber;

    ui->visualisation_info_textedit->setTextCursor(cursor);
}

void GSMainwindow::setup3DModule() {
    std::string tracePath{"../../ground_station/data/simulated_trajectory.csv"};
    FileReader<QVector3D> traceReader{tracePath, posFromString};

    traceData_ = QVector<QVector3D>::fromStdVector(traceReader.readFile());

    auto *view = new Qt3DExtras::Qt3DWindow();
    QWidget *container = QWidget::createWindowContainer(view);
    container->setMinimumSize(QSize(200, 100));

    auto *input = new Qt3DInput::QInputAspect;

    view->registerAspect(input);

    rootEntity3D_ = new RootEntity(view, nullptr);

    view->setRootEntity(rootEntity3D_);

    ui->visualisation_3D_layout->insertWidget(0, container);
    ui->visualisation_3D_layout->setStretch(0, 3);
    ui->visualisation_3D_layout->setStretch(1, 1);

    connect(rootEntity3D_, &RootEntity::addInfoString, this, &GSMainwindow::registerInfoString);
    connect(rootEntity3D_, &RootEntity::updateHighlightInfoString, this, &GSMainwindow::highlightInfoString);
    rootEntity3D_->init();
}

const RootEntity *GSMainwindow::get3DModule() {
    return rootEntity3D_;
}

/**
 * Setup the container widget which will hold and display all the QCustomPlot objects
 */
void GSMainwindow::setupGraphWidgets() {
    QWidget *plotContainer = ui->plot_container;

    setupPlots(plot1_, QStringLiteral("Altitude [m]"), QColor(180, 0, 0), true);
    setupPlots(plot2_, QStringLiteral("Acceleration [G]"), QColor(0, 180, 0), false);

    auto *layout = new QVBoxLayout(plotContainer);
    layout->addWidget(plot1_);
    layout->addWidget(plot2_);

    updatePlotSync(ui->graph_sync_button->isChecked());
    // Check if the number of graphs corresponds to the number of available features
    //assert(plot1_->graphCount() == static_cast<int>(GraphFeature::Count));

}

/**
 * Setup style and layout for a given QCustomPlot object
 *
 * @param plot The QCustomPlot object for which to setup
 * @param title The title for the plot
 * @param color The color ot use to draw the plot
 */
void GSMainwindow::setupPlots(QCustomPlot *plot, QString title, QColor color, bool labelTimeAxis) {
    plot->setInteractions(interactionItemsOnly_);
    plot->plotLayout()->clear();

    // TODO: check if needed on RaspberryPi3
    //customPlot->setOpenGl(true);

    QFont labelFont = QFont("sans", 10, QFont::Bold);

    auto axisRect = new QCPAxisRect(plot);

    axisRect->setRangeDrag(Qt::Horizontal);
    axisRect->setupFullAxesBox(true);
    axisRect->setMarginGroup(QCP::msLeft | QCP::msRight, &plotMargin_);

    plot->plotLayout()->addElement(0, 0, axisRect);

    QFont font;
    font.setPointSize(12);
    axisRect->axis(QCPAxis::atLeft, 0)->setTickLabelFont(font);
    axisRect->axis(QCPAxis::atBottom, 0)->setTickLabelFont(font);

    axisRect->axis(QCPAxis::atLeft, 0)->setLabel(title);
    if (labelTimeAxis) {
        axisRect->axis(QCPAxis::atBottom, 0)->setLabel("Time [seconds]");
    }
    axisRect->axis(QCPAxis::atLeft, 0)->setLabelFont(labelFont);
    axisRect->axis(QCPAxis::atBottom, 0)->setLabelFont(labelFont);

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

/**
 * Called whenever a plottable object has been clicked. Adds a text label with the value of the function at the
 * clicked point as well as a line pointing to it.
 *
 * @param plottable A pointer to the plottable which was clicked
 * @param dataIndex The index in the graph's data array corresponding to the point clicked
 */
void GSMainwindow::graphClicked(QCPAbstractPlottable *plottable, int dataIndex) {

    double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
    double dataKey = plottable->interface1D()->dataMainKey(dataIndex);
    QString message = QString("(%1 , %2)").arg(dataKey).arg(dataValue);


    auto *textLabel = new QCPItemText(plottable->parentPlot());
    textLabel->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
    textLabel->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
    textLabel->position->setCoords(dataKey, 0.0); // place position at center/top of axis rect

    textLabel->setFont(QFont(font().family(), 8)); // make font a bit larger
    textLabel->setText(message);

    // add the arrow:
    auto *arrow = new QCPItemLine(plottable->parentPlot());
    arrow->start->setParentAnchor(textLabel->bottom);
    arrow->end->setCoords(dataKey, dataValue);

    userItems_.emplace_back(std::make_tuple(textLabel, arrow));


#ifdef DEBUG
    std::cout << "Added item to plot: " << textLabel->text().toStdString() << std::endl;
#endif
}

void GSMainwindow::mouseWheelOnPlot() {
    updateAutoPlay(false);

    // Make all plots respond to wheel events
    applyToAllPlots(
            [](QCustomPlot *p) {
                p->axisRect()->setRangeZoom(
                        (Qt::ShiftModifier & QGuiApplication::keyboardModifiers()) ? Qt::Vertical : Qt::Horizontal);
            }
    );
}

void GSMainwindow::mousePressOnPlot() {
    if (!autoPlay_) {

        // Make all plots respond to mouse events
        applyToAllPlots(
                [](QCustomPlot *p) {
                    p->axisRect()->setRangeDrag(
                            (Qt::ShiftModifier & QGuiApplication::keyboardModifiers()) ? Qt::Vertical : Qt::Horizontal);
                }
        );
    }
}

void GSMainwindow::updateAutoPlay(bool enable) {
    autoPlay_ = enable;
    ui->graph_autoplay_button->setChecked(enable);

    applyToAllPlots(
            [this, enable](QCustomPlot *p) {
                p->setInteractions(enable ? interactionItemsOnly_ : interactionsAll_);
            }
    );
}

/**
 * Connects or disconnects the x-axis of every plot with the x axis of all
 * its siblings.
 *
 * @param checked the boolean value indicating synchronisation
 */
void GSMainwindow::updatePlotSync(bool checked) {
    if (checked) {
        for (int i = 0; i < plotVector_.size(); i++) {
            for (int j = 0; j < plotVector_.size(); j++) {
                // Skip such as not to connect a plot with itself
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

/**
 * When called, removes all the QAbstractItems subclasses that were previously added to any QCustomPlot object
 * @param checked
 */
void GSMainwindow::clearAllGraphItems(bool checked) {
    Q_UNUSED(checked);
    applyToAllPlots([](QCustomPlot *p) { p->clearItems(); });
}

void GSMainwindow::resetUIState() {

    playbackReversed_ = false;
    lastGraphUpdate_ = chrono::system_clock::now();
    lastRemoteTime_ = -1000;
    autoPlay_ = true;
    replayPlaybackSpeed_ = 1;
    ui->time_unfolding_current_speed->setText(
            QString::number(replayPlaybackSpeed_, 'f', 2));

    ui->event_log->clear();
    receiveSensorData(SensorsPacket{});

    for (auto &g_idx : plotVector_) {
        QCPGraph *g = g_idx->graph();
        g->data()->clear();
        g_idx->replot();
    };
}


/**
 * Applies a lambda function to all the QCustomPlots objects of the UI.
 *
 * @param f The lambda function which will be applied to all plots. Should take a pointer to a QCustomPlot object and
 * return void.
 */
void GSMainwindow::applyToAllPlots(const std::function<void(QCustomPlot *)> &f) {
    for (auto &plot : plotVector_) {
        f(plot);
    }
}

void GSMainwindow::increaseSpeed() {
    replayPlaybackSpeed_ *= DataConstants::INCREASE_FACTOR;
    emit changePlaybackSpeed(replayPlaybackSpeed_);
    ui->time_unfolding_current_speed->setText(
            QString::number(replayPlaybackSpeed_, 'f', 2));
}

void GSMainwindow::decreaseSpeed() {
    assert(replayMode_);
    replayPlaybackSpeed_ *= DataConstants::DECREASE_FACTOR;
    emit changePlaybackSpeed(replayPlaybackSpeed_);
    ui->time_unfolding_current_speed->setText(
            QString::number(replayPlaybackSpeed_, 'f', 2));
}

void GSMainwindow::resetPlayback() {
    assert(replayMode_);
    emit resetTelemetryReplayPlayback();
    playbackReversed_ = false;
    lastGraphUpdate_ = chrono::system_clock::now();
    lastRemoteTime_ = -1000;
    autoPlay_ = true;
    replayPlaybackSpeed_ = 1;
    ui->time_unfolding_current_speed->setText(
            QString::number(replayPlaybackSpeed_, 'f', 2));

    ui->event_log->clear();

    for (auto &g_idx : plotVector_) {
        QCPGraph *g = g_idx->graph();
        g->data()->clear();
        g_idx->replot();
    };
}


void GSMainwindow::setRealTimeMode() {
    replayMode_ = false;
    ui->time_unfolding_mode->setText(QString("REAL-TIME"));
    ui->replay_controls->hide();
}

void GSMainwindow::setReplayMode() {
    replayMode_ = true;
    ui->time_unfolding_mode->setText(QString("REPLAY"));
    ui->replay_controls->show();
}

void GSMainwindow::reversePlayback() {
    playbackReversed_ = !playbackReversed_;
    emit reverseTelemetryReplayPlayback(playbackReversed_);
}

/**
 * Captures user events such as keypresses and mouse clicks on the user interface
 * @param event
 * @return
 */
bool GSMainwindow::event(QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        std::cout << "Event" << std::endl;
        auto *ke = dynamic_cast<QKeyEvent *>(event);

        if (ke->key() == Qt::Key_Space) {
            emit toggleLogging();
            return true;
        } else if (ke->key() == Qt::Key_Control) {
            ui->stackedWidget->setCurrentIndex((ui->stackedWidget->currentIndex() + 1) % ui->stackedWidget->count());
            return true;
        }
#if TEST3D
        else if (ke->key() == Qt::Key_S) {

            static bool triggered{false};

            if (!triggered) {
                triggered = true;
                auto *launchTimer = new QTimer();
                launchTimer->start(std::lround((RocketConstants::SIMULATION_DT) * 1000));
                animationTriggerTime_ = QTime::currentTime();
                connect(launchTimer, &QTimer::timeout, this, &GSMainwindow::dummyAnimation);
            }
        } else if (ke->key() == Qt::Key_E) {
            this->registerEvent(EventPacket(0, 0, ""));
        } else if (ke->key() == Qt::Key_T) {
            emit toggleTracking();
        }
#endif

    }
    return QWidget::event(event);
}

void GSMainwindow::changeToReplayModeAction() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Files"), "./", tr("Telemetry data (*)"));
    cout << fileName.toStdString() << endl;

    emit defineReplayMode(fileName);
    setReplayMode();
}

void GSMainwindow::changeToRealTimeModeAction() {
    emit defineRealtimeMode(QString{""});
    setRealTimeMode();
}


#if TEST3D

void GSMainwindow::dummyAnimation() {
    static int i = 0;
    static bool maxV = false;
    static float maxSpeed{0};


    if (i < traceData_.size()) {

        if (i == traceData_.size() - 1) {
            rootEntity3D_->registerEvent({0, 40, ""});
        }

        QVector3D speed{0, 0, 0};
        if (0 < i && i < traceData_.size() - 1) {
            speed = (traceData_[i + 1] - traceData_[i - 1]) / (2.0 * RocketConstants::SIMULATION_DT);

            float speedNorm = speed.length();
            if (speedNorm > maxSpeed) {
                maxSpeed = speedNorm;
            } else if (!maxV) {
                rootEntity3D_->registerEvent({0, 30, ""});
                maxV = true;
            }

        }

        QVector<QVector3D> p{traceData_[i++]};
        this->registerStatus(p, speed);
    }
}

#endif