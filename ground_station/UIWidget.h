#ifndef GSWIDGET_H
#define GSWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QThread>
#include "DataStructures/datastructs.h"
#include "MainWorker.h"

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

    void updateTelemetry(TelemetryReading);
    void updateLinkStatus(bool, bool);
    void updateGroundStatus(float, float);

private:
    Ui::GSWidget *ui;
    QTimer clockTimer;

    const QString UP = "UP";
    const QString DOWN = "DOWN";

    QThread workerThread;
    Worker* worker;
};

#endif // GSWIDGET_H
