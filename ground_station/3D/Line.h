
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
    explicit Line(Qt3DCore::QNode *parent = 0);

    void setData(const QVector<QVector3D> &positions);

    void appendData(const QVector3D position);

public slots:

private:
    Qt3DExtras::QCuboidMesh mesh_;
    Qt3DRender::QMaterial material_;
    Qt3DRender::QGeometryRenderer geometryRenderer_;
    Qt3DRender::QGeometry geometry_;
    Qt3DRender::QAttribute attribute_;
    Qt3DRender::QBuffer buffer_;
    Qt3DRender::QEffect effect_;
    Qt3DRender::QTechnique technique_;
    Qt3DRender::QFilterKey filterKey_;
    Qt3DRender::QRenderPass renderPass_;
    Qt3DRender::QShaderProgram shaderProgram_;
    Qt3DRender::QLineWidth renderState_;
    int count_;
};


#endif //GS_MATTERHORN_LINE_H
