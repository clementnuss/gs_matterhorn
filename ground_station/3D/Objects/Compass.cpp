
#include <3D/ForwardRenderer/LayerManager.h>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPhongMaterial>
#include "Compass.h"


Compass::Compass(Qt3DCore::QNode *parent, Qt3DRender::QCamera *cam) :
        QEntity(parent),
        camera_{cam},
        transform_{new Qt3DCore::QTransform()} {

    transform_->setScale(0.1);

    auto *mesh = new Qt3DExtras::QCuboidMesh();
    auto *material = new Qt3DExtras::QPhongMaterial();


    this->addComponent(mesh);
    this->addComponent(material);
    this->addComponent(transform_);
}


void Compass::update() {

    QVector3D rightVector = QVector3D::crossProduct(
            camera_->viewVector().normalized(),
            camera_->upVector().normalized()
    ).normalized();

    QVector3D upVector = QVector3D::crossProduct(
            rightVector.normalized(),
            camera_->viewVector().normalized()
    ).normalized();

    // TODO adapt right and up offset to screen size
    QVector3D pos = camera_->position();
    pos += camera_->viewVector().normalized() * 1;
    pos += 0.4 * rightVector;
    pos += 0.3 * upVector;
    transform_->setTranslation(pos);

}