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

signals:

    void telemetryReady(TelemetryReading);

    void graphDataReady(QVector<QCPGraphData> &, GraphFeature);
    void linkStatusReady(bool, bool);
    void groundStatusReady(float, float);

    void dummySignal();
private:
    bool enabled;

    void logData(vector<TelemetryReading> &);
    unique_ptr<TelemetryHandler> telemetryHandler;
    FileLogger telemetryLogger;

    QVector<QCPGraphData> extractGraphData(vector<TelemetryReading> &, QCPGraphData (*)(TelemetryReading));
};

#endif // WORKER_H
