
#include <ProgramConstants.h>
#include <DataStructures/datastructs.h>
#include <FileReader.h>
#include <Utilities/ReaderUtils.h>
#include <3D/Ground/Ground.h>
#include "SplashDownPredictor.h"


SplashDownPredictor::SplashDownPredictor(std::string &path, Qt3DCore::QNode *parent) :
        predictionStrategy_{std::make_unique<NaivePredictionStrategy>()},
        status_{},
        trajectory_{},
        trajectoryLine_{new Line(parent, QColor::fromRgb(255, 153, 0))},
        windData_{WindData::fromFile(path)} {
    status_.position = {0, 0, 0};
    status_.speed = {0, 0, 0};
    status_.acceleration = {0, 0, 0};
}

QVector2D SplashDownPredictor::getTouchdownCoordinates(const Ground *const ground) const {

    for (const QVector3D *it = trajectory_.end() - 1; it != trajectory_.begin(); it--) {
        float x = it->x();
        float z = it->z();

        float elev = ground->groundElevationAt(
                static_cast<int>(std::round(x)),
                static_cast<int>(std::round(z))
        );

        if (elev < it->y()) {
            return {x, z};
        }
    }

    return {trajectory_.first().x(), trajectory_.first().z()};
}

void SplashDownPredictor::updatePos(const QVector3D &pos) {
    status_.position = pos;
    this->recomputePrediction();
}

void SplashDownPredictor::recomputePrediction() {
    trajectory_ = predictionStrategy_.get()->predictTrajectory(status_, windData_);
    this->trajectoryLine_->setData(trajectory_);
}
