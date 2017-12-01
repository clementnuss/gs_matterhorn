
#ifndef GS_MATTERHORN_TEXT_H
#define GS_MATTERHORN_TEXT_H

#include <QObject>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <Qt3DCore/QNode>

class Text : public Qt3DExtras::QText2DEntity {
Q_OBJECT
public:
    explicit Text(QString text, QVector3D offset, Qt3DRender::QCamera *camera, Qt3DCore::QNode *parent);

public slots:

    void updateTransform();

private:
    Qt3DCore::QTransform *transform_;
    Qt3DRender::QCamera *camera_;
    QVector3D offset_;
    QFont font_;
    static const QVector3D basePosition_;
};

#endif //GS_MATTERHORN_TEXT_H
