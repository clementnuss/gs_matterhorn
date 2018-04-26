
#include <3D/3DVisualisationConstants.h>
#include "TouchdownCrosshair.h"

TouchdownCrosshair::TouchdownCrosshair(const QVector3D &position, Qt3DRender::QCamera *camera, Qt3DCore::QNode *parent)
        : Qt3DCore::QEntity(parent),
          transform_{new Qt3DCore::QTransform()},
          marker_{nullptr},
          text_{nullptr} {

    this->addComponent(transform_);

    marker_ = new FlatBillboard(TextureConstants::CROSSHAIR, 2, 2, this);
    text_ = new Text3D(QStringLiteral("Touchdown location"), TextType::BOLD, camera, {0, 4, 0}, this);

    updatePosition(position);
}

void
TouchdownCrosshair::updatePosition(QVector3D newPosition) {
    QMatrix4x4 m{};
    m.translate(newPosition + QVector3D{0, 10, 0});
    m.scale(UI3DConstants::TRACKER_SIZE, UI3DConstants::TRACKER_SIZE, UI3DConstants::TRACKER_SIZE);

    transform_->setMatrix(m);
}

QVector3D
TouchdownCrosshair::getPosition() {
    return transform_->translation();
}

Qt3DCore::QTransform *
TouchdownCrosshair::getTransform() {
    return transform_;
}