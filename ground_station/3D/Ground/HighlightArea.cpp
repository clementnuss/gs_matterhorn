#include "HighlightArea.h"
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QMaterial>


HighlightArea::HighlightArea(Qt3DCore::QNode *parent) :
        Qt3DCore::QEntity(parent),
        transform_{new Qt3DCore::QTransform()},
        mesh_{new GridMesh(
                this,
                [](int x, int y) { return 0.0f; },
                [](int x, int y) { return QVector3D(0, 0, 0); },
                GridConstants::HIGHLIGHT_AREA_IN_METERS,
                GridConstants::HIGHLIGHT_AREA_RESOLUTION
        )} {

    // Build effect
    auto *shaderProgram = new Qt3DRender::QShaderProgram();
    shaderProgram->setVertexShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/highlightArea.vert"}));
    shaderProgram->setFragmentShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/highlightArea.frag"}));

    auto *blendEquationArguments = new Qt3DRender::QBlendEquationArguments();
    blendEquationArguments->setSourceAlpha(Qt3DRender::QBlendEquationArguments::One);
    blendEquationArguments->setDestinationAlpha(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);
    blendEquationArguments->setSourceRgb(Qt3DRender::QBlendEquationArguments::SourceAlpha);
    blendEquationArguments->setDestinationRgb(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);

    auto *renderPass = new Qt3DRender::QRenderPass();
    renderPass->setShaderProgram(shaderProgram);
    renderPass->addRenderState(blendEquationArguments);

    auto *filterKey = new Qt3DRender::QFilterKey();
    filterKey->setName("renderingStyle");
    filterKey->setValue("forward");

    auto *technique = new Qt3DRender::QTechnique();
    technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(OpenGLConstants::VERSION_MAJOR);
    technique->graphicsApiFilter()->setMinorVersion(OpenGLConstants::VERSION_MINOR);
    technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    technique->addFilterKey(filterKey);
    technique->addRenderPass(renderPass);

    auto *effect = new Qt3DRender::QEffect();
    effect->addTechnique(technique);

    // Set up material
    auto *material = new Qt3DRender::QMaterial();
    material->setEffect(effect);

    this->addComponent(mesh_);
    this->addComponent(material);
    this->addComponent(transform_);
}

void
HighlightArea::updatePos(const QVector2D &pos, const std::function<float(int, int)> &heightSampler) {
    transform_->setTranslation(QVector3D{pos.x(), 20, pos.y()});
    mesh_->resampleVertices(heightSampler, [](int x, int y) { return QVector3D(0, 0, 0); });
}