
#ifndef GS_MATTERHORN_MARKER_H
#define GS_MATTERHORN_MARKER_H

#include <Qt3DRender/QTexture>
#include <Qt3DCore/QNode>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>

class Marker : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Marker(const QString &texture, const float &width, const float &height, const QVector3D &offset,
                    const Qt3DRender::QCamera *const camera,
                    Qt3DCore::QNode *const parent);

public slots:

    void updateTransform();

private:
    Qt3DCore::QTransform *transform_;
    const Qt3DRender::QCamera *const camera_;
    QVector3D offset_;
    static const QVector3D basePosition_;
};


#endif //GS_MATTERHORN_MARKER_H
