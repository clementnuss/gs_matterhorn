
#include <3D/Scene/WorldReference.h>
#include "GridMesh.h"


GridMesh::GridMesh(Qt3DCore::QNode *parent, const std::function<float(int, int)> &heightSampler,
                   const std::function<QVector3D(int, int)> &normalSampler, int sideLength, int resolution)
        : QGeometryRenderer(parent),
          geometry_{new GridGeometry(
                  this,
                  heightSampler,
                  normalSampler,
                  sideLength,
                  resolution
          )} {

    Qt3DRender::QGeometryRenderer::setGeometry(geometry_);
}

float
GridMesh::vertexHeightAt(int i, int j) const {
    return geometry_->vertexHeightAt(i, j);
}

void
GridMesh::resampleVertices(const std::function<float(int, int)> &heightSampler,
                           const std::function<QVector3D(int, int)> &normalSampler) {
    geometry_->resampleVertices(heightSampler, normalSampler);
}