#ifndef GS_MATTERHORN_ROOTENTITY_H
#define GS_MATTERHORN_ROOTENTITY_H

#include <Qt3DExtras/Qt3DWindow>

#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DCore/QNode>
#include <3D/Line/Line.h>
#include <3D/CameraController.h>
#include <3D/Billboards/Tracker.h>
#include <3D/Objects/Ruler.h>
#include <DataStructures/datastructs.h>
#include <3D/Objects/SplashDownPredictor.h>
#include <3D/Objects/TouchdownCrosshair.h>

class RootEntity : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit RootEntity(Qt3DExtras::Qt3DWindow *view, Qt3DCore::QNode *parent = nullptr);

    void init();

    void updateRocketTracker(QVector<QVector3D> &positions, const QVector3D &speed);

    void registerEvent(const EventPacket &eventPacket);

public slots:

    void updateFlightPosition(const Position pos);

    void updatePayloadPosition(const Position pos);

    void resetTrace();

signals:

    void addInfoString(const QString &);

    void updateHighlightInfoString(int);

private:

    void initRenderSettings(Qt3DExtras::Qt3DWindow *view);

    void initCamera(Qt3DExtras::Qt3DWindow *view);

    void reportWindData();

    std::shared_ptr<const WorldReference> worldRef_;
    std::shared_ptr<const ContinuousElevationModel> elevationModel_;

    float groundAltitude_;
    float lastReportedAltitude_;
    float lastReportedXCoord_;
    float lastReportedYCoord_;
    QVector3D lastComputedSpeed_;
    QVector3D lastComputedPosition_;
    QVector3D previousComputedPosition_;
    std::chrono::system_clock::time_point lastComputedPositionTime_;

    Ground *ground_;
    const WindData *windData_;
    SplashDownPredictor *splashDownPredictor_;
    CameraController *cameraController_;
    Qt3DRender::QCamera *camera_;
    QVector3D launchSitePos_;
    Tracker *rocketTracker_;
    Tracker *payloadTracker_;
    Line *rocketTrace_;
    Line *payloadTrace_;
    Line *simTrace_;
    Ruler *rocketRuler_;

    std::vector<std::pair<uint32_t, Tracker *>> registeredEvents_;
};

#endif //GS_MATTERHORN_ROOTENTITY_H


