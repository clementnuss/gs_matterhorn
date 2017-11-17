
#ifndef GS_MATTERHORN_LINE_H
#define GS_MATTERHORN_LINE_H

#include <QObject>
#include <Qt3DExtras>
#include <Qt3DRender/QBuffer>
#include <Qt3DCore/QNode>

class Line : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Line(Qt3DCore::QNode *parent = 0);

public slots:

private:
    Qt3DExtras::QCuboidMesh mesh_;
    Qt3DExtras::QPhongMaterial material_;
};


#endif //GS_MATTERHORN_LINE_H
