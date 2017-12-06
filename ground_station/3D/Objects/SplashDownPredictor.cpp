
#include <ProgramConstants.h>
#include <DataStructures/datastructs.h>
#include <FileReader.h>
#include <Utilities/ReaderUtils.h>
#include "SplashDownPredictor.h"

SplashDownPredictor::SplashDownPredictor(std::string &path, Qt3DCore::QNode *parent) :
        windTable_{}, predictorPos_{0, 0, 0}, trajectoryEstimations_{}, descentSpeed_{0},
        trajectoryLine_{new Line(parent, QColor::fromRgb(255, 153, 0))}, searchIndex_{0}, predictionPath_{path} {

    loadPredictions();

}

void SplashDownPredictor::loadPredictions() {
    FileReader<WindPrediction> predictionReader{predictionPath_, windPredictionFromString};

    QVector<WindPrediction> predictions = predictionReader.readFile();

    for (auto prediction : predictions) {
        windTable_.push_back(
                {
                        static_cast<int>(prediction.altitude_),
                        dataToWindVector(prediction.speed_, prediction.direction_)
                }
        );
    }

    // Sort the table by decreasing altitude
    qSort(windTable_.begin(), windTable_.end(),
          [](const QPair<int, QVector2D> &p1, const QPair<int, QVector2D> &p2) { return p1.first > p2.first; });

    std::cout << ".." << std::endl;
    for (auto pair : windTable_) {
        std::cout << pair.first << " " << pair.second.x() << " " << pair.second.y() << std::endl;
    }
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

/**
 * Creates a QVector2D given a wind direction and wind speed
 *
 * @param windSpeed Wind speed in meters per second
 * @param windDirection Wind direction in degrees relative to North, clockwise
 * @return A QVector2D representing the wind speed on each OpenGL axis
 */
QVector2D SplashDownPredictor::dataToWindVector(const float windSpeed, const float windDirection) {

    // OpenGL x axis is aligned with north. Direction angles are specified relative to north, going clockwise

    float rads = -((M_PI / 180) * windDirection);
    return {sin(rads) * windSpeed, cos(rads) * windSpeed};
}
