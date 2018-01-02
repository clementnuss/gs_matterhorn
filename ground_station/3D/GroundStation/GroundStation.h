
#ifndef GS_MATTERHORN_GROUNDSTATION_H
#define GS_MATTERHORN_GROUNDSTATION_H

#include <Qt3DRender/QTexture>
#include <Qt3DRender/QCamera>

class GroundStation : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit GroundStation(QVector3D position, const QString &texture, Qt3DRender::QCamera *camera,
                           Qt3DCore::QNode *parent);

public slots:

private:
    Qt3DCore::QTransform *transform_;
    const QVector3D textOffset_{1, 1, 0};
    const QVector3D markerOffset_{0, 2, 0};
    QVector3D position_;
};


#endif //GS_MATTERHORN_GROUNDSTATION_H
