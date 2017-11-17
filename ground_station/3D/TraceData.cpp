
#include "TraceData.h"


using Qt3DRender::QBuffer;

TraceData::TraceData(Qt3DCore::QNode *parent)
        : Qt3DCore::QNode(parent), m_buffer(new QBuffer(QBuffer::VertexBuffer, this)) {
}

Qt3DRender::QBuffer *TraceData::buffer() {
    return m_buffer.data();
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

void TraceData::appendData(const QVector3D position) {
    // Add space for one additional position

    QByteArray tempBuffer = m_buffer->data();

    tempBuffer.resize(tempBuffer.size() + 1 * sizeof(TraceVBOData));

    // View QByteArray as array of TraceVBOData
    TraceVBOData *vboData = reinterpret_cast<TraceVBOData *>(tempBuffer.data());

    // Get a reference to last array element
    TraceVBOData &vbo = vboData[m_count];
    vbo.position = position;

    // Sets back buffer's data
    m_buffer->setData(tempBuffer);

    m_count += 1;
    emit countChanged(m_count);
}

int TraceData::count() const {
    return m_count;
}