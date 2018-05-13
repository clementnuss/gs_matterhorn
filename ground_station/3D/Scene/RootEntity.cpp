#include <algorithm>
#include <3D/GroundStation/GroundStation.h>
#include <3D/Billboards/Tracker.h>
#include <3D/ForwardRenderer/ForwardRenderer.h>
#include <3D/Objects/SplashDownPredictor.h>
#include <3D/Objects/OpenGL3DAxes.h>
#include <Utilities/ReaderUtils.h>
#include <3D/Objects/Ruler.h>
#include <3D/ForwardRenderer/LayerManager.h>
#include <3D/Objects/Compass.h>
#include <3D/Grid/CompositeElevationModel.h>
#include <ConfigParser/ConfigParser.h>
#include "RootEntity.h"
#include "Utilities/MathUtils.h"

static void
addToEach(QVector<QVector3D> &v, QVector3D v3d) {
    for (size_t i = 0; i < v.size(); i++) {
        v[i] += v3d;
    }
}

RootEntity::RootEntity(Qt3DExtras::Qt3DWindow *view, Qt3DCore::QNode *parent) :
        Qt3DCore::QEntity(parent),
        cameraController_{new CameraController(this)},
        camera_{view->camera()},
        rocketTrace_{nullptr},
        payloadTrace_{nullptr},
        simTrace_{nullptr},
        rocketRuler_{nullptr},
        splashDownPredictor_{nullptr},
        ground_{nullptr},
        windData_{nullptr},
        groundAltitude_{ConfSingleton::instance().get("launchsite.altitude", 0.0f)},
        lastReportedAltitude_{0},
        lastReportedXCoord_{0},
        lastReportedYCoord_{0},
        lastComputedPositionTime_{std::chrono::system_clock::now()},
        lastComputedSpeed_{0, 0, 0},
        lastComputedPosition_{0, 0, 0},
        previousComputedPosition_{0, 0, 0},
        registeredEvents_{},
        worldRef_{std::make_shared<const WorldReference>(LatLon{
                ConfSingleton::instance().get("origin.lat", 0.0),
                ConfSingleton::instance().get("origin.lon", 0.0)
        })},
        elevationModel_{nullptr},
        trackedFlyables_{} {

    initRenderSettings(view);
    initCamera(view);

    trackedFlyables_.insert(std::make_pair<FlyableType, Tracker *>(
            FlyableType::ROCKET,
            new Tracker(launchSitePos_, camera_, TextureConstants::CARET_DOWN, QStringLiteral("ROCKET"),
                        TextType::BOLD, this, OpenGLConstants::ABOVE_MARKER_OFFSET,
                        OpenGLConstants::ABOVE_CENTER_LABEL)
    ));

    trackedFlyables_.insert(std::make_pair<FlyableType, Tracker *>(
            FlyableType::PAYLOAD,
            new Tracker(launchSitePos_, camera_, TextureConstants::CARET_DOWN, QStringLiteral("PAYLOAD"),
                        TextType::LEGEND, this, OpenGLConstants::ABOVE_MARKER_OFFSET,
                        OpenGLConstants::ABOVE_RIGHT_LABEL)
    ));
}

