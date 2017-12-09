
#include <QtGui/QColor>
#include "OpenGL3DAxes.h"

OpenGL3DAxes::OpenGL3DAxes(Qt3DCore::QNode *parent) :
        QEntity(parent),
        xAxis_{new Line(this, QColor::fromRgb(255, 0, 0), true)},
        yAxis_{new Line(this, QColor::fromRgb(0, 255, 0), true)},
        zAxis_{new Line(this, QColor::fromRgb(0, 0, 255), true)} {

    xAxis_->appendData({0, HEIGHT, 0});
    xAxis_->appendData({AXIS_SIZE, HEIGHT, 0});

    yAxis_->appendData({0, HEIGHT, 0});
    yAxis_->appendData({0, HEIGHT + AXIS_SIZE, 0});

    zAxis_->appendData({0, HEIGHT, 0});
    zAxis_->appendData({0, HEIGHT, AXIS_SIZE});
}
