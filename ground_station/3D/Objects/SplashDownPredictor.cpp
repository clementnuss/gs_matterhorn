
#include <ProgramConstants.h>
#include "SplashDownPredictor.h"

SplashDownPredictor::SplashDownPredictor(Qt3DCore::QNode *parent) :
        windTable_{}, predictorPos_{0, 0, 0}, trajectoryEstimations_{}, descentSpeed_{0},
        trajectoryLine_{new Line(parent, QColor::fromRgb(255, 153, 0))}, searchIndex_{0} {

    windTable_.push_back({3000, QVector2D{10, 0}});
    windTable_.push_back({2500, QVector2D{5, 2}});
    windTable_.push_back({2290, QVector2D{2, 5}});
    windTable_.push_back({2000, QVector2D{0, 10}});
    windTable_.push_back({1700, QVector2D{0, 10}});
    windTable_.push_back({1300, QVector2D{0, 10}});
    windTable_.push_back({1000, QVector2D{0, 5}});
    windTable_.push_back({600, QVector2D{0, -5}});
    windTable_.push_back({400, QVector2D{0, -10}});

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

        QVector2D windVector = windVectorForAltitude(currentAltitude);

        estimatedPosition += QVector3D{
                PredictorConstants::PREDICTION_TIME_INTERVAL * windVector.x(),
                -PredictorConstants::PREDICTION_TIME_INTERVAL * rate,
                PredictorConstants::PREDICTION_TIME_INTERVAL * windVector.y()};

        currentAltitude = estimatedPosition.y();

        trajectoryEstimations_.push_back(estimatedPosition);
    }

    this->trajectoryLine_->setData(trajectoryEstimations_);
}

//TODO: Better data structure for faster implementation
QVector2D SplashDownPredictor::windVectorForAltitude(float alt) {

    while (searchIndex_ < windTable_.size() && windTable_[searchIndex_].first > alt) {
        searchIndex_++;
    }

    if (searchIndex_ >= windTable_.size()) {
        return windTable_[windTable_.size() - 1].second;
    } else if (searchIndex_ == 0) {
        return windTable_[searchIndex_].second;
    } else {
        return windTable_[searchIndex_ - 1].second;
    }

}
