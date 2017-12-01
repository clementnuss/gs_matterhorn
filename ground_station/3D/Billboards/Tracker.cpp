#include <QtGui/QFont>
#include <3D/Utils.h>
#include <3D/Billboards/Marker.h>
#include "Tracker.h"

const QVector3D Tracker::textOffset_{1.5, 0, 0};
const QVector3D Tracker::markerOffset_{0, 1, 0};

Tracker::Tracker(QVector3D position, Qt3DRender::QCamera *camera,
                 QUrl textureUrl, QUrl textUrl, Qt3DCore::QNode *parent)
        : Qt3DCore::QEntity(parent),
          transform_{new Qt3DCore::QTransform()},
          marker_{new Marker(textureUrl, 2, 2, markerOffset_, camera, this)},
          text_{new Marker(textUrl, 20, 3, textOffset_ + QVector3D(10, 1.5, 0), camera,
                           this)} {
    updatePosition(position);
    this->addComponent(transform_);
}

void Tracker::updatePosition(QVector3D newPosition) {
    QMatrix4x4 m{};
    m.translate(newPosition);
    m.scale(100, 100, 100);
    transform_->setMatrix(m);
}