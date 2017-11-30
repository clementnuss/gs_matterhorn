#include <iostream>
#include <ProgramConstants.h>
#include "Line.h"


Line::Line(Qt3DCore::QNode *parent, QColor &&color, bool isStatic) : Qt3DCore::QEntity(parent),
                                                                     geometryRenderer_{new Qt3DRender::QGeometryRenderer(this)},
                                                                     geometry_{new Qt3DRender::QGeometry(this)},
                                                                     attribute_{new Qt3DRender::QAttribute(this)},
                                                                     buffer_{new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this)},
                                                                     count_{0} {

    // Build effect
    auto *shaderProgram = new Qt3DRender::QShaderProgram(this);
    shaderProgram->setVertexShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/line.vert"}));
    shaderProgram->setFragmentShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/line.frag"}));

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

    auto *material = new Qt3DRender::QMaterial(this);
    material->addParameter(new Qt3DRender::QParameter("lineColor", color, this));
    material->setEffect(effect);

    //Build GeometryRenderer
    buffer_->setUsage(isStatic ? Qt3DRender::QBuffer::StaticDraw : Qt3DRender::QBuffer::DynamicDraw);
    attribute_->setName("vertexPosition");
    attribute_->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    attribute_->setVertexBaseType(Qt3DRender::QAttribute::Float);
    attribute_->setVertexSize(3);
    attribute_->setByteOffset(0);
    attribute_->setByteStride(3 * 4);
    attribute_->setCount(count_);
    attribute_->setBuffer(buffer_);

    geometry_->addAttribute(attribute_);

    geometryRenderer_->setInstanceCount(count_);
    geometryRenderer_->setPrimitiveType(Qt3DRender::QGeometryRenderer::PrimitiveType::LineStrip);
    geometryRenderer_->setGeometry(geometry_);

    this->addComponent(geometryRenderer_);
    this->addComponent(material);
}


void Line::setData(const QVector<QVector3D> &positions) {
    QByteArray ba;
    ba.resize(positions.size() * sizeof(VBOData));
    auto *vboData = reinterpret_cast<VBOData *>(ba.data());
    for (int i = 0; i < positions.size(); i++) {
        VBOData &vbo = vboData[i];
        vbo.position = positions[i];
    }

    buffer_->setData(ba);
    count_ = positions.count();
    attribute_->setCount(count_);
    geometryRenderer_->setInstanceCount(count_);
}

void Line::clearData() {

    buffer_->setData(QByteArray{});
    attribute_->setCount(0);
    geometryRenderer_->setInstanceCount(0);
}

void Line::appendData(const QVector3D position) {

    // Add space for one additional position
    QByteArray tempBuffer = buffer_->data();

    tempBuffer.resize(tempBuffer.size() + 1 * sizeof(VBOData));

    // View QByteArray as array of VBOData
    auto *vboData = reinterpret_cast<VBOData *>(tempBuffer.data());

    // Get a reference to last array element
    VBOData &vbo = vboData[count_];
    vbo.position = position;

    // Sets back buffer's data
    buffer_->setData(tempBuffer);

    count_ += 1;

    attribute_->setCount(count_);
    geometryRenderer_->setInstanceCount(count_);
}

void Line::appendData(const QVector<QVector3D> &positions) {
    QByteArray tempBuffer = buffer_->data();
    tempBuffer.resize(tempBuffer.size() + positions.size() * sizeof(VBOData));

    // View QByteArray as array of VBOData
    auto *vboData = reinterpret_cast<VBOData *>(tempBuffer.data());

    for (int i = 0; i < positions.size(); i++) {
        VBOData &vbo = vboData[count_ + i];
        vbo.position = positions[i];
    }

    // Sets back buffer's data
    buffer_->setData(tempBuffer);

    count_ += positions.size();
    attribute_->setCount(count_);
    geometryRenderer_->setInstanceCount(count_);
}