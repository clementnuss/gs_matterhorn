#include <QtGui/QFont>
#include <3D/Utils.h>
#include <3D/Billboards/Marker.h>
#include <Qt3DCore/QTransform>
#include "Tracker.h"

Tracker::Tracker(const QVector3D &position, const Qt3DRender::QCamera *const camera, const QString &iconTextureName, const QString &text,
                 TextType textType, Qt3DCore::QNode *const parent, const QVector3D &markerOffset, const QVector3D &textOffset)
        : Qt3DCore::QEntity(parent),
          transform_{new Qt3DCore::QTransform()},
          markerOffset_{markerOffset},
          textOffset_{textOffset},
          marker_{nullptr},
          text_{nullptr} {

    this->addComponent(transform_);

    marker_ = new Marker(iconTextureName, 2, 2, markerOffset_, camera, this);
    text_ = new Text3D(std::move(text), textType, camera, textOffset_, this);

    updatePosition(position);
}

void
Tracker::updatePosition(QVector3D newPosition) {
    QMatrix4x4 m{};
    m.translate(newPosition);
    m.scale(UI3DConstants::TRACKER_SIZE, UI3DConstants::TRACKER_SIZE, UI3DConstants::TRACKER_SIZE);
    transform_->setMatrix(m);
}

QVector3D
Tracker::getPosition() {
    return transform_->translation();
}

Qt3DCore::QTransform *
Tracker::getObjectTransform() const {
    return transform_;
}