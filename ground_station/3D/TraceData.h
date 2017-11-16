
#ifndef GS_MATTERHORN_TRACEDATA_H
#define GS_MATTERHORN_TRACEDATA_H


#include <QObject>
#include <Qt3DRender/QBuffer>
#include <Qt3DCore/QNode>
#include <QVector3D>

struct TraceVBOData {
    QVector3D position;
};

class TraceData : public Qt3DCore::QNode {
Q_OBJECT
    Q_PROPERTY(Qt3DRender::QBuffer *buffer
                       READ
                       buffer
                       CONSTANT)
    Q_PROPERTY(int count
                       READ
                       count
                       NOTIFY
                       countChanged)
public:
    explicit TraceData(Qt3DCore::QNode *parent = 0);

    Qt3DRender::QBuffer *buffer();

    void setData(QVector<TraceVBOData> data);

    void setData(const QVector<QVector3D> &positions);

    void setData(QByteArray byteArray, int count);

    void setPositions(QVector<QVector3D> positions);

    int count() const;

signals:

    void countChanged(int count);

public slots:

private:
    QScopedPointer<Qt3DRender::QBuffer> m_buffer;
    int m_count = 0;
};


#endif //GS_MATTERHORN_TRACEDATA_H