void
RootEntity::init() {

    // Display 3D module origin
    new OpenGL3DAxes(this);

    // Build digital representation of launch site
    std::unique_ptr<const IDiscreteElevationModel> compositeModel = CompositeElevationModel::buildModel(
            ConfSingleton::instance().getList<std::string>("dems"));
    elevationModel_ = std::make_shared<const ContinuousElevationModel>(std::move(compositeModel), worldRef_);

    // Read and build positions from config file
    LatLon gsLatLon = {
            ConfSingleton::instance().get("gs.lat", 0.0),
            ConfSingleton::instance().get("gs.lon", 0.0)
    };

    LatLon launchSiteLatLon = {
            ConfSingleton::instance().get("launchsite.lat", 0.0),
            ConfSingleton::instance().get("launchsite.lon", 0.0)
    };

    launchSitePos_ = worldRef_->worldPosAt(launchSiteLatLon, elevationModel_);
    ground_ = new Ground(
            this,
            elevationModel_,
            worldRef_,
            2,
            QString::fromStdString(ConfSingleton::instance().get("commonTexturePath", std::string{""})));

    rocketTrace_ = new Line(this, QColor::fromRgb(255, 255, 255), false);
    rocketTrace_->setData({launchSitePos_, launchSitePos_});

    payloadTrace_ = new Line(this, QColor::fromRgb(255, 159, 0), false);
    payloadTrace_->setData({launchSitePos_, launchSitePos_});

    auto *gs = new GroundStation(worldRef_->worldPosAt(gsLatLon, elevationModel_), TextureConstants::DOUBLE_DOWN_ARROW,
                                 camera_, this);

    // Initialise simulated rocket trace
    /*
    simTrace_ = new Line(this, QColor::fromRgb(0, 180, 0), true);
    std::string tracePath{"../../ground_station/data/simulated_trajectory.csv"};
    FileReader<QVector3D> traceReader{tracePath, posFromString};
    QVector<QVector3D> traceData = QVector<QVector3D>::fromStdVector(traceReader.readFile());
    addToEach(traceData, launchSitePos_);
    simTrace_->appendData(traceData);
    */

    std::string meteoPath = ConfSingleton::instance().get("meteoFile", std::string{""});
    splashDownPredictor_ = new SplashDownPredictor(meteoPath, this);
    rocketRuler_ = new Ruler(groundAltitude_, QVector3D{0, 0, 0}, camera_, TextureConstants::CARET_LEFT, this);

    // Setup dynamic camera parameters
    cameraController_->setCameraViewCenter(launchSitePos_);
    cameraController_->registerObservable(trackedFlyables_[FlyableType::ROCKET]);
    cameraController_->registerObservable(trackedFlyables_[FlyableType::PAYLOAD]);
    cameraController_->registerObservable(gs);

    reportWindData();
}

void
RootEntity::reportWindData() {

    windData_ = splashDownPredictor_->windData();

    for (float i = UI3DConstants::WIND_REPORT_MAX; i > 0; i -= UI3DConstants::WIND_REPORT_INTERVAL) {
        std::stringstream ss;
        std::pair<float, float> speedAndAngle = windData_->speedAndAngleForAltitude(i);
        ss << std::setw(7) << std::setfill(' ') << i << "m"
           << std::setw(7) << std::setfill(' ') << std::setprecision(2) << std::fixed << speedAndAngle.first << "m/s"
           << std::setw(8) << std::setfill(' ') << std::setprecision(2) << std::fixed << speedAndAngle.second << "°\n";
        emit addInfoString(QString::fromStdString(ss.str()));
    }

}

void
RootEntity::updateFlightPosition(const Position pos) {

    QVector2D horizontalWorldPos = worldRef_->translationFromOrigin(pos.latLon);
    previousComputedPosition_ = lastComputedPosition_;
    lastComputedPosition_ = {horizontalWorldPos.x(), static_cast<float>(pos.altitude), horizontalWorldPos.y()};

    double elapsedTime = msecsBetween(lastComputedPositionTime_, std::chrono::system_clock::now()) / 1000.0;

    if (elapsedTime > 0 && previousComputedPosition_.length() > 0) {
        lastComputedSpeed_ = (lastComputedPosition_ - previousComputedPosition_) / elapsedTime;

        trackedFlyables_[FlyableType::ROCKET]->updatePosition(lastComputedPosition_);
        rocketRuler_->updatePosition(lastComputedPosition_);
        rocketTrace_->appendData(lastComputedPosition_);

        splashDownPredictor_->updatePos(lastComputedPosition_);
        splashDownPredictor_->updateSpeed(lastComputedSpeed_);
        splashDownPredictor_->recomputePrediction();


        int windDataEntryIndex = UI3DConstants::WIND_REPORT_N_LINES -
                                 static_cast<int>(
                                         (lastComputedPosition_.y() - groundAltitude_) / UI3DConstants::WIND_REPORT_INTERVAL
                                 );

        windDataEntryIndex = clamp(windDataEntryIndex, 0, UI3DConstants::WIND_REPORT_N_LINES);

        emit updateHighlightInfoString(windDataEntryIndex);
    }
}

void
RootEntity::updatePayloadPosition(const Position pos) {

    QVector2D horizontalWorldPos = worldRef_->translationFromOrigin(pos.latLon);
    QVector3D pos3D = {horizontalWorldPos.x(), static_cast<float>(pos.altitude), horizontalWorldPos.y()};

    trackedFlyables_[FlyableType::PAYLOAD]->updatePosition(pos3D);
    payloadTrace_->appendData(pos3D);
}


void
RootEntity::updateTrackerLatLon(FlyableType flyableType, const LatLon &latLon) {

    Tracker *t{trackedFlyables_[flyableType]};
    QVector2D horPos{worldRef_->translationFromOrigin(latLon)};
    QVector3D tPos{t->getPosition()};

    t->updatePosition(QVector3D{horPos.x(), tPos.y(), horPos.y()});
}

