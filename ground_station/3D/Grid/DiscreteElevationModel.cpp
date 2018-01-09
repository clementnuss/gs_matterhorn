#include <iostream>
#include <boost/filesystem.hpp>
#include <3D/3DVisualisationConstants.h>
#include "DiscreteElevationModel.h"

DiscreteElevationModel::DiscreteElevationModel(std::string &filePath, GeoPoint &topLeftGeoPoint) {

    int numberOfElements =
            (DiscreteElevationModel::MODEL_RESOLUTION + 1) * (DiscreteElevationModel::MODEL_RESOLUTION + 1);
    int numberOfBytes = numberOfElements * DiscreteElevationModel::BYTES_PER_MEASURE;


    if (boost::filesystem::exists(filePath)) {
        if (boost::filesystem::is_regular_file((filePath))) {

            hgtFile_.open(filePath);
            hgtData_ = reinterpret_cast<const uint16_t *>(hgtFile_.data());
            uint16_t v = hgtData_[0] >> 8 | hgtData_[0] << 8;
            std::cout << v << " ";

/*
            for(int i=0; i < numberOfElements; i++){
                uint16_t v = hgtData_[i]>>8 | hgtData_[i]<<8;
                std::cout << v << " ";
            }
*/

        } else {
            throw (std::invalid_argument(filePath + " exists, but is not a regular file or directory\n"));
        }
    } else {
        throw (std::invalid_argument(filePath + " does not exist\n"));
    }

    if (hgtFile_.size() != numberOfBytes) {
        throw std::length_error("The specified file does not contain the correct number of bytes");
    }

    int minLongitudeIndex = topLeftGeoPoint.longitude.degrees * GridConstants::SAMPLES_PER_DEGREE;
    int minLatitudeIndex = topLeftGeoPoint.latitude.degrees * GridConstants::SAMPLES_PER_DEGREE;

    extent_ = Interval2D{
            boost::icl::construct<boost::icl::discrete_interval<int>>(minLatitudeIndex, minLatitudeIndex +
                                                                                        GridConstants::SAMPLES_PER_DEGREE),
            boost::icl::construct<boost::icl::discrete_interval<int>>(minLongitudeIndex, minLongitudeIndex +
                                                                                         GridConstants::SAMPLES_PER_DEGREE)
    };
}

float DiscreteElevationModel::elevationAt(int latitudeIndex, int longitudeIndex) const {

    if (!extent_.contains(latitudeIndex, longitudeIndex)) {
        throw std::invalid_argument("Specified indices are not contained in the discrete elevation model");
    }

    int xL = longitudeIndex - extent_.startLongitude();
    int yL = -(latitudeIndex - extent_.endLatitude());

    int i = yL * (GridConstants::SAMPLES_PER_DEGREE + 1) + xL;
    uint16_t d = (hgtData_[i] >> 8 | hgtData_[i] << 8);  // Swap byte order
    std::cout << d << std::endl;
    return d;
}

bool DiscreteElevationModel::contains(int latitudeIndex, int longitudeIndex) const {
    return extent_.contains(latitudeIndex, longitudeIndex);
}

int DiscreteElevationModel::geoAngleToIndex(const GeoAngle &geoAngle) {
    return geoAngle.degrees * GridConstants::SAMPLES_PER_DEGREE + geoAngle.minutes * GridConstants::SAMPLES_PER_MINUTE +
           geoAngle.seconds;
}