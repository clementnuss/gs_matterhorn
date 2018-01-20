
#include <3D/ForwardRenderer/LayerManager.h>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <3D/TextureManagerSingleton.h>
#include "Compass.h"


Compass::Compass(Qt3DCore::QNode *parent, Qt3DRender::QCamera *cam) :
        QEntity(parent),
        camera_{cam},
        mesh_{new Qt3DRender::QMesh()},
        material_{new Qt3DExtras::QTextureMaterial()},
        transform_{new Qt3DCore::QTransform()} {

    transform_->setScale(0.3);
    material_->setTexture(TextureManagerSingleton::getInstance().getTexture("qrc:3D/textures/compass_colors.png"));

    mesh_->setSource(QUrl{"qrc:3D/models/compass.obj"});

    this->addComponent(mesh_);
    this->addComponent(material_);
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
    pos += camera_->viewVector().normalized() * 10;
    pos += 4.5 * rightVector;
    pos += 3.0 * upVector;
    transform_->setTranslation(pos);

}