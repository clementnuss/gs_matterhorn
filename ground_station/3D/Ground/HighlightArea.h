
#ifndef GS_MATTERHORN_HIGHTLIGHTAREA_H
#define GS_MATTERHORN_HIGHTLIGHTAREA_H


#include <QtGui/QVector2D>
#include <Qt3DCore/QNode>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QParameter>
#include <Qt3DCore/QTransform>
#include <3D/Grid/GridMesh.h>

class HighlightArea : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit HighlightArea(Qt3DRender::QParameter *heightParameter, Qt3DCore::QNode *parent);

    void updatePos(const QVector2D &pos);

public slots:

private:
    Qt3DCore::QTransform *transform_;
    GridMesh *mesh_;
};


#endif //GS_MATTERHORN_HIGHTLIGHTAREA_H
