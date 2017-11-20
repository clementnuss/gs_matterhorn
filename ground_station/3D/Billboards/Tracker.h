
#ifndef GS_MATTERHORN_TRACKER_H
#define GS_MATTERHORN_TRACKER_H


#include <3D/Billboards/Marker.h>
#include <3D/Billboards/Text.h>


class Tracker : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Tracker(QVector3D position, Qt3DRender::QCamera *camera,
                     QUrl textureUrl, QString caption, Qt3DCore::QNode *parent);

public slots:

    void updatePosition(QVector3D pos);

private:
    Marker *marker_;
    Text *text_;

    Qt3DCore::QTransform *transform_;
    static const QVector3D textOffset_;
    static const QVector3D markerOffset_;
};


#endif //GS_MATTERHORN_TRACKER_H
