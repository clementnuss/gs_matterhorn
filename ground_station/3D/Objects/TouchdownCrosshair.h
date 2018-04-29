
#ifndef GS_MATTERHORN_TOUCHDOWNCROSSHAIR_H
#define GS_MATTERHORN_TOUCHDOWNCROSSHAIR_H


#include <QtCore/QArgument>
#include <QtGui/QVector3D>
#include <Qt3DRender/QCamera>
#include <3D/Billboards/Text3D.h>
#include <3D/Billboards/Tracker.h>
#include <3D/Billboards/FlatBillboard.h>

class TouchdownCrosshair : public Qt3DCore::QEntity, public IObservable {
Q_OBJECT

public:
    explicit TouchdownCrosshair(const QVector3D &position, const Qt3DRender::QCamera *const camera, Qt3DCore::QNode *const parent);

public slots:

    void updatePosition(QVector3D pos);

    QVector3D getPosition();

    Qt3DCore::QTransform *getObjectTransform() const override;

private:
    FlatBillboard *marker_;
    Text3D *text_;

    Qt3DCore::QTransform *transform_;
    QVector3D textOffset_;
    QVector3D markerOffset_;
};


#endif //GS_MATTERHORN_TOUCHDOWNCROSSHAIR_H
