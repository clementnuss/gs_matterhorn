
#ifndef GS_MATTERHORN_IPREDICTIONSTRATEGY_H
#define GS_MATTERHORN_IPREDICTIONSTRATEGY_H

#include <QtGui/QVector3D>
#include <DataStructures/WindData.h>
#include <QtCore/QVector>
#include <ProgramConstants.h>

struct FlightStatus {
    QVector3D position;
    QVector3D speed;
    QVector3D acceleration;
};

class IPredictionStrategy {
public:
    virtual QVector<QVector3D> predictTrajectory(const FlightStatus &, const WindData &) = 0;
};

class ConstantPredictionStrategy : public IPredictionStrategy {

public:
    QVector<QVector3D> predictTrajectory(const FlightStatus &currentStatus, const WindData &windData) override {
        QVector<QVector3D> preds{};
        preds.push_back(currentStatus.position);
        preds.push_back({currentStatus.position.x(), 0, currentStatus.position.z()});
        return preds;
    }
};

class NaivePredictionStrategy : public IPredictionStrategy {

public:
    QVector<QVector3D> predictTrajectory(const FlightStatus &currentStatus, const WindData &windData) override {
        // One datapoint per second.
        // Each second, update estimatedPosition by corresponding descent rate and wind speed

        QVector<QVector3D> preds{};
        QVector3D estimatedPosition = currentStatus.position;

        float currentAltitude = estimatedPosition.y();

        while (currentAltitude > 0) {
            float rate = (currentAltitude < PredictorConstants::PARACHUTE_DEPLOYMENT_ALTITUDE) ?
                         PredictorConstants::SLOW_DESCENT_RATE :
                         PredictorConstants::FAST_DESCENT_RATE;

            QVector2D windVector = windData[currentAltitude];

            estimatedPosition += QVector3D{
                    PredictorConstants::PREDICTION_TIME_INTERVAL * windVector.x(),
                    -PredictorConstants::PREDICTION_TIME_INTERVAL * rate,
                    PredictorConstants::PREDICTION_TIME_INTERVAL * windVector.y()};

            currentAltitude = estimatedPosition.y();

            preds.push_back(estimatedPosition);
        }

        return preds;
    }
};

class AdaptiveSpeedPredictionStrategy : public IPredictionStrategy {
public:

    explicit AdaptiveSpeedPredictionStrategy() : accumulatedSpeed_{0, 0, 0} {};

    QVector<QVector3D> predictTrajectory(const FlightStatus &currentStatus, const WindData &windData) override {

        accumulatedSpeed_ = currentStatus.speed;
        // One datapoint per second.
        // Each second, update estimatedPosition by corresponding descent rate, current speed and wind speed

        QVector<QVector3D> preds{};
        QVector3D estimatedPosition = currentStatus.position;

        float currentAltitude = estimatedPosition.y();

        while (currentAltitude > 0) {
            float rate = (currentAltitude < PredictorConstants::PARACHUTE_DEPLOYMENT_ALTITUDE) ?
                         PredictorConstants::SLOW_DESCENT_RATE :
                         PredictorConstants::FAST_DESCENT_RATE;

            QVector2D windVector = windData[currentAltitude];

            estimatedPosition += QVector3D{
                    PredictorConstants::PREDICTION_TIME_INTERVAL * windVector.x(),
                    -PredictorConstants::PREDICTION_TIME_INTERVAL * rate,
                    PredictorConstants::PREDICTION_TIME_INTERVAL * windVector.y()};

            estimatedPosition += accumulatedSpeed_;
            accumulatedSpeed_ *= AdaptiveSpeedPredictionStrategy::speedAccumulationFactor;

            currentAltitude = estimatedPosition.y();

            preds.push_back(estimatedPosition);
        }

        return preds;
    }

private:
    QVector3D accumulatedSpeed_;

    static constexpr float speedAccumulationFactor = 0.8f;
};

#endif //GS_MATTERHORN_IPREDICTIONSTRATEGY_H
