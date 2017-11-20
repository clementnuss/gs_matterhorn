
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <3D/Utils.h>
#include <3D/Marker/Marker.h>
#include "Text.h"

Text::Text(QString text, QVector3D position, Qt3DRender::QCamera *camera, Qt3DCore::QNode *parent) :
        Qt3DCore::QEntity(parent),
        camera_{camera},
        position_{position},
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

    QMatrix4x4 m{};
    m.rotate(180, QVector3D(0, 1, 0));
    transform_->setMatrix(m);

    // Set up transform
    updateTransform();

    this->addComponent(transform_);

    connect(camera, &Qt3DRender::QCamera::viewMatrixChanged,
            this, &Text::updateTransform);

    //new Marker(QUrl{"qrc:/3D/textures/caret_down.png"}, position + QVector3D(5,0,0), camera, this);

}

void Text::updateTransform() {
    QMatrix4x4 t{};
    // Make it face camera since default is like laying flat on ground
    transform_->setMatrix(billboardMV(position_, camera_->viewMatrix()) * t);
}
