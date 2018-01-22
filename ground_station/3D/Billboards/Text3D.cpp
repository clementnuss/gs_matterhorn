
#include <QtGui/QVector3D>
#include <Qt3DExtras/QExtrudedTextMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QParameter>
#include <3D/ForwardRenderer/LayerManager.h>
#include "Text3D.h"
#include "3D/Utils.h"


const QVector3D Text3D::basePosition_{0, 0, 0};

//TODO: Implement common class for billboards
//TODO: change shader name to uniform color shader
Text3D::Text3D(QString text, TextType textType, Qt3DRender::QCamera *camera, QVector3D &offsetToParent,
               Qt3DCore::QNode *parent) :
        QEntity(parent),
        transform_{new Qt3DCore::QTransform()},
        camera_{camera},
        offset_{offsetToParent} {
    // Build effect
    auto *shaderProgram = new Qt3DRender::QShaderProgram();
    shaderProgram->setVertexShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/line.vert"}));
    shaderProgram->setFragmentShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/line.frag"}));

    auto *renderPass = new Qt3DRender::QRenderPass();

    renderPass->setShaderProgram(shaderProgram);

    auto *filterKey = new Qt3DRender::QFilterKey();
    filterKey->setName("renderingStyle");
    filterKey->setValue("forward");

    auto *technique = buildTechnique();

    technique->addFilterKey(filterKey);
    technique->addRenderPass(renderPass);

    auto *effect = new Qt3DRender::QEffect();
    effect->addTechnique(technique);

    auto *material = new Qt3DRender::QMaterial();
    material->addParameter(new Qt3DRender::QParameter("lineColor", QColor::fromRgb(255, 255, 255)));
    material->setEffect(effect);

    auto *textMesh = new Qt3DExtras::QExtrudedTextMesh();

    textMesh->setText(text);
    textMesh->setDepth(0.1);

    switch (textType) {
        case TextType::BOLD:
            textMesh->setFont({"Tahoma", 18, QFont::Bold});
            break;
        case TextType::LEGEND:
            textMesh->setFont({"Tahoma", 18,
                               QFont::Bold}); // Font size has limited effect in 3D space. + It can change the mesh resolution.
            transform_->setScale(0.8);
            break;
        default:
            textMesh->setFont({"Courier", 14});
            break;
    }

    this->addComponent(textMesh);
    this->addComponent(transform_);
    this->addComponent(material);
    this->addComponent(LayerManager::getInstance().getLayer(LayerType::BILLBOARDS_1));

    connect(camera, &Qt3DRender::QCamera::viewMatrixChanged,
            this, &Text3D::updateTransform);
}

void Text3D::updateTransform() {
    QMatrix4x4 t{};
    // Make it face camera since default is like laying flat on ground
    t.translate(offset_);
    t.scale(transform_->scale());
    transform_->setMatrix(billboardMV(basePosition_, camera_->viewMatrix()) * t);
}