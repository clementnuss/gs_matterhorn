
#include <3D/Ground/Ground.h>
#include "RootEntity.h"

RootEntity::RootEntity(Qt3DExtras::Qt3DWindow *view, Qt3DCore::QNode *parent) :
        Qt3DCore::QEntity(parent),
        cameraController_{new Qt3DExtras::QFirstPersonCameraController(this)} {

    Qt3DRender::QCamera *camera = view->camera();
    camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
    camera->setFieldOfView(45.0);
    camera->setNearPlane(0.1);
    camera->setFarPlane(1000.0);
    camera->setPosition(QVector3D{-40.0, 20.0, -40.0});
    camera->setUpVector(QVector3D{0.0, 1.0, 0.0});
    camera->setViewCenter(QVector3D{0.0, 0.0, 0.0});

    cameraController_->setCamera(camera);


    new Ground(this);
}