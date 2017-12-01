#include <QtGui/QFont>
#include <3D/Utils.h>
#include <3D/Billboards/Marker.h>
#include "Tracker.h"

const QVector3D Tracker::textOffset_{1.5, 0, 0};
const QVector3D Tracker::markerOffset_{0, 1, 0};

Tracker::Tracker(QVector3D position, Qt3DRender::QCamera *camera,
                 QUrl textureUrl, QUrl textUrl, QString text, Qt3DCore::QNode *parent)
        : Qt3DCore::QEntity(parent),
          transform_{new Qt3DCore::QTransform()},
          marker_{new Marker(textureUrl, 2, 2, markerOffset_, camera, this)},
          text_{nullptr} {


    auto textMetrics = QFontMetricsF(QFont{"Input Bold Condensed"});
    QRectF rect = textMetrics.boundingRect(text);
    qreal margin = textMetrics.width('_');
    rect.adjust(-margin, margin, margin, margin);

    float width = static_cast<float>(rect.width()) / 10.0f;
    float height = static_cast<float>(rect.height()) / 10.0f;

    std::cout << width << std::endl;
    std::cout << height << std::endl;

    text_ = new Marker(textUrl, width, height, textOffset_ + QVector3D(width / 2.0, height / 2.0, 0), camera,
                       this);

    updatePosition(position);
    this->addComponent(transform_);
}

void Tracker::updatePosition(QVector3D newPosition) {
    QMatrix4x4 m{};
    m.translate(newPosition);
    m.scale(100, 100, 100);
    transform_->setMatrix(m);
}