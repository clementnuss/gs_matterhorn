
#ifndef GS_MATTERHORN_OPENGL3DAXES_H
#define GS_MATTERHORN_OPENGL3DAXES_H

#include <Qt3DCore/QEntity>
#include <3D/Line/Line.h>

/**
 * An entity that represent the internal OpenGL axis system with a set of three orthogonal lines
 *
 * Red line for x axis
 * Green line for y axis
 * Blue line for z axis
 */
class OpenGL3DAxes : public Qt3DCore::QEntity {
Q_OBJECT;

public:
    explicit OpenGL3DAxes(Qt3DCore::QNode *parent);

private:
    Line *xAxis_;
    Line *yAxis_;
    Line *zAxis_;

    const float AXIS_SIZE = 200;
    const float HEIGHT = 100;
};


#endif //GS_MATTERHORN_OPENGL3DAXES_H
