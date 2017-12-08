#include <QtGui/QFont>
#include <3D/Utils.h>
#include <3D/Billboards/Marker.h>
#include "Tracker.h"

Tracker::Tracker(QVector3D position,
                 Qt3DRender::QCamera *camera,
                 Qt3DRender::QTexture2D *texture,
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


    auto textMetrics = QFontMetricsF(QFont{"Input Bold Condensed"});
    QRectF rect = textMetrics.boundingRect(text);
    qreal margin = textMetrics.width('_');
    rect.adjust(-margin, margin, margin, margin);

    float width = static_cast<float>(rect.width()) / 10.0f;
    float height = static_cast<float>(rect.height()) / 10.0f;

    std::cout << width << std::endl;
    std::cout << height << std::endl;
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