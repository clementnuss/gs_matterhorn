
#ifndef GS_MATTERHORN_UTILS_H
#define GS_MATTERHORN_UTILS_H

#include <Qt3DCore>
#include <Qt3DRender>
#include <ProgramConstants.h>

Qt3DRender::QTexture2D *loadTextureImage(QUrl textureUrl, Qt3DCore::QNode *owner) {
    auto *texture2D = new Qt3DRender::QTexture2D(owner);
    auto *textureImage = new Qt3DRender::QTextureImage(texture2D);

    textureImage->setSource(textureUrl);

    texture2D->setMinificationFilter(Qt3DRender::QTexture2D::LinearMipMapLinear);
    texture2D->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);
    texture2D->setWrapMode(Qt3DRender::QTextureWrapMode{Qt3DRender::QTextureWrapMode::ClampToEdge, owner});
    texture2D->setGenerateMipMaps(true);
    texture2D->setMaximumAnisotropy(OpenGLConstants::ANISOTROPY_MAX);
    texture2D->addTextureImage(textureImage);

    return texture2D;
}


#endif //GS_MATTERHORN_UTILS_H
