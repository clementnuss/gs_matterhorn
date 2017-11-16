#include <c++/iostream>
#include "TraceData.h"


using Qt3DRender::QBuffer;

TraceData::TraceData(Qt3DCore::QNode *parent)
        : Qt3DCore::QNode(parent), m_buffer(new QBuffer(QBuffer::VertexBuffer, this)) {
    QVector<QVector3D> positions{
            QVector3D{10.0f, 10.0, 0.0},
            QVector3D{10.0, 0.0, 0.0},
            QVector3D{0.0, 10.0, 0.0},
            QVector3D{0.0, 0.0, 0.0}};

    QByteArray ba;                   
    ba.resize(positions.size() * sizeof(TraceVBOData));
    TraceVBOData *vboData = reinterpret_cast<TraceVBOData *>(ba.data());
    for (int i = 0; i < positions.size(); i++) {
        TraceVBOData &vbo = vboData[i];
        vbo.position = positions[i];
        std::cout << vbo.position.length() << std::endl;
    }
    m_buffer->setData(ba);
    m_count = positions.count();
    std::cout << m_count << std::endl;
    emit countChanged(m_count);
}

Qt3DRender::QBuffer *TraceData::buffer() {
    return m_buffer.data();
}

void TraceData::setData(QVector<TraceVBOData> data) {
    QByteArray ba;
    ba.resize(data.size() * sizeof(TraceVBOData));
    TraceVBOData *posData = reinterpret_cast<TraceVBOData *>(ba.data());
    // TODO can we just set the address here? Instead of copying.
    for (const TraceVBOData &pos : data) {
        *posData++ = pos;
    }
    m_buffer->setData(ba);
    m_count = data.count();
    emit countChanged(m_count);
}

void TraceData::setData(QByteArray byteArray, int count) {
    m_buffer->setData(byteArray);
    m_count = count;
    emit countChanged(m_count);
}

void TraceData::setData(const QVector<QVector3D> &positions) {
    QByteArray ba;
    ba.resize(positions.size() * sizeof(TraceVBOData));
    TraceVBOData *vboData = reinterpret_cast<TraceVBOData *>(ba.data());
    for (int i = 0; i < positions.size(); i++) {
        TraceVBOData &vbo = vboData[i];
        vbo.position = positions[i];
    }
    m_buffer->setData(ba);
    m_count = positions.count();
    emit countChanged(m_count);
}

void TraceData::setPositions(QVector<QVector3D> positions) {
    QByteArray ba;
    ba.resize(positions.size() * sizeof(TraceVBOData));
    TraceVBOData *vboData = reinterpret_cast<TraceVBOData *>(ba.data());
    for (int i = 0; i < positions.size(); i++) {
        TraceVBOData &vbo = vboData[i];
        vbo.position = positions[i];
    }
    m_buffer->setData(ba);
    m_count = positions.count();
    emit countChanged(m_count);
}

int TraceData::count() const {
    return m_count;
}