#ifndef MAINWORKER_H
#define MAINWORKER_H

#include <QtWidgets/QWidget>
#include <DataStructures/datastructs.h>
#include <DataHandlers/TelemetryHandler.h>
#include <qcustomplot.h>
#include <chrono>
#include "GraphFeature.h"
#include "FileLogger.h"

using namespace std;
class Worker : public QObject
{
    Q_OBJECT

public:
    explicit Worker(std::string);
    ~Worker() override;

    void mainRoutine();

public slots:
    void run();

    void emitAllStatuses();
    void updateLoggingStatus();

signals:

    void loggingStatusReady(bool);

    void telemetryReady(TelemetryReading);
    void graphDataReady(QVector<QCPGraphData> &, GraphFeature);

    void newEventsReady(vector<RocketEvent> &);

    void linkStatusReady(HandlerStatus);
    void groundStatusReady(float, float);
    void dummySignal();

private:
    bool loggingEnabled;

    void checkLinkStatuses();
    void displayMostRecentTelemetry(TelemetryReading);

    unique_ptr<TelemetryHandler> telemetryHandler;
    FileLogger telemetryLogger;
    FileLogger eventLogger;
    TelemetryReading lastDisplayableReading;
    chrono::system_clock::time_point lastUIupdate;
    chrono::system_clock::time_point timeOfLastLinkCheck;
    chrono::system_clock::time_point timeOfLastReceivedTelemetry;
    long long int millisBetweenLastTwoPackets;

    QVector<QCPGraphData> extractGraphData(vector<TelemetryReading> &, QCPGraphData (*)(TelemetryReading));
};

#endif // WORKER_H
