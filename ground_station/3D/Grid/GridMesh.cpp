
#include "GridMesh.h"
#include "GridGeometry.h"


GridMesh::GridMesh(Qt3DCore::QNode *parent, const DiscreteElevationModel *const model)
        : QGeometryRenderer(parent) {
    auto *geometry = new GridGeometry(this, model);
    Qt3DRender::QGeometryRenderer::setGeometry(geometry);
}
