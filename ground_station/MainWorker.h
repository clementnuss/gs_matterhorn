#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#include "DataStructures/datastructs.h"

class Worker : public QObject
{
    Q_OBJECT

public:
    Worker();
    ~Worker();

public slots:
    void run();

signals:

    void telemetryReady(TelemetryReading);
    void linkStatusReady(bool, bool);
    void groundStatusReady(float, float);
    void dummySignal();

private:
    bool enabled;
};

#endif // WORKER_H
