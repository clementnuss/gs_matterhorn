
#include <ProgramConstants.h>
#include <3D/Utils.h>
#include "Ground.h"
#include "HighlightArea.h"

/**
 * 1000x1000 units terrain surface. Correspond to meters
 *
 * @param parent The QNode parent to which to attach this entity. Will usually be the scene root.
 */
Ground::Ground(Qt3DCore::QNode *parent) : Qt3DCore::QEntity(parent),
                                          transform_{new Qt3DCore::QTransform()} {
    // Build effect
    auto *shaderProgram = new Qt3DRender::QShaderProgram();
    shaderProgram->setVertexShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/terrain.vert"}));
    shaderProgram->setFragmentShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/DShader.frag"}));

    auto *renderPass = new Qt3DRender::QRenderPass();
    renderPass->setShaderProgram(shaderProgram);

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
    auto *diffuseParam = new Qt3DRender::QParameter();
    auto *diffuseTexture = loadTextureImage(QUrl{"qrc:3D/textures/default.jpg"});
    diffuseParam->setName(QStringLiteral("diffuseTexture"));
    diffuseParam->setValue(QVariant::fromValue(diffuseTexture));

    auto *heightParam = new Qt3DRender::QParameter();
    auto *heightTexture = loadTextureImage(QUrl{"qrc:3D/textures/default.jpg"});
    heightParam->setName(QStringLiteral("heightTexture"));
    heightParam->setValue(QVariant::fromValue(heightTexture));

    auto *material = new Qt3DRender::QMaterial();
    material->setEffect(effect);
    material->addParameter(diffuseParam);
    material->addParameter(heightParam);

    // Set up mesh
    auto *mesh = new Qt3DExtras::QPlaneMesh();
    mesh->setHeight(10000);
    mesh->setWidth(10000);
    mesh->setMeshResolution(QSize{100, 100});

    this->addComponent(mesh);
    this->addComponent(material);

    // Add highlighted surface
    new HighlightArea(heightParam, this);
}