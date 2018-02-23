
#include <stdexcept>
#include <iostream>
#include "Ground.h"


Ground::Ground(Qt3DCore::QNode *parent,
               std::shared_ptr<const ContinuousElevationModel> model,
               std::shared_ptr<const WorldReference> worldRef,
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

    std::cout << tiles_.size() << std::endl;

}

float Ground::groundElevationAt(int worldX, int worldZ) const {

    if (isBetween(-extentFromOrigin_, extentFromOrigin_, worldX)
        &&
        isBetween(-extentFromOrigin_, extentFromOrigin_, worldZ)) {
        // Switch coordinate system -> go from origin in the middle of the terrain to origin on top left
        int horPos = worldZ + halfSideLength_ * GridConstants::GRID_LENGTH_IN_METERS;
        int vertPos = halfSideLength_ * GridConstants::GRID_LENGTH_IN_METERS - worldX;

        //Find the correct tile to sample
        int j = horPos / GridConstants::GRID_LENGTH_IN_METERS;
        int i = vertPos / GridConstants::GRID_LENGTH_IN_METERS;

        int tileIndex = (i * sideLength_) + j;

        //Find the coordinate within the tile
        int vx = horPos - j * GridConstants::GRID_LENGTH_IN_METERS;
        int vy = vertPos - i * GridConstants::GRID_LENGTH_IN_METERS;

        //Find the vertex index
        int vi = vx / (GridConstants::GRID_RESOLUTION - 1);
        int vj = vy / (GridConstants::GRID_RESOLUTION - 1);

        return tiles_[tileIndex].get()->vertexHeightAt(vi, vj);
    } else {
        return 0.0f;
    }
}
