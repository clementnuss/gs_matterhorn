
#include <ProgramConstants.h>
#include <DataStructures/datastructs.h>
#include <FileReader.h>
#include <Utilities/ReaderUtils.h>
#include "SplashDownPredictor.h"


SplashDownPredictor::SplashDownPredictor(std::string &path, Qt3DCore::QNode *parent) :
        predictorPos_{0, 0, 0},
        trajectoryEstimations_{},
        descentSpeed_{0},
        trajectoryLine_{new Line(parent, QColor::fromRgb(255, 153, 0))},
        searchIndex_{0},
        windData_{WindData::fromFile(path)} {

}

QVector2D SplashDownPredictor::getTouchdownCoordinates() {
    QVector3D &lastPoint = trajectoryEstimations_.last();
    return QVector2D{lastPoint.x(), lastPoint.z()};
}

void SplashDownPredictor::updatePos(const QVector3D &pos) {
    predictorPos_ = pos;
    this->recomputePrediction();
}

void SplashDownPredictor::recomputePrediction() {
    // One datapoint per second.
    // Each second, update estimatedPosition by corresponding descent rate and wind speed

    trajectoryEstimations_.clear();
    QVector3D estimatedPosition = predictorPos_;
    searchIndex_ = 0;

    float currentAltitude = predictorPos_.y();

    while (currentAltitude > 0) {
        float rate = (currentAltitude < PredictorConstants::PARACHUTE_DEPLOYMENT_ALTITUDE) ?
                     PredictorConstants::SLOW_DESCENT_RATE :
                     PredictorConstants::FAST_DESCENT_RATE;

        QVector2D windVector = windData_[currentAltitude];

        estimatedPosition += QVector3D{
                PredictorConstants::PREDICTION_TIME_INTERVAL * windVector.x(),
                -PredictorConstants::PREDICTION_TIME_INTERVAL * rate,
                PredictorConstants::PREDICTION_TIME_INTERVAL * windVector.y()};

        currentAltitude = estimatedPosition.y();

        trajectoryEstimations_.push_back(estimatedPosition);
    }

    this->trajectoryLine_->setData(trajectoryEstimations_);
}
