
#include "GridMesh.h"
#include "GridGeometry.h"


GridMesh::GridMesh(Qt3DCore::QNode *parent)
        : QGeometryRenderer(parent) {
    auto *geometry = new GridGeometry(this);
    Qt3DRender::QGeometryRenderer::setGeometry(geometry);
}
