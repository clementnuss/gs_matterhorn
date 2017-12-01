#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <3D/Utils.h>
#include <3D/Billboards/Marker.h>
#include <3D/Billboards/Tracker.h>
#include "GroundStation.h"

GroundStation::GroundStation(QVector3D position, Qt3DRender::QCamera *camera, Qt3DCore::QNode *parent)
        : Qt3DCore::QEntity(parent),
          position_{position},
          transform_{new Qt3DCore::QTransform()} {

    QMatrix4x4 m{};
    m.translate(position_);
    transform_->setMatrix(m);

    this->addComponent(transform_);

    new Tracker(QVector3D{0, 1, 0}, camera,
                QUrl(QStringLiteral("qrc:/3D/textures/double_down_arrow.png")),
                QUrl(QStringLiteral("qrc:/3D/textures/text/ground_station.png")),
                this);
}

