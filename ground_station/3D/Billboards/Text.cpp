#include <Qt3DExtras>
#include <3D/Utils.h>
#include "Text.h"

const QVector3D Text::basePosition_{0, 0, 0};

Text::Text(QString text, QVector3D offset, Qt3DRender::QCamera *camera, Qt3DCore::QNode *parent) :
        Qt3DCore::QEntity(parent),
        camera_{camera},
        offset_{offset},
        transform_{new Qt3DCore::QTransform(this)},
        font_{} {

    // Set up mesh
    font_.setPointSizeF(0.8);
    font_.setBold(true);
    auto textMetrics = QFontMetricsF(font_);
    QRectF rect = textMetrics.boundingRect(text);
    qreal margin = textMetrics.width('_');
    rect.adjust(-margin, margin, margin, margin);

    auto *textEntity = new Qt3DExtras::QText2DEntity(this);
    textEntity->setText(text);
    textEntity->setWidth(rect.width());
    textEntity->setHeight(rect.height());
    textEntity->setFont(font_);


    // Set up transform
    updateTransform();

    this->addComponent(transform_);

    connect(camera, &Qt3DRender::QCamera::viewMatrixChanged,
            this, &Text::updateTransform);

}

void Text::updateTransform() {
    QMatrix4x4 t{};
    t.translate(offset_);
    // Make it face camera since default is like laying flat on ground
    transform_->setMatrix(billboardMV(basePosition_, camera_->viewMatrix()) * t);
}

