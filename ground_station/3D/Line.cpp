#include <iostream>
#include "Line.h"


Line::Line(Qt3DCore::QNode *parent) : Qt3DCore::QEntity(parent), mesh_{this}, material_{this}, effect_{this},
                                      geometryRenderer_{this}, geometry_{&geometryRenderer_}, attribute_{&geometry_},
                                      buffer_{},
                                      technique_{&effect_}, filterKey_{&technique_}, renderPass_{&technique_},
                                      shaderProgram_{&renderPass_}, renderState_{&renderPass_}, count_{0} {

    // Build effect
    renderState_.setValue(5.0);

    shaderProgram_.setVertexShaderCode(shaderProgram_.loadSource(QUrl{"qrc:/shaders/line.vert"}));
    shaderProgram_.setFragmentShaderCode(shaderProgram_.loadSource(QUrl{"qrc:/shaders/line.frag"}));

    renderPass_.setShaderProgram(&shaderProgram_);
    renderPass_.addRenderState(&renderState_);

    filterKey_.setName("renderingStyle");
    filterKey_.setValue("forward");

    technique_.graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique_.graphicsApiFilter()->setMajorVersion(3);
    technique_.graphicsApiFilter()->setMinorVersion(2);
    technique_.graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    technique_.addFilterKey(&filterKey_);
    technique_.addRenderPass(&renderPass_);

    effect_.addTechnique(&technique_);

    material_.setEffect(&effect_);
    //Build GeometryRenderer

    attribute_.setName("vertexPosition");
    attribute_.setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    attribute_.setVertexBaseType(Qt3DRender::QAttribute::Float);
    attribute_.setVertexSize(3);
    attribute_.setByteOffset(0);
    attribute_.setByteStride(3 * 4);
    attribute_.setCount(count_);
    attribute_.setBuffer(&buffer_);

    geometry_.addAttribute(&attribute_);

    geometryRenderer_.setInstanceCount(count_);
    geometryRenderer_.setPrimitiveType(Qt3DRender::QGeometryRenderer::PrimitiveType::LineStrip);
    geometryRenderer_.setGeometry(&geometry_);

    this->addComponent(&geometryRenderer_);
    this->addComponent(&material_);
}


void Line::setData(const QVector<QVector3D> &positions) {
    QByteArray ba;
    ba.resize(positions.size() * sizeof(VBOData));
    VBOData *vboData = reinterpret_cast<VBOData *>(ba.data());
    for (int i = 0; i < positions.size(); i++) {
        VBOData &vbo = vboData[i];
        vbo.position = positions[i];
    }

    buffer_.setData(ba);
    count_ = positions.count();
    attribute_.setCount(count_);
    geometryRenderer_.setInstanceCount(count_);
}

void Line::clearData() {

    buffer_.setData(QByteArray{});
    attribute_.setCount(0);
    geometryRenderer_.setInstanceCount(0);
}

void Line::appendData(const QVector3D position) {
    // Add space for one additional position

    QByteArray tempBuffer = buffer_.data();

    tempBuffer.resize(tempBuffer.size() + 1 * sizeof(VBOData));

    // View QByteArray as array of VBOData
    VBOData *vboData = reinterpret_cast<VBOData *>(tempBuffer.data());

    // Get a reference to last array element
    VBOData &vbo = vboData[count_];
    vbo.position = position;

    // Sets back buffer's data
    buffer_.setData(tempBuffer);

    count_ += 1;

    attribute_.setCount(count_);
    geometryRenderer_.setInstanceCount(count_);
}

void Line::appendData(const QVector<QVector3D> &positions) {
    // TODO: do not call append data individually, resize needed each time
    for (auto position : positions) {
        appendData(position);
    }
}