
#include <ProgramConstants.h>
#include <3D/Utils.h>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QMaterial>
#include <Qt3DExtras/QPlaneMesh>
#include <3D/Grid/GridMesh.h>
#include "Ground.h"

/**
 * 1000x1000 units terrain surface. Correspond to meters
 *
 * @param parent The QNode parent to which to attach this entity. Will usually be the scene root.
 */
Ground::Ground(Qt3DCore::QNode *parent, const QVector2D &offset, const LatLon &topLeftLatLon) : Qt3DCore::QEntity(
        parent),
                                                                                                transform_{
                                                                                                        new Qt3DCore::QTransform()},
                                                                                                highlightedArea_{
                                                                                                        nullptr} {
    // Build effect
    auto *shaderProgram = new Qt3DRender::QShaderProgram();
    shaderProgram->setVertexShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/terrain.vert"}));
    shaderProgram->setFragmentShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/terrain.frag"}));

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
    auto *diffuseTexture = loadTextureImage(QUrl{"qrc:3D/textures/diffusemap_photo_1024.jpg"});
    diffuseParam->setName(QStringLiteral("diffuseTexture"));
    diffuseParam->setValue(QVariant::fromValue(diffuseTexture));

    auto *heightParam = new Qt3DRender::QParameter();
    auto *heightTexture = loadTextureImage(QUrl{"qrc:3D/textures/heightmap_1024.jpg"});
    heightParam->setName(QStringLiteral("heightTexture"));
    heightParam->setValue(QVariant::fromValue(heightTexture));

    auto *material = new Qt3DRender::QMaterial();
    material->setEffect(effect);
    material->addParameter(diffuseParam);
    material->addParameter(heightParam);

    // Set up mesh
    GeoPoint gp{{46, 0, 0}, {6, 0, 0}};
    std::string s{"../../ground_station/data/N46E006.hgt"};
    auto *mesh = new GridMesh(nullptr, new ContinuousElevationModel(new DiscreteElevationModel(s, gp)),
                              topLeftLatLon, 10000, 101);

    transform_->setTranslation(QVector3D{offset.x(), 0.0, offset.y()});
    this->addComponent(transform_);
    this->addComponent(mesh);
    this->addComponent(material);

    // Add highlighted surface
    highlightedArea_ = new HighlightArea(heightParam, parent);
}


void Ground::highlightArea(const QVector2D &areaCenter) {
    highlightedArea_->updatePos(areaCenter);
}