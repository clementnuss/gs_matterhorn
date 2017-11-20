#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <3D/Utils.h>
#include <3D/Marker/Marker.h>
#include <3D/Text/Text.h>
#include "GroundStation.h"

GroundStation::GroundStation(QVector3D position, Qt3DRender::QCamera *camera, Qt3DCore::QNode *parent)
        : Qt3DCore::QEntity(parent),
          position_{position},
          transform_{new Qt3DCore::QTransform(this)} {


    auto *mesh = new Qt3DExtras::QCuboidMesh(this);
    auto *material = new Qt3DExtras::QPhongMaterial(this);

    QMatrix4x4 m{};
    m.translate(position_);
    transform_->setMatrix(m);

    this->addComponent(mesh);
    this->addComponent(material);
    this->addComponent(transform_);

    new Marker(QUrl(QStringLiteral("qrc:/3D/textures/double_down_arrow.png")), markerOffset_, camera, this);
    new Text(QStringLiteral("GROUND STATION"), textOffset_, camera, this);
}

