
#include <ProgramConstants.h>
#include <3D/Billboards/Tracker.h>
#include <3D/ForwardRenderer/LayerManager.h>
#include "Ruler.h"

Ruler::Ruler(QVector3D &measurePos, Qt3DRender::QCamera *camera, Qt3DRender::QTexture2D *tickTexture,
             Qt3DCore::QNode *parent) :
        QEntity(parent),
        transform_{new Qt3DCore::QTransform()},
        measurePos_{measurePos},
        ticksVector_{},
        tickLabels_{},
        rulerAxis_{new Line(this, QColor::fromRgb(255, 255, 255))} {

    rulerAxis_->appendData({0, measurePos_.y(), 0});
    rulerAxis_->appendData({0, 0, 0});
    transform_->setTranslation({measurePos_.x(), 0, measurePos_.z()});

    this->addComponent(transform_);
    this->addComponent(LayerManager::getInstance().getLayer(LayerType::VISIBLE));

    initLabels(camera, tickTexture);
    redraw();
}


void Ruler::updatePosition(const QVector3D &newPos) {
    measurePos_ = newPos;
    transform_->setTranslation({measurePos_.x(), 0, measurePos_.z()});
    redraw();
}


void Ruler::redraw() {

    for (auto *tracker : tickLabels_) {
        if (tracker->getPosition().y() < measurePos_.y()) {
            tracker->setVisible();
        } else {
            tracker->setInvisible();
        }
    }

}

void Ruler::initLabels(Qt3DRender::QCamera *camera, Qt3DRender::QTexture2D *tickTexture) {

    for (int i = OpenGLConstants::RULER_SPACING;
         i <= OpenGLConstants::RULER_MAX_HEIGHT;
         i += OpenGLConstants::RULER_SPACING) {
        tickLabels_.push_back(
                new Tracker({0, i, 0}, camera, tickTexture, QString::number(i),
                            this, OpenGLConstants::RIGHT_1, OpenGLConstants::RIGHT_4)
        );
    }
}
