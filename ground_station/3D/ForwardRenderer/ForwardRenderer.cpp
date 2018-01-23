
#include <ProgramConstants.h>
#include "ForwardRenderer.h"


ForwardRenderer::ForwardRenderer(Qt3DExtras::Qt3DWindow *view, Qt3DCore::QNode *parent) : Qt3DRender::QTechniqueFilter(
        parent) {

    auto *filterKey = new Qt3DRender::QFilterKey(this);
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));

    this->addMatch(filterKey);

    auto *surfaceSelector = new Qt3DRender::QRenderSurfaceSelector(this);
    surfaceSelector->setSurface(view);

    auto *viewPort = new Qt3DRender::QViewport(surfaceSelector);
    viewPort->setNormalizedRect(QRectF{0.0, 0.0, 1.0, 1.0});

    auto *cameraSelector = new Qt3DRender::QCameraSelector(viewPort);
    cameraSelector->setCamera(view->camera());

    auto *clearBuffer = new Qt3DRender::QClearBuffers(cameraSelector);
    clearBuffer->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    clearBuffer->setClearColor(QColor{"#383838"});

    /*

    auto *visibleLayerFilter = new Qt3DRender::QLayerFilter(clearBuffer);
    visibleLayerFilter->setFilterMode(Qt3DRender::QLayerFilter::DiscardAnyMatchingLayers);
    visibleLayerFilter->addLayer(LayerManager::getInstance().getLayer(LayerType::INVISIBLE));

    /*
    auto *billboards1LayerFilter = new Qt3DRender::QLayerFilter(visibleLayerFilter);
    billboards1LayerFilter->setFilterMode(Qt3DRender::QLayerFilter::AcceptAllMatchingLayers);
    billboards1LayerFilter->addLayer(LayerManager::getInstance().getLayer(LayerType::VISIBLE));
    billboards1LayerFilter->addLayer(LayerManager::getInstance().getLayer(LayerType::BILLBOARDS_1));

    auto *billboards2LayerFilter = new Qt3DRender::QLayerFilter(visibleLayerFilter);
    billboards1LayerFilter->setFilterMode(Qt3DRender::QLayerFilter::AcceptAllMatchingLayers);
    billboards1LayerFilter->addLayer(LayerManager::getInstance().getLayer(LayerType::VISIBLE));
    billboards1LayerFilter->addLayer(LayerManager::getInstance().getLayer(LayerType::BILLBOARDS_2));*/
}