
#include <Qt3DRender/FunctorType>
#include <3D/3DVisualisationConstants.h>
#include <3D/Utils.h>
#include "GridGeometry.h"

QByteArray GridGeometry::createPlaneVertexData() {
    Q_ASSERT(sideLength_ > 0.0f);
    Q_ASSERT(gridResolution_ >= 2);

    const int nVerts = gridResolution_ * gridResolution_;
    float vertSpacing = sideLength_ / (gridResolution_ - 1);

    // Populate a buffer with the interleaved per-vertex data with
    // vec3 pos, vec2 texCoord, vec3 normal, vec4 tangent
    const quint32 elementSize = 3 + 2 + 3 + 4;
    const quint32 stride = elementSize * sizeof(float);
    QByteArray bufferBytes;
    bufferBytes.resize(stride * nVerts);
    float *fptr = reinterpret_cast<float *>(bufferBytes.data());

    const float x0 = 0.0f;
    const float z0 = 0.0f;
    const float dx = vertSpacing;
    const float dz = vertSpacing;
    const float du = 1.0f / (gridResolution_ - 1);
    const float dv = 1.0f / (gridResolution_ - 1);

    // Iterate over z
    for (int j = 0; j < gridResolution_; ++j) {
        const float z = z0 + static_cast<float>(j) * dz;
        const float u = static_cast<float>(j) * du;

        // Iterate over x
        for (int i = 0; i < gridResolution_; ++i) {
            const float x = x0 - static_cast<float>(i) * dx;
            const float v = static_cast<float>(i) * dv;

            LatLon p{
                    worldRef_->latitudeFromDistance(x, topLeftLatLon_.latitude),
                    worldRef_->longitudeFromDistance(z, topLeftLatLon_.longitude)
            };

            // position
            *fptr++ = x;
            *fptr++ = model_->elevationAt(p);
            *fptr++ = z;

            // texture coordinates
            *fptr++ = u;
            *fptr++ = 1.0f - v;

            // normal
            QVector3D n = model_->slopeAt(p);
            *fptr++ = n.x();
            *fptr++ = n.y();
            *fptr++ = n.z();

            // tangent
            *fptr++ = 1.0f;
            *fptr++ = 0.0f;
            *fptr++ = 0.0f;
            *fptr++ = 1.0f;
        }
    }

    return bufferBytes;
}

QByteArray GridGeometry::createPlaneIndexData() {
    // Create the index data. 2 triangles per rectangular face
    const int faces = 2 * (gridResolution_ - 1) * (gridResolution_ - 1);
    const int indices = 3 * faces;
    Q_ASSERT(indices < std::numeric_limits<quint16>::max());
    QByteArray indexBytes;
    indexBytes.resize(indices * sizeof(quint16));
    quint16 *indexPtr = reinterpret_cast<quint16 *>(indexBytes.data());

    // Iterate over z
    for (int j = 0; j < gridResolution_ - 1; ++j) {
        const int rowStartIndex = j * gridResolution_;
        const int nextRowStartIndex = (j + 1) * gridResolution_;

        // Iterate over x
        for (int i = 0; i < GridConstants::GRID_RESOLUTION - 1; ++i) {
            // Split quad into two triangles
            *indexPtr++ = rowStartIndex + i;
            *indexPtr++ = rowStartIndex + i + 1;
            *indexPtr++ = nextRowStartIndex + i;

            *indexPtr++ = nextRowStartIndex + i;
            *indexPtr++ = rowStartIndex + i + 1;
            *indexPtr++ = nextRowStartIndex + i + 1;
        }
    }

    return indexBytes;
}


GridGeometry::GridGeometry(Qt3DCore::QNode *parent, const ContinuousElevationModel *const model,
                           const WorldReference *const worldRef, const LatLon &topLeftGeoPoint, int sideLength,
                           int resolution)
        :
        Qt3DRender::QGeometry(parent),
        model_{model},
        worldRef_{worldRef},
        sideLength_{sideLength},
        gridResolution_{resolution},
        topLeftLatLon_{topLeftGeoPoint},
        positionAttribute_(new Qt3DRender::QAttribute()),
        normalAttribute_(new Qt3DRender::QAttribute()),
        texCoordAttribute_(new Qt3DRender::QAttribute()),
        tangentAttribute_(new Qt3DRender::QAttribute()),
        indexAttribute_(new Qt3DRender::QAttribute()),
        vertexBuffer_(new Qt3DRender::QBuffer()),
        indexBuffer_(new Qt3DRender::QBuffer()) {

    const int nVerts = gridResolution_ * gridResolution_;
    const int stride = (3 + 2 + 3 + 4) * sizeof(float);
    const int faces = 2 * (gridResolution_ - 1) * (gridResolution_ - 1);

    vertexBuffer_->setData(createPlaneVertexData());
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
