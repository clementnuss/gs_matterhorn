#include <iostream>
#include <ProgramConstants.h>
#include <3D/ForwardRenderer/LayerManager.h>
#include "Line.h"


Line::Line(Qt3DCore::QNode *parent, QColor &&color, bool isStatic) : Qt3DCore::QEntity(parent),
                                                                     geometryRenderer_{
                                                                             new Qt3DRender::QGeometryRenderer()},
                                                                     geometry_{new Qt3DRender::QGeometry()},
                                                                     attribute_{new Qt3DRender::QAttribute()},
                                                                     drawBuffer_{new Qt3DRender::QBuffer(
                                                                             Qt3DRender::QBuffer::VertexBuffer)},
                                                                     drawBufferCount_{0},
                                                                     lineData_{} {

    // Build effect
    auto *shaderProgram = new Qt3DRender::QShaderProgram();
    shaderProgram->setVertexShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/line.vert"}));
    shaderProgram->setFragmentShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/line.frag"}));

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

    auto *material = new Qt3DRender::QMaterial();
    material->addParameter(new Qt3DRender::QParameter("lineColor", color));
    material->setEffect(effect);

    //Build GeometryRenderer
    drawBuffer_->setUsage(isStatic ? Qt3DRender::QBuffer::StaticDraw : Qt3DRender::QBuffer::DynamicDraw);
    attribute_->setName("vertexPosition");
    attribute_->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    attribute_->setVertexBaseType(Qt3DRender::QAttribute::Float);
    attribute_->setVertexSize(3);
    attribute_->setByteOffset(0);
    attribute_->setByteStride(3 * 4);
    attribute_->setCount(drawBufferCount_);
    attribute_->setBuffer(drawBuffer_);

    geometry_->addAttribute(attribute_);

    geometryRenderer_->setInstanceCount(drawBufferCount_);
    geometryRenderer_->setPrimitiveType(Qt3DRender::QGeometryRenderer::PrimitiveType::LineStrip);
    geometryRenderer_->setGeometry(geometry_);

    this->addComponent(geometryRenderer_);
    this->addComponent(material);
    this->addComponent(LayerManager::getInstance().getLayer(LayerType::VISIBLE));
}


void Line::setData(const QVector<QVector3D> &positions) {
    QByteArray ba;
    ba.resize(positions.size() * sizeof(VBOData));
    auto *vboData = reinterpret_cast<VBOData *>(ba.data());
    for (int i = 0; i < positions.size(); i++) {
        VBOData &vbo = vboData[i];
        vbo.position = positions[i];
    }

    drawBuffer_->setData(ba);
    drawBufferCount_ = positions.count();
    attribute_->setCount(drawBufferCount_);
    geometryRenderer_->setInstanceCount(drawBufferCount_);

    enforceLineResolution();
}

void Line::clearData() {

    drawBuffer_->setData(QByteArray{});
    attribute_->setCount(0);
    geometryRenderer_->setInstanceCount(0);
}

void Line::appendData(const QVector3D position) {

    lineData_.push_back(position);

    // Add space for one additional position
    QByteArray tempBuffer = drawBuffer_->data();

    tempBuffer.resize(tempBuffer.size() + 1 * sizeof(VBOData));

    // View QByteArray as array of VBOData
    auto *vboData = reinterpret_cast<VBOData *>(tempBuffer.data());

    // Get a reference to last array element
    VBOData &vbo = vboData[drawBufferCount_];
    vbo.position = position;

    // Sets back buffer's data
    drawBuffer_->setData(tempBuffer);

    drawBufferCount_ += 1;

    attribute_->setCount(drawBufferCount_);
    geometryRenderer_->setInstanceCount(drawBufferCount_);

    enforceLineResolution();
}

void Line::appendData(const QVector<QVector3D> &positions) {

    lineData_ += positions;

    QByteArray tempBuffer = drawBuffer_->data();
    tempBuffer.resize(tempBuffer.size() + positions.size() * sizeof(VBOData));

    // View QByteArray as array of VBOData
    auto *vboData = reinterpret_cast<VBOData *>(tempBuffer.data());

    for (int i = 0; i < positions.size(); i++) {
        VBOData &vbo = vboData[drawBufferCount_ + i];
        vbo.position = positions[i];
    }

    // Sets back buffer's data
    drawBuffer_->setData(tempBuffer);

    drawBufferCount_ += positions.size();
    attribute_->setCount(drawBufferCount_);
    geometryRenderer_->setInstanceCount(drawBufferCount_);

    enforceLineResolution();
}


void Line::enforceLineResolution() {

    if (lineData_.size() > OpenGLConstants::MAX_LINE_DATAPOINTS) {
        lineData_.remove(0, lineData_.size() - OpenGLConstants::MAX_LINE_DATAPOINTS);
    }

    if (drawBufferCount_ > OpenGLConstants::MAX_LINE_RESOLUTION) {

        QVector<QVector3D> selectedData{OpenGLConstants::MAX_LINE_RESOLUTION};

        double stepSize = (lineData_.size() - 1) / (static_cast<double>(OpenGLConstants::MAX_LINE_RESOLUTION) - 1);
        double selectionIndex = 0;

        for (int i = 0; i < OpenGLConstants::MAX_LINE_RESOLUTION; i++) {
            selectedData[i] = lineData_[static_cast<int>(selectionIndex + 0.5)];
            selectionIndex += stepSize;
        }

        this->setData(selectedData);
    }
}