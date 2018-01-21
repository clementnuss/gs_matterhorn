
#include <stdexcept>
#include "Ground.h"


Ground::Ground(Qt3DCore::QNode *parent,
               const ContinuousElevationModel *model,
               const WorldReference *const worldRef,
               int sideLength) :
        sideLength_{(sideLength / 2) * 2}, // Enforce side length to be even
        tiles_{} {

    if (sideLength_ <= 0) {
        throw std::invalid_argument("The side length in unit of tiles should be even and at least 2");
    }


    for (int i = -sideLength_ / 2; i < sideLength_ / 2; i++) {

        for (int j = -sideLength_ / 2; j < sideLength_ / 2; j++) {

            float dLat = static_cast<float>(-i * GridConstants::GRID_LENGTH_IN_METERS);
            float dLon = static_cast<float>(j * GridConstants::GRID_LENGTH_IN_METERS);
            //Compute corresponding top-left latitude and longitude coordinates
            LatLon tileOrigin = worldRef->latLonFromPointAndDistance(
                    worldRef->origin(),
                    dLat,
                    dLon
            );

            //Compute world offset
            QVector2D tileOffset{dLat, dLon};

            tiles_.emplace_back(std::make_unique<GroundTile>(parent, tileOffset, tileOrigin, model, worldRef));
        }
    }

}
