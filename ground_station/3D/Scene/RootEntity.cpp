
#include <3D/Ground/Ground.h>
#include <3D/GroundStation/GroundStation.h>
#include <3D/Billboards/Tracker.h>
#include <3D/ForwardRenderer/ForwardRenderer.h>
#include <3D/Objects/SplashDownPredictor.h>
#include <3D/Objects/OpenGL3DAxes.h>
#include <Utilities/ReaderUtils.h>
#include <FileReader.h>
#include <3D/Utils.h>
#include <3D/Objects/Ruler.h>
#include <3D/ForwardRenderer/LayerManager.h>
#include "RootEntity.h"

RootEntity::RootEntity(Qt3DExtras::Qt3DWindow *view, Qt3DCore::QNode *parent) :
        Qt3DCore::QEntity(parent),
        cameraController_{new Qt3DExtras::QFirstPersonCameraController(this)},
        camera_{view->camera()},
        rocketTracker_{nullptr},
        rocketTrace_{nullptr},
        simTrace_{nullptr},
        rocketRuler_{nullptr} {

    auto *renderSettings = new Qt3DRender::QRenderSettings();
    auto *forwardRenderer = new ForwardRenderer(view, renderSettings);
    renderSettings->setActiveFrameGraph(forwardRenderer);

    this->addComponent(renderSettings);
    this->addComponent(LayerManager::getInstance().getLayer(LayerType::VISIBLE));

    cameraController_->setCamera(camera_);
    cameraController_->setLinearSpeed(1000.0);

    auto *ground = new Ground(this);
    rocketTracker_ = new Tracker(QVector3D{0, 20, 0}, camera_, TextureConstants::CARET_DOWN, QStringLiteral("ROCKET"),
                                 this,
                                 OpenGLConstants::ABOVE, OpenGLConstants::ABOVE_CENTER_LABEL);

    rocketTrace_ = new Line(this, QColor::fromRgb(255, 255, 255), false);
    rocketTrace_->setData({{0, 0, 0},
                           {0, 0, 0}});

    // Initialise simulated rocket trace
    simTrace_ = new Line(this, QColor::fromRgb(255, 0, 0), true);

    std::string tracePath{"../../ground_station/data/simulated_trajectory.csv"};
    FileReader<QVector3D> traceReader{tracePath, posFromString};

    QVector<QVector3D> traceData = traceReader.readFile();

    new Tracker(traceData.last(), view->camera(), TextureConstants::CARET_DOWN, QStringLiteral("SIM"), this,
                OpenGLConstants::ABOVE, OpenGLConstants::ABOVE_RIGHT);
    simTrace_->appendData(traceData);

    QVector3D gsPos{3000, 50, -700};

    new GroundStation(gsPos, TextureConstants::DOUBLE_DOWN_ARROW, camera_, this);

    std::string meteoPath{"../../ground_station/MeteoData/meteo_payerne_test.txt"};
    SplashDownPredictor splashDownPredictor{meteoPath, this};
    splashDownPredictor.updatePos(traceData.last());

    ground->highlightArea(splashDownPredictor.getTouchdownCoordinates());


    new OpenGL3DAxes(this);
    QVector3D initialPos{0, 0, 0};
    rocketRuler_ = new Ruler(initialPos, camera_, TextureConstants::CARET_LEFT, this);


    camera_->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
    camera_->setFieldOfView(45.0);
    camera_->setNearPlane(0.1);
    camera_->setFarPlane(100000.0);
    camera_->setPosition(QVector3D{-5000.0, 2000.0, -3000.0});
    camera_->setUpVector(QVector3D{0.0, 1.0, 0.0});
    camera_->setViewCenter(QVector3D{0.0, 2000.0, 0.0});
}

void RootEntity::updateRocketTracker(const QVector<QVector3D> &positions) {
    rocketTracker_->updatePosition(positions.last());
    rocketRuler_->updatePosition(positions.last());
    rocketTrace_->appendData(positions);
}