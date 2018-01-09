
#include <3D/Scene/WorldReference.h>
#include "GridMesh.h"
#include "GridGeometry.h"


GridMesh::GridMesh(Qt3DCore::QNode *parent, const ContinuousElevationModel *const model,
                   const WorldReference *const worldRef,
                   const LatLon &topLeftGeoPoint, int sideLength, int resolution)
        : QGeometryRenderer(parent) {
    auto *geometry = new GridGeometry(this, model, worldRef, topLeftGeoPoint, sideLength, resolution);
    Qt3DRender::QGeometryRenderer::setGeometry(geometry);
}
