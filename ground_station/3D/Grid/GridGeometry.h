
#ifndef GS_MATTERHORN_GRIDGEOMETRY_H
#define GS_MATTERHORN_GRIDGEOMETRY_H


#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometry>
#include "DiscreteElevationModel.h"
#include "ContinuousElevationModel.h"


class GridGeometry : public Qt3DRender::QGeometry {
Q_OBJECT

public:
    explicit GridGeometry(Qt3DCore::QNode *parent, const ContinuousElevationModel *const model,
                          const LatLon &topLeftGeoPoint,
                          int sideLength, int resolution);

private:

    QByteArray createPlaneVertexData();

    QByteArray createPlaneIndexData();

    int gridResolution_;
    int sideLength_;
    double arcWestEastDistance_;
    LatLon topLeftLatLon_;
    const ContinuousElevationModel *const model_;

    Qt3DRender::QAttribute *positionAttribute_;
    Qt3DRender::QAttribute *normalAttribute_;
    Qt3DRender::QAttribute *texCoordAttribute_;
    Qt3DRender::QAttribute *tangentAttribute_;
    Qt3DRender::QAttribute *indexAttribute_;
    Qt3DRender::QBuffer *vertexBuffer_;
    Qt3DRender::QBuffer *indexBuffer_;
};


#endif //GS_MATTERHORN_GRIDGEOMETRY_H
