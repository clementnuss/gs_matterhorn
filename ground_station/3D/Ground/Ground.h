
#ifndef GS_MATTERHORN_GROUND_H
#define GS_MATTERHORN_GROUND_H

#include <Qt3DCore/QNode>
#include <3D/Scene/WorldReference.h>
#include "HighlightArea.h"
#include "3D/CoordinateUtils.h"
#include "3D/Grid/ContinuousElevationModel.h"

class Ground : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Ground(Qt3DCore::QNode *parent, const QVector2D &offset, const LatLon &topLeftLatLon,
                    const ContinuousElevationModel *model, const WorldReference *const worldRef);

    void highlightArea(const QVector2D &areaCenter);

public slots:

private:
    Qt3DCore::QTransform *transform_;
    HighlightArea *highlightedArea_;
};


#endif //GS_MATTERHORN_GROUND_H
