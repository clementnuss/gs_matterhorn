#include "Line.h"


Line::Line(Qt3DCore::QNode *parent) : mesh_{}, material_{} {
    this->addComponent(&mesh_);
    this->addComponent(&material_);
}