
#ifndef GS_MATTERHORN_ROOTENTITY_H
#define GS_MATTERHORN_ROOTENTITY_H


#include <QObject>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <Qt3DCore/QNode>
#include <3D/Line/Line.h>
#include "3D/Billboards/Tracker.h"

class RootEntity : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit RootEntity(Qt3DExtras::Qt3DWindow *view, Qt3DCore::QNode *parent = 0);

    void updateRocketTracker(const QVector<QVector3D> &positions);

private:
    Qt3DExtras::QFirstPersonCameraController *cameraController_;
    Tracker *rocketTracker_;
    Line *rocketTrace_;
};

#endif //GS_MATTERHORN_ROOTENTITY_H
