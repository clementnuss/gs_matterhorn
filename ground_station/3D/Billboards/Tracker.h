
#ifndef GS_MATTERHORN_TRACKER_H
#define GS_MATTERHORN_TRACKER_H


#include <3D/Billboards/Marker.h>


class Tracker : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Tracker(QVector3D position, Qt3DRender::QCamera *camera,
                     QUrl textureUrl, QUrl textUrl, QString text, Qt3DCore::QNode *parent);

public slots:

    void updatePosition(QVector3D pos);

private:
    Marker *marker_;
    Marker *text_;

    Qt3DCore::QTransform *transform_;
    static const QVector3D textOffset_;
    static const QVector3D markerOffset_;
};


#endif //GS_MATTERHORN_TRACKER_H
