#ifndef GSWIDGET_H
#define GSWIDGET_H

#include <QtWidgets/QWidget>
#include <DataStructures/datastructs.h>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <Qt3DCore/QEntity>
#include <3D/Line/Line.h>
#include <3D/Scene/RootEntity.h>
#include <ui_gswidget.h>
#include "MainWorker.h"
#include "ProgramConstants.h"

namespace Ui {
    class GSMainwindow;
}


class GSMainwindow : public QMainWindow {
Q_OBJECT

public:

    Ui::GS_mainWindow *ui;

    bool event(QEvent *) override;

    explicit GSMainwindow();

    ~GSMainwindow() override;

    void setReplayMode();

    void setRealTimeMode();

    const RootEntity *get3DModule();

public slots:

    /* TELEMETRY-RELATED SLOTS */
    void receiveSensorData(const SensorsPacket);

    void receiveEventData(const EventPacket);

    void receiveGPSData(const GPSPacket);

    void receiveGraphData(QVector<QCPGraphData> &, GraphFeature);


    /* USER INTERACTION SLOTS */
    void graphClicked(QCPAbstractPlottable *, int);

    void mouseWheelOnPlot();

    void mousePressOnPlot();


    /* STATUS UPDATE SLOTS */
    void updateLoggingStatus(bool);

    void updateLinkStatus(HandlerStatus);

    void updateGroundStatus(float, float);


    /* CONTROL SLOTS */
    void updateTime();

    void clearAllGraphItems(bool);

    void resetUIState();

    void updateAutoPlay(bool);

    void updatePlotSync(bool);

    void increaseSpeed();

    void decreaseSpeed();

    void resetPlayback();

    void reversePlayback();

    /* 3D VISUALISATION SLOTS */
    void registerStatus(QVector<QVector3D> &, const QVector3D &);

    void registerEvent(const EventPacket &);

    void registerInfoString(const QString &);

    void highlightInfoString(int n);

#if TEST3D

    void dummyAnimation();

#endif

signals:

    void sendCommand(int);

    void toggleLogging();

    void toggleTracking();

    void changePlaybackSpeed(double);

    void resetTelemetryReplayPlayback();

    void reverseTelemetryReplayPlayback(bool);

    void defineReplayMode(const QString &);

    void defineRealtimeMode(const QString &);

private:

    void setupGraphWidgets();

    void setupPlots(QCustomPlot *, QString, QColor, bool labelTimeAxis);

    void setup3DModule();

    void connectComponents();

    void applyToAllPlots(const std::function<void(QCustomPlot *)> &);

    void highlightLine(int, bool, QTextCursor &cursor);

    void changeToReplayModeAction();

    void changeToRealTimeModeAction();

    double groundAltitude_;
    int prevInfoHighlight_;
    QTimer clockTimer;

    // Playback
    bool autoPlay_;
    bool replayMode_;
    bool playbackReversed_;
    double lastRemoteTime_;
    double replayPlaybackSpeed_;

    // QCustomPlot
    const QFlags<QCP::Interaction> interactionItemsOnly_ = QCP::iSelectItems;
    const QFlags<QCP::Interaction> interactionsAll_ = QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems;
    std::vector<std::tuple<QCPAbstractItem *, QCPAbstractItem *>> userItems_;
    QCustomPlot *plot1_;
    QCustomPlot *plot2_;
    std::vector<QCustomPlot *> plotVector_;
    QCPMarginGroup plotMargin_;
    chrono::system_clock::time_point lastGraphUpdate_;

    // 3D Module
    Line *currentTrace_;
    RootEntity *rootEntity3D_;
    QTime animationTriggerTime_;
    QVector<QVector3D> traceData_;
};

#endif // GSWIDGET_H
