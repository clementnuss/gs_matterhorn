
#include <Utilities/ExceptionUtils.h>
#include "CompositeElevationModel.h"

CompositeElevationModel::CompositeElevationModel(
        std::unique_ptr<const IDiscreteElevationModel> d1,
        std::unique_ptr<const IDiscreteElevationModel> d2) :
        dem1_{std::move(d1)}, dem2_{std::move(d2)}, extent_{} {

    requireNonNull(dem1_);
    requireNonNull(dem2_);

    extent_ = dem1_->extent().unionWith(dem2_->extent());
}

Interval2D CompositeElevationModel::extent() const {
    return extent_;
}

float CompositeElevationModel::elevationAt(int latitudeIndex, int longitudeIndex) const {
    if (!extent_.contains(latitudeIndex, longitudeIndex)) {
        throw std::invalid_argument("The specified latitude and/or longitude "
                                            "index is not contained in this composite elevation model");
    }
    return (dem1_->extent().contains(latitudeIndex, longitudeIndex) ? dem1_ : dem2_)->elevationAt(latitudeIndex,
                                                                                                  longitudeIndex);
}