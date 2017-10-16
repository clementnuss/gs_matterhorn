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


class GSWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GSWidget(QWidget *parent = nullptr);

    ~GSWidget() override;

public slots:
    void dummySlot();
    void updateTime();

    void updateEvents(vector<RocketEvent> &);
    void updateGraphData(QVector<QCPGraphData> &, GraphFeature);
    void updateTelemetry(TelemetryReading);
    void updateLinkStatus(bool, bool);
    void updateGroundStatus(float, float);


private:
    Ui::GSWidget *ui;
    QTimer clockTimer;

    const QString UP = "UP";
    const QString DOWN = "DOWN";

    QThread workerThread;
    Worker *worker;

    void graphSetup();
};

#endif // GSWIDGET_H
