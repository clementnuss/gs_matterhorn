
#ifndef GS_MATTERHORN_GROUND_H
#define GS_MATTERHORN_GROUND_H


#include <QObject>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <Qt3DCore/QNode>

class Ground : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Ground(Qt3DCore::QNode *parent = nullptr);

public slots:

private:
    Qt3DCore::QTransform *transform_;
};


#endif //GS_MATTERHORN_GROUND_H
