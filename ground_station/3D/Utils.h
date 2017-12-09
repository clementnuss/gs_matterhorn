
#ifndef GS_MATTERHORN_UTILS_H
#define GS_MATTERHORN_UTILS_H

#include <ProgramConstants.h>
#include <Qt3DRender/QTextureImage>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QGraphicsApiFilter>
#include <QtGui/QMatrix4x4>

static Qt3DRender::QTexture2D *loadTextureImage(const QUrl &textureUrl) {
    auto *texture2D = new Qt3DRender::QTexture2D();
    auto *textureImage = new Qt3DRender::QTextureImage();

    textureImage->setSource(textureUrl);

    texture2D->setMinificationFilter(Qt3DRender::QTexture2D::LinearMipMapLinear);
    texture2D->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);
    texture2D->setWrapMode(Qt3DRender::QTextureWrapMode{Qt3DRender::QTextureWrapMode::ClampToEdge});
    texture2D->setGenerateMipMaps(true);
    texture2D->setMaximumAnisotropy(OpenGLConstants::ANISOTROPY_MAX);

    texture2D->addTextureImage(textureImage);

    return texture2D;
}

static Qt3DRender::QTechnique *buildTechnique() {
    auto *technique = new Qt3DRender::QTechnique();
    technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(OpenGLConstants::VERSION_MAJOR);
    technique->graphicsApiFilter()->setMinorVersion(OpenGLConstants::VERSION_MINOR);
    technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    return technique;
}

static QMatrix4x4 billboardMV(const QVector3D &position, QMatrix4x4 viewMatrix) {
    QMatrix4x4 transform{};
    float *viewMatrixData = viewMatrix.data();
    float *transformData = transform.data();

    transformData[0] = viewMatrixData[0];
    transformData[1] = viewMatrixData[4];
    transformData[2] = viewMatrixData[8];
    transformData[3] = 0;

    transformData[4] = viewMatrixData[1];
    transformData[5] = viewMatrixData[5];
    transformData[6] = viewMatrixData[9];
    transformData[7] = 0;

    transformData[8] = viewMatrixData[2];
    transformData[9] = viewMatrixData[6];
    transformData[10] = viewMatrixData[10];
    transformData[11] = 0;

    transformData[12] = position.x();
    transformData[13] = position.y();
    transformData[14] = position.z();
    transformData[15] = 1;

    return transform;
}

#endif //GS_MATTERHORN_UTILS_H
