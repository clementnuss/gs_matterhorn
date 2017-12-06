
#include <3D/Ground/Ground.h>
#include <3D/GroundStation/GroundStation.h>
#include <3D/Billboards/Tracker.h>
#include <3D/ForwardRenderer/ForwardRenderer.h>
#include <3D/Objects/SplashDownPredictor.h>
#include <3D/Objects/OpenGL3DAxes.h>
#include <Utilities/ReaderUtils.h>
#include <FileReader.h>
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
    camera->setPosition(QVector3D{-5000.0, 2000.0, -4000.0});
    camera->setUpVector(QVector3D{0.0, 1.0, 0.0});
    camera->setViewCenter(QVector3D{0.0, 2000.0, 0.0});

    auto *renderSettings = new Qt3DRender::QRenderSettings();
    auto *forwardRenderer = new ForwardRenderer(view, renderSettings);
    renderSettings->setActiveFrameGraph(forwardRenderer);

    this->addComponent(renderSettings);

    cameraController_->setCamera(camera);
    cameraController_->setLinearSpeed(1000.0);

    auto *ground = new Ground(this);
    rocketTracker_ = new Tracker(QVector3D{0, 20, 0}, view->camera(),
                                 QUrl(QStringLiteral("qrc:/3D/textures/caret_down.png")),
                                 QUrl(QStringLiteral("qrc:/3D/textures/text/rocket.png")),
                                 QStringLiteral("ROCKET"),
                                 this);

    rocketTrace_ = new Line(this, QColor::fromRgb(255, 255, 255), false);

    // Initialise simulated rocket trace
    simTrace_ = new Line(this, QColor::fromRgb(255, 0, 0), true);

    std::string tracePath{"../../ground_station/data/simulated_trajectory.csv"};
    FileReader<QVector3D> traceReader{tracePath, posFromString};

    QVector<QVector3D> traceData = traceReader.readFile();

    new Tracker(traceData.last(), view->camera(),
                QUrl(QStringLiteral("qrc:/3D/textures/caret_down.png")),
                QUrl(QStringLiteral("qrc:/3D/textures/text/sim.png")),
                QStringLiteral("SIM"),
                this);
    simTrace_->appendData(traceData);

    // Initialise
    splashDownTrace_ = new Line(this, QColor::fromRgb(255, 255, 255), true);
    splashDownTrace_->appendData(traceData.last());
    splashDownTrace_->appendData({traceData.last().x(), 0, traceData.last().z()});

    QVector3D gsPos{50, 50, 0};

    new GroundStation(gsPos, camera, this);

    std::string meteoPath{"../../ground_station/MeteoData/meteo_payerne_test.txt"};
    SplashDownPredictor splashDownPredictor{meteoPath, this};
    splashDownPredictor.updatePos(traceData.last());

    ground->highlightArea(splashDownPredictor.getTouchdownCoordinates());


    new OpenGL3DAxes(this);


}

void RootEntity::updateRocketTracker(const QVector<QVector3D> &positions) {
    rocketTracker_->updatePosition(positions.last());
    rocketTrace_->appendData(positions);
}