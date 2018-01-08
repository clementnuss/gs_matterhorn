#include <iostream>
#include <boost/filesystem.hpp>
#include <3D/3DVisualisationConstants.h>
#include "DiscreteElevationModel.h"

DiscreteElevationModel::DiscreteElevationModel(std::string &filePath, GeoPoint &topLeftGeoPoint) {

    int numberOfElements =
            (DiscreteElevationModel::MODEL_RESOLUTION + 1) * (DiscreteElevationModel::MODEL_RESOLUTION + 1);
    int numberOfBytes = numberOfElements * DiscreteElevationModel::BYTES_PER_MEASURE;

    boost::iostreams::mapped_file_source file;

    if (boost::filesystem::exists(filePath)) {
        if (boost::filesystem::is_regular_file((filePath))) {

            file.open(filePath);
            hgtData_ = reinterpret_cast<const int16_t *>(file.data());

        } else {
            throw (std::invalid_argument(filePath + " exists, but is not a regular file or directory\n"));
        }
    } else {
        throw (std::invalid_argument(filePath + " does not exist\n"));
    }

    if (file.size() != numberOfBytes) {
        throw std::length_error("The specified file does not contain the correct number of bytes");
    }

    int minX = topLeftGeoPoint.longitude.degrees * GridConstants::GRID_RESOLUTION;
    int minY = topLeftGeoPoint.latitude.degrees * GridConstants::GRID_RESOLUTION;

    extent_ = Interval2D{
            boost::icl::construct<boost::icl::discrete_interval<int>>(minX, minX + GridConstants::GRID_RESOLUTION),
            boost::icl::construct<boost::icl::discrete_interval<int>>(minY, minY + GridConstants::GRID_RESOLUTION)};
}

float DiscreteElevationModel::elevationAt(int x, int y) const {

    if (!extent_.contains(x, y)) {
        throw std::invalid_argument("Specified indices are not contained in the discrete elevation model");
    }

    int xL = x - extent_.fromX();
    int yL = -(y - extent_.toY());
    return hgtData_[yL * (GridConstants::GRID_RESOLUTION + 1) + xL];
}

bool DiscreteElevationModel::contains(int x, int y) const {
    return extent_.contains(x, y);
}

int DiscreteElevationModel::geoAngleToIndex(const GeoAngle &geoAngle) {
    return geoAngle.degrees * GridConstants::GRID_RESOLUTION + geoAngle.minutes * 60 + geoAngle.seconds;
}