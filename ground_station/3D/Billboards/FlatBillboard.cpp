
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QRenderPass>
#include <3D/Utils.h>
#include <Qt3DRender/QEffect>
#include <3D/TextureManagerSingleton.h>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DExtras/QPlaneMesh>
#include <3D/ForwardRenderer/LayerManager.h>
#include <Qt3DCore/QTransform>
#include "FlatBillboard.h"

FlatBillboard::FlatBillboard(const QString &texture, float width, float height, Qt3DCore::QNode *parent)
        : Qt3DCore::QEntity(parent), transform_{new Qt3DCore::QTransform()} {

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

    // Set up transform

    this->addComponent(mesh);
    this->addComponent(material);
    this->addComponent(transform_);
    this->addComponent(LayerManager::getInstance().getLayer(LayerType::BILLBOARDS_1));
}