
#include <ProgramConstants.h>
#include <DataStructures/datastructs.h>
#include <FileReader.h>
#include <Utilities/ReaderUtils.h>
#include <3D/Ground/Ground.h>
#include "SplashDownPredictor.h"


SplashDownPredictor::SplashDownPredictor(std::string &path, Qt3DCore::QNode *parent) :
        predictionStrategy_{std::make_unique<AdaptiveSpeedPredictionStrategy>()},
        status_{},
        trajectory_{},
        trajectoryLine_{new Line(parent, QColor::fromRgb(220, 0, 0))},
        windData_{WindData::fromFile(path)} {
    status_.position = {0, 0, 0};
    status_.speed = {0, 0, 0};
    status_.acceleration = {0, 0, 0};
}

void SplashDownPredictor::highlightTouchdown(Ground *const ground) const {

    for (const QVector3D *it = trajectory_.end() - 1; it != trajectory_.begin(); it--) {
        float x = it->x();
        float z = it->z();

        float elev = ground->groundElevationAt(
                static_cast<int>(std::round(x)),
                static_cast<int>(std::round(z))
        );

        if (elev < it->y()) {
            ground->highlightRegion({x, z});
        }
    }

    ground->highlightRegion({trajectory_.first().x(), trajectory_.first().z()});
}

void SplashDownPredictor::updatePos(const QVector3D &pos) {
    status_.position = pos;
}

void SplashDownPredictor::updateSpeed(const QVector3D &speed) {
    status_.speed = status_.speed * SplashDownPredictor::LEAKY_FACTOR + speed * (1 - SplashDownPredictor::LEAKY_FACTOR);
}

void SplashDownPredictor::recomputePrediction() {
    trajectory_ = predictionStrategy_.get()->predictTrajectory(status_, windData_);
    this->trajectoryLine_->setData(trajectory_);
}
