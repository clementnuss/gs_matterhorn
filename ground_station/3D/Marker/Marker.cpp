#include <ProgramConstants.h>
#include <3D/Utils.h>
#include "Marker.h"

Marker::Marker(QUrl textureUrl, QVector3D position, Qt3DRender::QCamera *camera, Qt3DCore::QNode *parent)
        : Qt3DCore::QEntity(parent),
          camera_{camera}, position_{position}, transform_{new Qt3DCore::QTransform(this)} {
    // Build effect
    auto *shaderProgram = new Qt3DRender::QShaderProgram(this);
    shaderProgram->setVertexShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/marker.vert"}));
    shaderProgram->setFragmentShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/DShader.frag"}));

    auto *blendEquationArguments = new Qt3DRender::QBlendEquationArguments(this);
    blendEquationArguments->setSourceAlpha(Qt3DRender::QBlendEquationArguments::One);
    blendEquationArguments->setDestinationAlpha(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);
    blendEquationArguments->setSourceRgb(Qt3DRender::QBlendEquationArguments::SourceAlpha);
    blendEquationArguments->setDestinationRgb(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);

    auto *renderPass = new Qt3DRender::QRenderPass(this);
    renderPass->setShaderProgram(shaderProgram);
    renderPass->addRenderState(blendEquationArguments);

    auto *filterKey = new Qt3DRender::QFilterKey(this);
    filterKey->setName("renderingStyle");
    filterKey->setValue("forward");

    auto *technique = buildTechnique(this);

    technique->addFilterKey(filterKey);
    technique->addRenderPass(renderPass);

    auto *effect = new Qt3DRender::QEffect(this);
    effect->addTechnique(technique);

    // Set up material
    auto *diffuseParam = new Qt3DRender::QParameter(this);
    auto *diffuseTexture = loadTextureImage(textureUrl, this);
    diffuseParam->setName(QStringLiteral("diffuseTexture"));
    diffuseParam->setValue(QVariant::fromValue(diffuseTexture));

    auto *material = new Qt3DRender::QMaterial(this);
    material->setEffect(effect);
    material->addParameter(diffuseParam);

    // Set up mesh
    auto *mesh = new Qt3DExtras::QPlaneMesh(this);
    mesh->setHeight(2);
    mesh->setWidth(2);
    mesh->setMeshResolution(QSize{2, 2});

    // Set up transform
    updateTransform();

    this->addComponent(mesh);
    this->addComponent(material);
    this->addComponent(transform_);

    connect(camera, &Qt3DRender::QCamera::viewMatrixChanged,
            this, &Marker::updateTransform);
}

void Marker::updateTransform() {
    QMatrix4x4 t{};
    // Make it face camera since default is like laying flat on ground
    t.rotate(180, QVector3D(0, 1, 0));
    t.rotate(-90, QVector3D(1, 0, 0));
    transform_->setMatrix(billboardMV(position_, camera_->viewMatrix()) * t);
}

void Marker::updatePosition(const QVector3D &newPosition) {
    position_ = newPosition;
}