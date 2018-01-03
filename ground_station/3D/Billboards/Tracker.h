
#ifndef GS_MATTERHORN_TRACKER_H
#define GS_MATTERHORN_TRACKER_H

#include <Qt3DRender/QTexture>
#include <3D/Billboards/Marker.h>
#include <3D/Billboards/Text3D.h>
#include <Qt3DRender/QCamera>


class Tracker : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Tracker(QVector3D position, Qt3DRender::QCamera *camera, const QString &texture, QString text,
                     Qt3DCore::QNode *parent, const QVector3D &markerOffset, const QVector3D &textOffset);

public slots:

    void updatePosition(QVector3D pos);

    QVector3D getPosition();

    Qt3DCore::QTransform *getTransform();

private:
    Marker *marker_;
    Text3D *text_;

    Qt3DCore::QTransform *transform_;
    QVector3D textOffset_;
    QVector3D markerOffset_;
};


#endif //GS_MATTERHORN_TRACKER_H
