
#include <stdexcept>
#include "Ground.h"


Ground::Ground(Qt3DCore::QNode *parent,
               const ContinuousElevationModel *model,
               const WorldReference *const worldRef,
               int sideLength) :
        sideLength_{(sideLength / 2) * 2}, // Enforce side length to be even
        halfSideLength_{sideLength / 2},
        extentFromOrigin_{GridConstants::GRID_LENGTH_IN_METERS * sideLength_ / 2},
        tiles_{} {

    if (sideLength_ <= 0) {
        throw std::invalid_argument("The side length in unit of tiles should be even and at least 2");
    }


    for (int i = -halfSideLength_; i < halfSideLength_; i++) {

        for (int j = -halfSideLength_; j < halfSideLength_; j++) {

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

void Ground::groundElevationAt(const QVector2D &regionCenter) {


    int x = static_cast<int>(std::round(regionCenter.x()));
    int y = static_cast<int>(std::round(regionCenter.y()));
    if (isBetween(-extentFromOrigin_, extentFromOrigin_, x)
        &&
        isBetween(-extentFromOrigin_, extentFromOrigin_, y)) {
        // Switch coordinate system -> go from origin in the middle of the terrain to origin on top left
        int newx = x + halfSideLength_ * GridConstants::GRID_LENGTH_IN_METERS;
        int newy = halfSideLength_ * GridConstants::GRID_LENGTH_IN_METERS - y;

        //Find the correct tile to sample
        int j = newx / GridConstants::GRID_LENGTH_IN_METERS + halfSideLength_;
        int i = newy / GridConstants::GRID_LENGTH_IN_METERS + halfSideLength_;

        int tileIndex = (i * sideLength_) + j;

        //Find the coordinate within the tile
        int vx = x - j * GridConstants::GRID_LENGTH_IN_METERS;
        int vy = y - i * GridConstants::GRID_LENGTH_IN_METERS;

        tiles_[tileIndex].get()->vertexHeightAt(vx, vy);
    }
}
