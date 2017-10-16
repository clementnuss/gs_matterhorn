#ifndef MAINWORKER_H
#define MAINWORKER_H

#include <QtWidgets/QWidget>
#include <DataStructures/datastructs.h>
#include <DataHandlers/TelemetryHandler.h>
#include <qcustomplot.h>
#include "GraphFeature.h"
#include "FileLogger.h"

using namespace std;
class Worker : public QObject
{
    Q_OBJECT

public:
    Worker();
    ~Worker() override;

public slots:
    void run();

    void mainRoutine();

signals:

    void telemetryReady(TelemetryReading);
    void graphDataReady(QVector<QCPGraphData> &, GraphFeature);

    void newEventsReady(vector<RocketEvent> &);
    void linkStatusReady(bool, bool);
    void groundStatusReady(float, float);
    void dummySignal();

private:
    bool enabled;

    void displayMostRecentTelemetry(TelemetryReading);

    unique_ptr<TelemetryHandler> telemetryHandler;
    FileLogger telemetryLogger;
    FileLogger eventLogger;
    TelemetryReading lastDisplayableReading;
    chrono::system_clock::time_point lastUIupdate;

    QVector<QCPGraphData> extractGraphData(vector<TelemetryReading> &, QCPGraphData (*)(TelemetryReading));
};

#endif // WORKER_H
