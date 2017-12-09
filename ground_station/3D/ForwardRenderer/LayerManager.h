
#ifndef GS_MATTERHORN_LAYERMANAGER_H
#define GS_MATTERHORN_LAYERMANAGER_H

#include <Qt3DRender/QLayer>

enum class LayerType {
    VISIBLE, INVISIBLE
};


class LayerManager {
public:
    static LayerManager &getInstance() {
        static LayerManager instance;

        return instance;
    }

    Qt3DRender::QLayer *getLayer(LayerType layerType) {
        return layerMap_[layerType];
    }

private:
    LayerManager() : layerMap_{
            {LayerType::VISIBLE,   new Qt3DRender::QLayer()},
            {LayerType::INVISIBLE, new Qt3DRender::QLayer()}
    } {

        for (auto pair : layerMap_) {
            //pair.second->setRecursive(true); //TODO: activate once on Qt 5.10
        }
    }

    std::map<LayerType, Qt3DRender::QLayer *> layerMap_;

public:
    LayerManager(LayerManager const &) = delete;

    void operator=(LayerManager const &)  = delete;
};

#endif //GS_MATTERHORN_LAYERMANAGER_H
