#ifndef GSWIDGET_H
#define GSWIDGET_H

#include <QtWidgets/QWidget>
#include <DataStructures/datastructs.h>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <Qt3DCore/QEntity>
#include <3D/Line/Line.h>
#include <3D/Scene/RootEntity.h>
#include "MainWorker.h"
#include "ProgramConstants.h"

namespace Ui {
    class GSWidget;
}


class GSWidget : public QWidget
{
    Q_OBJECT

public:
    GSWidget(QWidget *, std::string);

    bool event(QEvent *) override;

    explicit GSWidget(QWidget *parent = nullptr);

    ~GSWidget() override;

    void setReplayMode();

    void setRealTimeMode();

public slots:

    void dummySlot(bool);

    void graphClicked(QCPAbstractPlottable *, int);

    void updateTime();

    void updateEvents(vector<RocketEvent> &);

    void updateGraphData(QVector<QCPGraphData> &, GraphFeature);

    void clearAllGraphItems(bool);

    void updateTelemetry(TelemetryReading);

    void updateLoggingStatus(bool);

    void updateLinkStatus(HandlerStatus);

    void updateGroundStatus(float, float);

    void mouseWheelOnPlot();

    void mousePressOnPlot();

    void updateAutoPlay(bool);

    void updatePlotSync(bool);

    void increaseSpeed();

    void decreaseSpeed();

    void resetPlayback();

    void reversePlayback();

    // 3D visualisation slots
    void register3DPoints(const QVector<QVector3D> &);

signals:

    void toggleLogging();

    void changePlaybackSpeed(double);

    void resetTelemetryReplayPlayback();

    void reverseTelemetryReplayPlayback(bool);

private:

    void graphWidgetSetup();

    void plotSetup(QCustomPlot *, QString, QColor);

    void connectComponents();

    void applyToAllPlots(const std::function<void(QCustomPlot *)> &);

    Ui::GSWidget *ui;
    QCustomPlot *plot1_;
    QCustomPlot *plot2_;
    QCPMarginGroup plotMargin_;
    std::vector<QCustomPlot *> plotVector_;
    QTimer clockTimer;
    chrono::system_clock::time_point lastGraphUpdate_;
    std::vector<std::tuple<QCPAbstractItem *, QCPAbstractItem *>> userItems_;
    bool autoPlay_;
    bool replayMode_;
    bool playbackReversed_;
    double lastRemoteTime_;
    double replayPlaybackSpeed_;
    Line *currentTrace_;
    RootEntity *rootEntity3D_;

    const QFlags<QCP::Interaction> interactionItemsOnly_ = QCP::iSelectItems;
    const QFlags<QCP::Interaction> interactionsAll_ = QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems;
};

#endif // GSWIDGET_H
