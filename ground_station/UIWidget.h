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

public slots:
    void dummySlot();

    void updateTime();
    void updateEvents(vector<RocketEvent> &);
    void updateGraphData(QVector<QCPGraphData> &, GraphFeature);
    void updateTelemetry(TelemetryReading);
    void updateLoggingStatus(bool);
    void updateLinkStatus(HandlerStatus);
    void updateGroundStatus(float, float);

    // 3D visualisation slots
    void register3DPoints(const QVector<QVector3D> &);

signals:
    void toggleLogging();

private:
    Ui::GSWidget *ui;
    QTimer clockTimer;
    Line *currentTrace_;
    RootEntity *rootEntity3D_;

    void graphSetup();
};

#endif // GSWIDGET_H
