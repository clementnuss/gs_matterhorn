
#ifndef GS_MATTERHORN_GRIDMESH_H
#define GS_MATTERHORN_GRIDMESH_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/qgeometryrenderer.h>
#include <QtCore/QSize>
#include "ContinuousElevationModel.h"


class GridMesh : public Qt3DRender::QGeometryRenderer {
Q_OBJECT

public:
    explicit GridMesh(Qt3DCore::QNode *parent, const ContinuousElevationModel *const model,
                      const LatLon &topLeftGeoPoint, int sideLength, int resolution);

};


#endif //GS_MATTERHORN_GRIDMESH_H
