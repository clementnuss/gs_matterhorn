#ifndef GSWIDGET_H
#define GSWIDGET_H

#include <QtWidgets/QWidget>
#include <DataStructures/datastructs.h>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <qcustomplot.h>
#include "MainWorker.h"
#include "ProgramConstants.h"

namespace Ui {
    class GSWidget;
}


class GSWidget : public QWidget {
Q_OBJECT

public:
    GSWidget(QWidget *, std::string);

    bool event(QEvent *) override;

    explicit GSWidget(QWidget *parent = nullptr);

    ~GSWidget() override;

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

signals:

    void toggleLogging();

private:

    void graphWidgetSetup();

    void plotSetup(QCustomPlot *, QString, QColor);

    void connectComponents();

    void applyToAllPlots(const std::function<void(QCustomPlot *)> &);

    Ui::GSWidget *ui;
    QCustomPlot *plot1_;
    QCustomPlot *plot2_;
    std::vector<QCustomPlot *> plotVector_;
    QTimer clockTimer;
    chrono::system_clock::time_point lastGraphUpdate_;
    std::vector<std::tuple<QCPAbstractItem *, QCPAbstractItem *>> userItems_;
    bool autoPlay_;
    double lastRemoteTime_;
};

#endif // GSWIDGET_H
