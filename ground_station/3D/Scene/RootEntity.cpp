
#include <3D/Ground/Ground.h>
#include <3D/GroundStation/GroundStation.h>
#include <3D/Billboards/Tracker.h>
#include <3D/ForwardRenderer/ForwardRenderer.h>
#include <3D/TraceReader.h>
#include <iostream>
#include "RootEntity.h"

RootEntity::RootEntity(Qt3DExtras::Qt3DWindow *view, Qt3DCore::QNode *parent) :
        Qt3DCore::QEntity(parent),
        cameraController_{new Qt3DExtras::QFirstPersonCameraController(this)},
        rocketTracker_{nullptr},
        rocketTrace_{nullptr},
        simTrace_{nullptr} {


    Qt3DRender::QCamera *camera = view->camera();
    camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
    camera->setFieldOfView(45.0);
    camera->setNearPlane(0.1);
    camera->setFarPlane(100000.0);
    camera->setPosition(QVector3D{-500.0, 2000.0, -400.0});
    camera->setUpVector(QVector3D{0.0, 1.0, 0.0});
    camera->setViewCenter(QVector3D{0.0, 2000.0, 0.0});

    auto *renderSettings = new Qt3DRender::QRenderSettings(this);
    auto *forwardRenderer = new ForwardRenderer(view, renderSettings);
    renderSettings->setActiveFrameGraph(forwardRenderer);

    this->addComponent(renderSettings);

    cameraController_->setCamera(camera);
    cameraController_->setLinearSpeed(1000.0);

    new Ground(this);
    rocketTracker_ = new Tracker(QVector3D{0, 20, 0}, view->camera(),
                                 QUrl(QStringLiteral("qrc:/3D/textures/caret_down.png")), QStringLiteral("ROCKET"),
                                 this);
    rocketTrace_ = new Line(this, QColor::fromRgb(255, 255, 255), false);

    // Initialise simulated rocket trace
    simTrace_ = new Line(this, QColor::fromRgb(255, 0, 0), true);
    TraceReader traceReader{40};
    QVector <QVector3D> traceData = traceReader.read({"../../ground_station/data/simulated_trajectory.csv"});

    std::cout << traceData.size();

    new Tracker(traceData.last(), view->camera(), QUrl(QStringLiteral("qrc:/3D/textures/caret_down.png")),
                QStringLiteral("SIMTRACE"),
                this);
    simTrace_->appendData(traceData);

    // Initialise
    splashDownTrace_ = new Line(this, QColor::fromRgb(255, 153, 0), true);
    splashDownTrace_->appendData(traceData.last());
    splashDownTrace_->appendData({traceData.last().x(), 0, traceData.last().z()});

    QVector3D gsPos{50, 50, 0};

    new GroundStation(gsPos, camera, this);

}

void RootEntity::updateRocketTracker(const QVector<QVector3D> &positions) {
    rocketTracker_->updatePosition(positions.last());
    rocketTrace_->appendData(positions);
}