#include <ProgramConstants.h>
#include <3D/Utils.h>
#include <3D/ForwardRenderer/LayerManager.h>
#include <3D/TextureManagerSingleton.h>
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPlaneMesh>
#include "Marker.h"

const QVector3D Marker::basePosition_{0, 0, 0};

Marker::Marker(const QString &texture,
               float width, float height,
               QVector3D offset,
               Qt3DRender::QCamera *camera,
               Qt3DCore::QNode *parent)
        : Qt3DCore::QEntity(parent),
          camera_{camera}, offset_{offset}, transform_{new Qt3DCore::QTransform()} {
    // Build effect
    auto *shaderProgram = new Qt3DRender::QShaderProgram();
    shaderProgram->setVertexShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/marker.vert"}));
    shaderProgram->setFragmentShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/DShader.frag"}));

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

    auto *technique = buildTechnique();

    technique->addFilterKey(filterKey);
    technique->addRenderPass(renderPass);

    auto *effect = new Qt3DRender::QEffect();
    effect->addTechnique(technique);

    // Set up material
    auto *diffuseParam = new Qt3DRender::QParameter();
    diffuseParam->setName(QStringLiteral("diffuseTexture"));
    diffuseParam->setValue(QVariant::fromValue(TextureManagerSingleton::getInstance().getTexture(texture)));

    auto *material = new Qt3DRender::QMaterial();
    material->setEffect(effect);
    material->addParameter(diffuseParam);

    // Set up mesh
    auto *mesh = new Qt3DExtras::QPlaneMesh();
    mesh->setWidth(width);
    mesh->setHeight(height);
    mesh->setMeshResolution(QSize{2, 2});

    transform_->setTranslation(offset_);

    this->addComponent(mesh);
    this->addComponent(material);
    this->addComponent(transform_);
    this->addComponent(LayerManager::getInstance().getLayer(LayerType::BILLBOARDS_1));

    // Set up transform
    updateTransform();

    connect(camera, &Qt3DRender::QCamera::viewMatrixChanged,
            this, &Marker::updateTransform);
}

void
Marker::updateTransform() {
    QMatrix4x4 t{};
    // Make it face camera since default is like laying flat on ground
    t.translate(offset_);
    t.scale({UI3DConstants::MARKER_SCALE, UI3DConstants::MARKER_SCALE, UI3DConstants::MARKER_SCALE});
    t.rotate(180, QVector3D(0, 1, 0));
    t.rotate(-90, QVector3D(1, 0, 0));
    transform_->setMatrix(billboardMV(basePosition_, camera_->viewMatrix()) * t);
}