void
RootEntity::updateTrackerAltitude(FlyableType flyableType, const float &alt) {

    Tracker *t{trackedFlyables_[flyableType]};
    QVector3D tPos{t->getPosition()};

    t->updatePosition(QVector3D{tPos.x(), alt, tPos.z()});
}

void
RootEntity::resetTrace() {
    rocketTrace_->clearData();
}


//TODO: delete ? -> when replay is available to 3d module
void
RootEntity::updateRocketTracker(QVector<QVector3D> &positions, const QVector3D &speed) {

    static QVector3D accumulatedBias{0, 0, 0};

    QVector2D bias = (*windData_)[positions.last().y()];
    accumulatedBias += RocketConstants::SIMULATION_DT * QVector3D{bias.x(), 0, bias.y()};

    addToEach(positions, launchSitePos_ + accumulatedBias);
    QVector3D lastPos = positions.last();

    trackedFlyables_[FlyableType::ROCKET]->updatePosition(lastPos);
    rocketRuler_->updatePosition(lastPos);
    rocketTrace_->appendData(positions);

    splashDownPredictor_->updatePos(lastPos);
    splashDownPredictor_->updateSpeed(speed);
    splashDownPredictor_->recomputePrediction();

    //touchdownCrosshair_->updatePosition(splashDownPredictor_->getTouchdownCoordinates(ground_));

    int windDataEntryIndex = UI3DConstants::WIND_REPORT_N_LINES -
                             static_cast<int>(
                                     (lastPos.y() - groundAltitude_) / UI3DConstants::WIND_REPORT_INTERVAL
                             );

    windDataEntryIndex = clamp(windDataEntryIndex, 0, UI3DConstants::WIND_REPORT_N_LINES);

    emit updateHighlightInfoString(windDataEntryIndex);
}

void
RootEntity::registerEvent(const EventPacket &event) {

    //TODO: invalid event are already checked for in decoding function
    if (RocketEventConstants::EVENT_CODES.find(event.code_) == RocketEventConstants::EVENT_CODES.end()) {
        std::cout << "This event code is invalid: " << event.code_ << std::endl;
        return;
    }

    registeredEvents_.emplace_back(std::make_pair(event.timestamp_, new Tracker(trackedFlyables_[FlyableType::ROCKET]->getPosition(), camera_,
                                                                                TextureConstants::DOWNWARD_DIAGONAL,
                                                                                QString::fromStdString(
                                                                                        RocketEventConstants::EVENT_CODES.at(
                                                                                                event.code_)),
                                                                                TextType::LEGEND, this,
                                                                                OpenGLConstants::LEFT_LEGEND_ICON_OFFSET,
                                                                                OpenGLConstants::LEFT_LEGEND_TEXT_OFFSET)));
}


void
RootEntity::initRenderSettings(Qt3DExtras::Qt3DWindow *view) {
    auto *renderSettings = new Qt3DRender::QRenderSettings();
    auto *forwardRenderer = new ForwardRenderer(view, renderSettings);
    renderSettings->setActiveFrameGraph(forwardRenderer);
    this->addComponent(renderSettings);
    this->addComponent(LayerManager::getInstance().getLayer(LayerType::VISIBLE));
}

void
RootEntity::initCamera(Qt3DExtras::Qt3DWindow *view) {
    cameraController_->setCamera(camera_);
    cameraController_->setLinearSpeed(CameraConstants::LINEAR_SPEED);
    cameraController_->setLookSpeed(CameraConstants::LOOK_SPEED);

    camera_->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
    camera_->setFieldOfView(45.0);
    camera_->setNearPlane(0.1);
    camera_->setFarPlane(100000.0);
    camera_->setPosition(launchSitePos_ + QVector3D{-5000.0, 2000.0, 3000.0});
    camera_->setUpVector(QVector3D{0.0, 1.0, 0.0});

    auto *compass = new Compass(this, camera_);
    connect(camera_, &Qt3DRender::QCamera::viewMatrixChanged, compass, &Compass::update);
    connect(view, &Qt3DExtras::Qt3DWindow::widthChanged, compass, &Compass::updateHorizontalOffset);
    connect(view, &Qt3DExtras::Qt3DWindow::heightChanged, compass, &Compass::updateVerticalOffset);

    connect(cameraController_, &CameraController::resetPressed, this, &RootEntity::resetTrace);
}