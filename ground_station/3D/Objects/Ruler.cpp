
#include <ProgramConstants.h>
#include <Qt3DCore/QTransform>
#include <3D/Billboards/Tracker.h>
#include <3D/ForwardRenderer/LayerManager.h>
#include <3D/3DVisualisationConstants.h>
#include "Ruler.h"

Ruler::Ruler(float originHeight, QVector3D measurePos, Qt3DRender::QCamera *camera, const QString &tickTexture, Qt3DCore::QNode *parent) :
        QEntity(parent),
        originHeight_{originHeight},
        transform_{new Qt3DCore::QTransform()},
        measurePos_{measurePos},
        ticksVector_{},
        tickLabels_{},
        rulerAxis_{new Line(this, QColor::fromRgb(255, 255, 255))} {

    rulerAxis_->appendData({0, measurePos_.y(), 0});
    rulerAxis_->appendData({0, originHeight_, 0});
    transform_->setTranslation({measurePos_.x(), 0, measurePos_.z()});

    this->addComponent(transform_);

    initLabels(camera, tickTexture);
    redraw();
}


void
Ruler::updatePosition(const QVector3D &newPos) {
    measurePos_ = newPos;
    transform_->setTranslation({measurePos_.x(), 0, measurePos_.z()});
    redraw();
}


void
Ruler::redraw() {
    rulerAxis_->setData({{0, measurePos_.y(), 0},
                         {0, originHeight_,   0}});

    for (auto *tracker : tickLabels_) {
        if (tracker->getPosition().y() < measurePos_.y()) {
            tracker->removeComponent(LayerManager::getInstance().getLayer(LayerType::INVISIBLE));
        } else {
            tracker->addComponent(LayerManager::getInstance().getLayer(LayerType::INVISIBLE));
        }
    }

}

void
Ruler::initLabels(Qt3DRender::QCamera *camera, const QString &tickTexture) {

    for (int i = OpenGLConstants::RULER_SPACING + static_cast<int>(originHeight_);
         i <= OpenGLConstants::RULER_MAX_HEIGHT + static_cast<int>(originHeight_);
         i += OpenGLConstants::RULER_SPACING) {

        if (i < originHeight_) {
            continue;
        }

        tickLabels_.push_back(
                new Tracker({0, static_cast<float>(i), 0}, camera, tickTexture,
                            QString::number(i - originHeight_) + QStringLiteral(" m"), TextType::BOLD,
                            this,
                            OpenGLConstants::RIGHT_TICK, OpenGLConstants::RIGHT_LABEL)
        );
    }
}
