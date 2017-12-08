
#include <QtGui/QVector3D>
#include <Qt3DExtras/QExtrudedTextMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include "Text3D.h"
#include "3D/Utils.h"


const QVector3D Text3D::basePosition_{0, 0, 0};

//TODO: Implement common class for billboards
//TODO: change shader name to uniform color shader
Text3D::Text3D(QString text, Qt3DRender::QCamera *camera, QVector3D &offsetToParent, Qt3DCore::QNode *parent) :
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

    auto *test = new Qt3DExtras::QExtrudedTextMesh();

    test->setFont({"Times", 18, QFont::Bold});
    test->setText(text);
    test->setDepth(0.1);

    auto *transform = new Qt3DCore::QTransform();
    transform->setTranslation(offset_);

    this->addComponent(test);
    this->addComponent(transform_);
    this->addComponent(material);

    connect(camera, &Qt3DRender::QCamera::viewMatrixChanged,
            this, &Text3D::updateTransform);
}

void Text3D::updateTransform() {
    QMatrix4x4 t{};
    // Make it face camera since default is like laying flat on ground
    t.translate(offset_);
    transform_->setMatrix(billboardMV(basePosition_, camera_->viewMatrix()) * t);
}