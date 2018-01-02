
#ifndef GS_MATTERHORN_MARKER_H
#define GS_MATTERHORN_MARKER_H

#include <Qt3DRender/QTexture>
#include <Qt3DCore/QNode>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>

class Marker : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Marker(const QString &texture, float width, float height, QVector3D offset,
                    Qt3DRender::QCamera *camera,
                    Qt3DCore::QNode *parent);

public slots:

    void updateTransform();

private:
    Qt3DCore::QTransform *transform_;
    Qt3DRender::QCamera *camera_;
    QVector3D offset_;
    static const QVector3D basePosition_;
};


#endif //GS_MATTERHORN_MARKER_H
