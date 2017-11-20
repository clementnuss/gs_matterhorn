
#include "ForwardRenderer.h"


ForwardRenderer::ForwardRenderer(Qt3DCore::QEntity *camera, Qt3DCore::QNode *parent) : Qt3DRender::QTechniqueFilter(
        parent) {

    auto *filterKey = new Qt3DRender::QFilterKey(this);
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));

    this->addMatch(filterKey);

    auto *surfaceSelector = new Qt3DRender::QRenderSurfaceSelector(this);

    auto *viewPort = new Qt3DRender::QViewport(surfaceSelector);
    viewPort->setNormalizedRect(QRectF{0.0, 0.0, 1.0, 1.0});

    auto *cameraSelector = new Qt3DRender::QCameraSelector(viewPort);
    cameraSelector->setCamera(camera);

    auto *clearBuffer = new Qt3DRender::QClearBuffers(cameraSelector);
    clearBuffer->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    clearBuffer->setClearColor(QColor{"steelblue"});

}