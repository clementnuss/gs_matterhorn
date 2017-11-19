
#ifndef GS_MATTERHORN_MARKER_H
#define GS_MATTERHORN_MARKER_H

#include <QObject>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <Qt3DCore/QNode>

class Marker : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Marker(QUrl textureUrl, QVector3D position, Qt3DRender::QCamera *camera, Qt3DCore::QNode *parent);

public slots:

    void updateTransform();

    void updatePosition(const QVector3D &newPosition);

private:
    Qt3DCore::QTransform *transform_;
    Qt3DRender::QCamera *camera_;
    QVector3D position_;
};


#endif //GS_MATTERHORN_MARKER_H
