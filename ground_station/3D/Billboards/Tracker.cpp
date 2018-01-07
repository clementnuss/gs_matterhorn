#include <QtGui/QFont>
#include <3D/Utils.h>
#include <3D/Billboards/Marker.h>
#include <Qt3DCore/QTransform>
#include "Tracker.h"

Tracker::Tracker(QVector3D position,
                 Qt3DRender::QCamera *camera,
                 const QString &texture,
                 QString text,
                 Qt3DCore::QNode *parent,
                 const QVector3D &markerOffset,
                 const QVector3D &textOffset)
        : Qt3DCore::QEntity(parent),
          transform_{new Qt3DCore::QTransform()},
          markerOffset_{markerOffset},
          textOffset_{textOffset},
          marker_{nullptr},
          text_{nullptr} {

    this->addComponent(transform_);

    marker_ = new Marker(texture, 2, 2, markerOffset_, camera, this);
    text_ = new Text3D(text, camera, textOffset_, this);

    updatePosition(position);
}

void Tracker::updatePosition(QVector3D newPosition) {
    QMatrix4x4 m{};
    m.translate(newPosition);
    m.scale(100, 100, 100);
    transform_->setMatrix(m);
}

QVector3D Tracker::getPosition() {
    return transform_->translation();
}

Qt3DCore::QTransform *Tracker::getTransform() {
    return transform_;
}