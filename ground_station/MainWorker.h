#ifndef WORKER_H
#define WORKER_H

#include <QtWidgets/QWidget>
#include <DataStructures/datastructs.h>
#include <DataHandlers/TelemetryHandler.h>
#include <qcustomplot.h>
#include "GraphFeature.h"

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

    void graphDataReady(QVector<QCPGraphData>, GraphFeature);
    void linkStatusReady(bool, bool);
    void groundStatusReady(float, float);
    void dummySignal();

private:
    bool enabled;
    TelemetryHandler telemetryHandler;

    QVector<QCPGraphData> graphSpeedData(vector<TelemetryReading> &);
};

#endif // WORKER_H
