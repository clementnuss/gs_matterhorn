
#include <Qt3DRender/FunctorType>
#include <3D/3DVisualisationConstants.h>
#include "GridGeometry.h"

QByteArray createPlaneVertexData(float w, float h, bool mirrored) {
    Q_ASSERT(w > 0.0f);
    Q_ASSERT(h > 0.0f);
    Q_ASSERT(GridConstants::GRID_RESOLUTION >= 2);
    Q_ASSERT(GridConstants::GRID_RESOLUTION >= 2);

    const int nVerts = GridConstants::GRID_RESOLUTION * GridConstants::GRID_RESOLUTION;

    // Populate a buffer with the interleaved per-vertex data with
    // vec3 pos, vec2 texCoord, vec3 normal, vec4 tangent
    const quint32 elementSize = 3 + 2 + 3 + 4;
    const quint32 stride = elementSize * sizeof(float);
    QByteArray bufferBytes;
    bufferBytes.resize(stride * nVerts);
    float *fptr = reinterpret_cast<float *>(bufferBytes.data());

    const float x0 = -w / 2.0f;
    const float z0 = -h / 2.0f;
    const float dx = w / (GridConstants::GRID_RESOLUTION - 1);
    const float dz = h / (GridConstants::GRID_RESOLUTION - 1);
    const float du = 1.0 / (GridConstants::GRID_RESOLUTION - 1);
    const float dv = 1.0 / (GridConstants::GRID_RESOLUTION - 1);

    // Iterate over z
    for (int j = 0; j < GridConstants::GRID_RESOLUTION; ++j) {
        const float z = z0 + static_cast<float>(j) * dz;
        const float v = static_cast<float>(j) * dv;

        // Iterate over x
        for (int i = 0; i < GridConstants::GRID_RESOLUTION; ++i) {
            const float x = x0 + static_cast<float>(i) * dx;
            const float u = static_cast<float>(i) * du;

            // position
            *fptr++ = x;
            *fptr++ = 0.0;
            *fptr++ = z;

            // texture coordinates
            *fptr++ = u;
            *fptr++ = mirrored ? 1.0f - v : v;

            // normal
            *fptr++ = 0.0f;
            *fptr++ = 1.0f;
            *fptr++ = 0.0f;

            // tangent
            *fptr++ = 1.0f;
            *fptr++ = 0.0f;
            *fptr++ = 0.0f;
            *fptr++ = 1.0f;
        }
    }

    return bufferBytes;
}

QByteArray createPlaneIndexData() {
    // Create the index data. 2 triangles per rectangular face
    const int faces = 2 * (GridConstants::GRID_RESOLUTION - 1) * (GridConstants::GRID_RESOLUTION - 1);
    const int indices = 3 * faces;
    Q_ASSERT(indices < std::numeric_limits<quint16>::max());
    QByteArray indexBytes;
    indexBytes.resize(indices * sizeof(quint16));
    quint16 *indexPtr = reinterpret_cast<quint16 *>(indexBytes.data());

    // Iterate over z
    for (int j = 0; j < GridConstants::GRID_RESOLUTION - 1; ++j) {
        const int rowStartIndex = j * GridConstants::GRID_RESOLUTION;
        const int nextRowStartIndex = (j + 1) * GridConstants::GRID_RESOLUTION;

        // Iterate over x
        for (int i = 0; i < GridConstants::GRID_RESOLUTION - 1; ++i) {
            // Split quad into two triangles
            *indexPtr++ = rowStartIndex + i;
            *indexPtr++ = nextRowStartIndex + i;
            *indexPtr++ = rowStartIndex + i + 1;

            *indexPtr++ = nextRowStartIndex + i;
            *indexPtr++ = nextRowStartIndex + i + 1;
            *indexPtr++ = rowStartIndex + i + 1;
        }
    }

    return indexBytes;
}


GridGeometry::GridGeometry(Qt3DCore::QNode *parent) : Qt3DRender::QGeometry(parent),
                                                      positionAttribute_(new Qt3DRender::QAttribute()),
                                                      normalAttribute_(new Qt3DRender::QAttribute()),
                                                      texCoordAttribute_(new Qt3DRender::QAttribute()),
                                                      tangentAttribute_(new Qt3DRender::QAttribute()),
                                                      indexAttribute_(new Qt3DRender::QAttribute()),
                                                      vertexBuffer_(new Qt3DRender::QBuffer()),
                                                      indexBuffer_(new Qt3DRender::QBuffer()) {
    const int nVerts = GridConstants::GRID_RESOLUTION * GridConstants::GRID_RESOLUTION;
    const int stride = (3 + 2 + 3 + 4) * sizeof(float);
    const int faces = 2 * (GridConstants::GRID_RESOLUTION - 1) * (GridConstants::GRID_RESOLUTION - 1);

    vertexBuffer_->setData(createPlaneVertexData(10000, 10000, false));
    indexBuffer_->setData(createPlaneIndexData());

    positionAttribute_->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute_->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute_->setVertexSize(3);
    positionAttribute_->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute_->setBuffer(vertexBuffer_);
    positionAttribute_->setByteStride(stride);
    positionAttribute_->setCount(nVerts);

    texCoordAttribute_->setName(Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName());
    texCoordAttribute_->setVertexBaseType(Qt3DRender::QAttribute::Float);
    texCoordAttribute_->setVertexSize(2);
    texCoordAttribute_->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    texCoordAttribute_->setBuffer(vertexBuffer_);
    texCoordAttribute_->setByteStride(stride);
    texCoordAttribute_->setByteOffset(3 * sizeof(float));
    texCoordAttribute_->setCount(nVerts);

    normalAttribute_->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());
    normalAttribute_->setVertexBaseType(Qt3DRender::QAttribute::Float);
    normalAttribute_->setVertexSize(3);
    normalAttribute_->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    normalAttribute_->setBuffer(vertexBuffer_);
    normalAttribute_->setByteStride(stride);
    normalAttribute_->setByteOffset(5 * sizeof(float));
    normalAttribute_->setCount(nVerts);

    tangentAttribute_->setName(Qt3DRender::QAttribute::defaultTangentAttributeName());
    tangentAttribute_->setVertexBaseType(Qt3DRender::QAttribute::Float);
    tangentAttribute_->setVertexSize(4);
    tangentAttribute_->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    tangentAttribute_->setBuffer(vertexBuffer_);
    tangentAttribute_->setByteStride(stride);
    tangentAttribute_->setByteOffset(8 * sizeof(float));
    tangentAttribute_->setCount(nVerts);

    indexAttribute_->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute_->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);
    indexAttribute_->setBuffer(indexBuffer_);

    // Each primitive has 3 vertives
    indexAttribute_->setCount(faces * 3);

    this->addAttribute(positionAttribute_);
    this->addAttribute(texCoordAttribute_);
    this->addAttribute(normalAttribute_);
    this->addAttribute(tangentAttribute_);
    this->addAttribute(indexAttribute_);
}
