
#ifndef GS_MATTERHORN_GRIDGEOMETRY_H
#define GS_MATTERHORN_GRIDGEOMETRY_H


#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometry>
#include <3D/Scene/WorldReference.h>
#include "DiscreteElevationModel.h"
#include "ContinuousElevationModel.h"


class GridGeometry : public Qt3DRender::QGeometry {
Q_OBJECT

public:
    explicit GridGeometry(Qt3DCore::QNode *parent,
                          const std::function<float(int, int)> &heightSampler,
                          const std::function<QVector3D(int, int)> &normalSampler,
                          int sideLength,
                          int resolution);

    float vertexHeightAt(int i, int j) const;

    void resampleVertices(const std::function<float(int, int)> &heightSampler,
                          const std::function<QVector3D(int, int)> &normalSampler);

private:

    QByteArray createPlaneVertexData(const std::function<float(int, int)> &heightSampler,
                                     const std::function<QVector3D(int, int)> &normalSampler);

    QByteArray createPlaneIndexData();

    int gridResolution_;
    int sideLength_;

    Qt3DRender::QAttribute *positionAttribute_;
    Qt3DRender::QAttribute *normalAttribute_;
    Qt3DRender::QAttribute *texCoordAttribute_;
    Qt3DRender::QAttribute *tangentAttribute_;
    Qt3DRender::QAttribute *indexAttribute_;
    Qt3DRender::QBuffer *vertexBuffer_;
    Qt3DRender::QBuffer *indexBuffer_;
};


#endif //GS_MATTERHORN_GRIDGEOMETRY_H
