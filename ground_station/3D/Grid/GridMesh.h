
#ifndef GS_MATTERHORN_GRIDMESH_H
#define GS_MATTERHORN_GRIDMESH_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DCore/QTransform>
#include "3D/Scene/WorldReference.h"
#include <Qt3DRender/qgeometryrenderer.h>
#include <QtCore/QSize>
#include "ContinuousElevationModel.h"
#include "GridGeometry.h"


class GridMesh : public Qt3DRender::QGeometryRenderer {
Q_OBJECT

public:
    explicit GridMesh(Qt3DCore::QNode *parent, const std::function<float(int, int)> &heightSampler,
                      const std::function<QVector3D(int, int)> &normalSampler, int sideLength, int resolution);

    float vertexHeightAt(int i, int j) const;

private :
    GridGeometry *geometry_;
};


#endif //GS_MATTERHORN_GRIDMESH_H
