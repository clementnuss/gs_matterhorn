
#include "GroundTile.h"

#include <Qt3DRender/QEffect>
#include <Qt3DRender/QMaterial>

/**
 * 1000x1000 units terrain surface. Correspond to meters
 *
 * @param parent The QNode parent to which to attach this entity. Will usually be the scene root.
 */
GroundTile::GroundTile(Qt3DCore::QNode *parent,
                       const QVector2D &offset,
                       const LatLon &topLeftLatLon,
                       const ContinuousElevationModel *model,
                       const WorldReference *const worldRef,
                       const int textureID) :
        Qt3DCore::QEntity(parent),
        transform_{new Qt3DCore::QTransform()},
        mesh_{new GridMesh(
                nullptr,
                [model, worldRef, topLeftLatLon](int x, int z) {
                    LatLon p{
                            worldRef->latitudeFromDistance(x, topLeftLatLon.latitude),
                            worldRef->longitudeFromDistance(z, topLeftLatLon.longitude)
                    };
                    return model->elevationAt(p);
                },
                [model, worldRef, topLeftLatLon](int x, int z) {
                    LatLon p{
                            worldRef->latitudeFromDistance(x, topLeftLatLon.latitude),
                            worldRef->longitudeFromDistance(z, topLeftLatLon.longitude)
                    };
                    return model->slopeAt(p);
                },
                GridConstants::GRID_LENGTH_IN_METERS,
                GridConstants::GRID_RESOLUTION)} {

    // Build effect
    auto *shaderProgram = new Qt3DRender::QShaderProgram();
    shaderProgram->setVertexShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/terrain.vert"}));
    shaderProgram->setFragmentShaderCode(
            shaderProgram->loadSource(
                    textureID != -1 ? QUrl{"qrc:/shaders/terrain.frag"} : QUrl{"qrc:/shaders/terrain_notex.frag"}
            ));

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
    auto *material = new Qt3DRender::QMaterial();
    material->setEffect(effect);

    if (textureID != -1) {
        auto *diffuseParam = new Qt3DRender::QParameter();

        QString s{"qrc:3D/textures/terrain/tex_"};
        s += textureID;
        s += ".jpg";

        auto *diffuseTexture = loadTextureImage(QUrl{s});
        diffuseParam->setName(QStringLiteral("diffuseTexture"));
        diffuseParam->setValue(QVariant::fromValue(diffuseTexture));
        material->addParameter(diffuseParam);
    }

    transform_->setTranslation(QVector3D{offset.x(), 0.0, offset.y()});
    this->addComponent(transform_);
    this->addComponent(mesh_);
    this->addComponent(material);
}

float GroundTile::vertexHeightAt(int i, int j) const {
    return mesh_->vertexHeightAt(i, j);
}