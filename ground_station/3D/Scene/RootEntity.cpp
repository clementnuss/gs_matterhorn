
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
#include <3D/Objects/Compass.h>
#include "RootEntity.h"
#include "WorldReference.h"

RootEntity::RootEntity(Qt3DExtras::Qt3DWindow *view, Qt3DCore::QNode *parent) :
        Qt3DCore::QEntity(parent),
        cameraController_{new CameraController(this)},
        camera_{view->camera()},
        rocketTracker_{nullptr},
        rocketTrace_{nullptr},
        simTrace_{nullptr},
        rocketRuler_{nullptr},
        registeredEvents_{} {

    auto *renderSettings = new Qt3DRender::QRenderSettings();
    auto *forwardRenderer = new ForwardRenderer(view, renderSettings);
    renderSettings->setActiveFrameGraph(forwardRenderer);

    this->addComponent(renderSettings);
    this->addComponent(LayerManager::getInstance().getLayer(LayerType::VISIBLE));

    cameraController_->setCamera(camera_);
    cameraController_->setLinearSpeed(100.0);
    cameraController_->setLookSpeed(3.0f);


    WorldReference worldRef{LatLon{46.567201, 6.501007}};
    auto *ground = new Ground(this, {0, 0}, worldRef.origin());

    LatLon nextOrigin = worldRef.latLonFromPointAndDistance(worldRef.origin(), 0, 10000);

    std::cout << "Next origin is: " << nextOrigin.latitude << " " << nextOrigin.longitude << std::endl;

    auto *ground2 = new Ground(this, {0, 10000}, nextOrigin);
    rocketTracker_ = new Tracker(QVector3D{0, 20, 0}, camera_, TextureConstants::CARET_DOWN, QStringLiteral("ROCKET"),
                                 TextType::BOLD, this, OpenGLConstants::ABOVE, OpenGLConstants::ABOVE_CENTER_LABEL);

    rocketTrace_ = new Line(this, QColor::fromRgb(255, 255, 255), false);
    rocketTrace_->setData({{0, 0, 0},
                           {0, 0, 0}});

    // Initialise simulated rocket trace
    simTrace_ = new Line(this, QColor::fromRgb(255, 0, 0), true);

    std::string tracePath{"../../ground_station/data/simulated_trajectory.csv"};
    FileReader<QVector3D> traceReader{tracePath, posFromString};

    QVector<QVector3D> traceData = QVector<QVector3D>::fromStdVector(traceReader.readFile());

    simTrace_->appendData(traceData);

    QVector3D gsPos{-2000, 550, 4300};

    GroundStation *gs = new GroundStation(gsPos, TextureConstants::DOUBLE_DOWN_ARROW, camera_, this);

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

    cameraController_->registerObservable(rocketTracker_);
    cameraController_->registerObservable(gs);

    auto *compass = new Compass(this, camera_);
    connect(camera_, &Qt3DRender::QCamera::viewMatrixChanged, compass, &Compass::update);
}

void RootEntity::updateRocketTracker(const QVector<QVector3D> &positions) {
    rocketTracker_->updatePosition(positions.last());
    rocketRuler_->updatePosition(positions.last());
    rocketTrace_->appendData(positions);
}

void RootEntity::registerEvent(const RocketEvent &event) {

    if (EVENT_CODES.find(event.code) == EVENT_CODES.end()) {
        std::cout << "This event code is invalid: " << event.code << std::endl;
        return;
    }

    registeredEvents_.emplace_back(std::make_pair(event.timestamp_, new Tracker(rocketTracker_->getPosition(), camera_,
                                                                                TextureConstants::DOWNWARD_DIAGONAL,
                                                                                QString::fromStdString(
                                                                                        EVENT_CODES.at(event.code)),
                                                                                TextType::LEGEND, this,
                                                                                OpenGLConstants::LEFT_LEGEND_ICON_OFFSET,
                                                                                OpenGLConstants::LEFT_LEGEND_TEXT_OFFSET)));
}