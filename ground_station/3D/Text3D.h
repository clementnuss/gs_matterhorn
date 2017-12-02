
#ifndef GS_MATTERHORN_TEXT3D_H
#define GS_MATTERHORN_TEXT3D_H


#include <Qt3DCore/QEntity>

class Text3D : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Text3D(QString text, QVector3D scale, Qt3DRender::QCamera *camera, Qt3DCore::QNode *parent);

public slots:

    void updateTransform();

private:
    Qt3DRender::QCamera *camera_;
    Qt3DCore::QTransform *transform_;
    QVector3D offset_;
    QVector3D scale_;
    static const QVector3D basePosition_;
};

#endif //GS_MATTERHORN_TEXT3D_H
