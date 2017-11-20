
#include <ProgramConstants.h>
#include <3D/Utils.h>
#include "Ground.h"

Ground::Ground(Qt3DCore::QNode *parent) : Qt3DCore::QEntity(parent),
                                          transform_{new Qt3DCore::QTransform(this)} {
    // Build effect
    auto *shaderProgram = new Qt3DRender::QShaderProgram(this);
    shaderProgram->setVertexShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/terrain.vert"}));
    shaderProgram->setFragmentShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/DShader.frag"}));

    auto *renderPass = new Qt3DRender::QRenderPass(this);
    renderPass->setShaderProgram(shaderProgram);

    auto *filterKey = new Qt3DRender::QFilterKey(this);
    filterKey->setName("renderingStyle");
    filterKey->setValue("forward");

    auto *technique = new Qt3DRender::QTechnique(this);
    technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(OpenGLConstants::VERSION_MAJOR);
    technique->graphicsApiFilter()->setMinorVersion(OpenGLConstants::VERSION_MINOR);
    technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    technique->addFilterKey(filterKey);
    technique->addRenderPass(renderPass);

    auto *effect = new Qt3DRender::QEffect(this);
    effect->addTechnique(technique);

    // Set up material
    auto *diffuseParam = new Qt3DRender::QParameter(this);
    auto *diffuseTexture = loadTextureImage(QUrl{"qrc:3D/textures/diffusemap_photo_1024.jpg"}, this);
    diffuseParam->setName(QStringLiteral("diffuseTexture"));
    diffuseParam->setValue(QVariant::fromValue(diffuseTexture));

    auto *heightParam = new Qt3DRender::QParameter(this);
    auto *heightTexture = loadTextureImage(QUrl{"qrc:3D/textures/heightmap_1024.jpg"}, this);
    heightParam->setName(QStringLiteral("heightTexture"));
    heightParam->setValue(QVariant::fromValue(heightTexture));

    auto *material = new Qt3DRender::QMaterial(this);
    material->setEffect(effect);
    material->addParameter(diffuseParam);
    material->addParameter(heightParam);

    // Set up mesh
    auto *mesh = new Qt3DExtras::QPlaneMesh(this);
    mesh->setHeight(100);
    mesh->setWidth(100);
    mesh->setMeshResolution(QSize{100, 100});

    this->addComponent(mesh);
    this->addComponent(material);
}