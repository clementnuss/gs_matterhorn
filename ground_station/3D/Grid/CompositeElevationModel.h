
#ifndef GS_MATTERHORN_COMPOSITEELEVATIONMODEL_H
#define GS_MATTERHORN_COMPOSITEELEVATIONMODEL_H


#include "DiscreteElevationModel.h"
#include "IDiscreteElevationModel.h"

class CompositeElevationModel : public IDiscreteElevationModel {
public:
    explicit CompositeElevationModel(const IDiscreteElevationModel *const d1, const IDiscreteElevationModel *const d2);

    float elevationAt(int latitudeIndex, int longitudeIndex) const override;

    Interval2D extent() const;

private:
    const IDiscreteElevationModel *dem1_;
    const IDiscreteElevationModel *dem2_;
    Interval2D extent_;
};


#endif //GS_MATTERHORN_COMPOSITEELEVATIONMODEL_H
