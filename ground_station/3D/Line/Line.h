
#ifndef GS_MATTERHORN_LINE_H
#define GS_MATTERHORN_LINE_H


#include <Qt3DCore/QNode>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <QVector3D>
#include <QColor>


struct VBOData {
    QVector3D position;
};

class Line : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Line(Qt3DCore::QNode *const parent, const QColor &&col, const bool &isStatic = false);

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
