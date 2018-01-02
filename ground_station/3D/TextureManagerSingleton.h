
#ifndef GS_MATTERHORN_TEXTUREMANAGERSINGLETON_H
#define GS_MATTERHORN_TEXTUREMANAGERSINGLETON_H

#include <QtCore/QString>
#include <Qt3DRender/QTexture>
#include "Utils.h"

/**
 * This class acts as a texture cache. It holds all the pointers to texture needed by the program.
 *
 * When a texture is required, the path to the texture should be given. If it was already loaded before
 * the singleton simply returns it. Otherwise it loads the texture from disk and return a pointer to it
 */
class TextureManagerSingleton {
public:
    static TextureManagerSingleton &getInstance() {
        static TextureManagerSingleton instance;
        return instance;
    }

    Qt3DRender::QTexture2D *getTexture(const QString &texturePath) {

        if (!textureMap_.contains(texturePath)) {
            // Load and store the texture
            Qt3DRender::QTexture2D *tex{loadTextureImage(QUrl{texturePath})};
            textureMap_.insert(texturePath, tex);
        }

        return textureMap_[texturePath];
    }

private:
    TextureManagerSingleton() : textureMap_{} {}

    QHash<QString, Qt3DRender::QTexture2D *> textureMap_;

public:
    TextureManagerSingleton(TextureManagerSingleton const &) = delete;

    void operator=(TextureManagerSingleton const &)  = delete;
};

#endif //GS_MATTERHORN_TEXTUREMANAGERSINGLETON_H
