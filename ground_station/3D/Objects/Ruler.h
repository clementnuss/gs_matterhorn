
#ifndef GS_MATTERHORN_RULER_H
#define GS_MATTERHORN_RULER_H


#include <Qt3DCore/QEntity>
#include <3D/Line/Line.h>
#include <3D/Billboards/Text3D.h>

class Ruler : public Qt3DCore::QEntity {
Q_OBJECT

public:
    Ruler(QVector3D &measurePos, Qt3DRender::QCamera *camera, Qt3DRender::QTexture2D *tickTexture,
          Qt3DCore::QNode *parent);

    void updatePosition(const QVector3D &newPos);

private:
    void redraw();

    void initLabels(Qt3DRender::QCamera *camera, Qt3DRender::QTexture2D *tickTexture);

    Qt3DCore::QTransform *transform_;
    QVector3D measurePos_;
    Line *rulerAxis_;
    QVector<QVector3D> ticksVector_;
    QVector<Tracker *> tickLabels_;
};


#endif //GS_MATTERHORN_RULER_H