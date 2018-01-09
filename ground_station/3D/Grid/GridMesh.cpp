
#include "GridMesh.h"
#include "GridGeometry.h"


GridMesh::GridMesh(Qt3DCore::QNode *parent, const ContinuousElevationModel *const model, const LatLon &topLeftGeoPoint,
                   int sideLength, int resolution)
        : QGeometryRenderer(parent) {
    auto *geometry = new GridGeometry(this, model, topLeftGeoPoint, sideLength, resolution);
    Qt3DRender::QGeometryRenderer::setGeometry(geometry);
}
