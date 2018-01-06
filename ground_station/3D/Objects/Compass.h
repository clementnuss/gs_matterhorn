
#ifndef GS_MATTERHORN_COMPASS_H
#define GS_MATTERHORN_COMPASS_H


#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>

class Compass : public Qt3DCore::QEntity {
Q_OBJECT;

public:
    explicit Compass(Qt3DCore::QNode *parent, Qt3DRender::QCamera *cam);

public slots:

    void update();

private:
    Qt3DRender::QCamera *camera_;
    Qt3DCore::QTransform *transform_;
};


#endif //GS_MATTERHORN_COMPASS_H
