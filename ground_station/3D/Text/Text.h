
#ifndef GS_MATTERHORN_TEXT_H
#define GS_MATTERHORN_TEXT_H

#include <QObject>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <Qt3DCore/QNode>

class Text : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Text(QString text, QVector3D position, Qt3DRender::QCamera *camera, Qt3DCore::QNode *parent);

public slots:

    void updateTransform();

private:
    Qt3DCore::QTransform *transform_;
    Qt3DRender::QCamera *camera_;
    QVector3D position_;
    QFont font_;
};

#endif //GS_MATTERHORN_TEXT_H
