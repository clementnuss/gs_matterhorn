
#ifndef GS_MATTERHORN_ROOTENTITY_H
#define GS_MATTERHORN_ROOTENTITY_H

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DCore/QNode>
#include <3D/Line/Line.h>
#include <3D/CameraController.h>
#include "3D/Billboards/Tracker.h"
#include "3D/Objects/Ruler.h"

class RootEntity : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit RootEntity(Qt3DExtras::Qt3DWindow *view, Qt3DCore::QNode *parent = nullptr);

    void updateRocketTracker(const QVector<QVector3D> &positions);

private:
    CameraController *cameraController_;
    Qt3DRender::QCamera *camera_;
    Tracker *rocketTracker_;
    Line *rocketTrace_;
    Line *simTrace_;
    Ruler *rocketRuler_;
};

#endif //GS_MATTERHORN_ROOTENTITY_H
