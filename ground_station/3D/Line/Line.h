
#ifndef GS_MATTERHORN_LINE_H
#define GS_MATTERHORN_LINE_H

#include <QObject>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <Qt3DCore/QNode>


struct VBOData {
    QVector3D position;
};

class Line : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Line(Qt3DCore::QNode *parent, QColor &&col, bool isStatic = false);

    void setData(const QVector<QVector3D> &positions);

    void appendData(const QVector3D position);

    void appendData(const QVector<QVector3D> &positions);

    void clearData();

public slots:

private:
    void enforceLineResolution();

    Qt3DRender::QGeometryRenderer *geometryRenderer_;
    Qt3DRender::QGeometry *geometry_;
    Qt3DRender::QAttribute *attribute_;
    Qt3DRender::QBuffer *drawBuffer_;
    QVector<QVector3D> lineData_;
    int drawBufferCount_;
};


#endif //GS_MATTERHORN_LINE_H
