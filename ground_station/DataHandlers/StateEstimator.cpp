#include "StateEstimator.h"

StateEstimator::StateEstimator(TelemetryHandler *underlyingHandler) :
        handler_{underlyingHandler},
        readingsBuffer_{MOVING_AVERAGE_POINTS},
        pendingDetectedRocketEvents_{} {
    auto replayHandler = dynamic_cast<ITelemetryReplayHandler *>(handler_.get());
    if (replayHandler != nullptr) {
        replayHandler_ = unique_ptr<ITelemetryReplayHandler>(replayHandler);
    }
}

void StateEstimator::startup() {
    handler_->startup();

}

vector<RocketEvent> StateEstimator::pollEvents() {
    auto polledEvents = handler_->pollEvents();
    if (!pendingDetectedRocketEvents_.empty()) {
        polledEvents.insert(polledEvents.end(),
                            pendingDetectedRocketEvents_.begin(), pendingDetectedRocketEvents_.end());
        pendingDetectedRocketEvents_.clear();
    }
    return polledEvents;
}

vector<TelemetryReading> StateEstimator::pollData() {
    auto polledData = handler_->pollData();
    vector<TelemetryReading> smoothedReadings;

    for (const auto &telemReading : polledData) {
        auto readingMA = computeMA(telemReading);
        smoothedReadings.push_back(readingMA);
        computeState(readingMA);
    }

    return smoothedReadings;
}

bool StateEstimator::isReplayHandler() {
    return (replayHandler_ != nullptr);
}

void StateEstimator::updatePlaybackSpeed(double speed) {
    assert (isReplayHandler());
    replayHandler_->updatePlaybackSpeed(speed);
}

void StateEstimator::setPlaybackReversed(bool reversed) {
    assert (isReplayHandler());
    replayHandler_->setPlaybackReversed(reversed);
}

void StateEstimator::resetPlayback() {
    assert (isReplayHandler());
    replayHandler_->resetPlayback();
}

bool StateEstimator::endOfPlayback() {
    assert (isReplayHandler());
    return replayHandler_->endOfPlayback();
}

TelemetryReading StateEstimator::computeMA(const TelemetryReading &r) {
    TelemetryReading movAverage;
    readingsBuffer_.push_back(r);

    for (const auto &telemReading : readingsBuffer_) {
        movAverage += telemReading;
    }

    movAverage *= 1.0 / readingsBuffer_.size();
    movAverage.timestamp_ = r.timestamp_;
    movAverage.sequenceNumber_ = r.sequenceNumber_;

    return movAverage;
}

void StateEstimator::computeState(const TelemetryReading &r) {
    switch (currentState_) {
        case READY: {
            if (r.acceleration_.norm() > ACCELERATION_THRESHOLD) {
                currentState_ = BURNING_PHASE;
                initialTelemetryState_ = r;
                RocketEvent event{r.timestamp_, 0, "MOTOR START"};
                pendingDetectedRocketEvents_.push_back(event);
            }
            break;
        }
        case BURNING_PHASE: {
            if (r.acceleration_.norm() < ACCELERATION_THRESHOLD) {
                currentState_ = BURNOUT;
                double deltaT = (r.timestamp_ - initialTelemetryState_.timestamp_) / 1'000'000.0;
                RocketEvent event{r.timestamp_, 0, "BURN OUT\n\tt+:" + std::to_string(deltaT)};
                pendingDetectedRocketEvents_.push_back(event);
            }
            break;
        }
        case BURNOUT: {
            if (r.acceleration_.norm() > 3 * ACCELERATION_THRESHOLD) {
                currentState_ = PARACHUTE_DESCENT;
                double deltaT = (r.timestamp_ - initialTelemetryState_.timestamp_) / 1'000'000.0;

                RocketEvent event{r.timestamp_, 0, "PARACHUTE_DEPLOYMENT\n\tt+:" + std::to_string(deltaT)};
                pendingDetectedRocketEvents_.push_back(event);
            }
            break;
        }
        case PARACHUTE_DESCENT: {
            if (abs(r.altitude_ - initialTelemetryState_.altitude_) < 15) {
                currentState_ = TOUCHDOWN;
                RocketEvent touchdown{r.timestamp_, 0, "TOUCHDOWN"};
                double flightDuration = (r.timestamp_ - initialTelemetryState_.timestamp_) / 1'000'000.0;
                RocketEvent flightDurationEvent{r.timestamp_, 0, "FLIGHT DURATION:\n\t" + std::to_string(flightDuration)};
                pendingDetectedRocketEvents_.push_back(touchdown);
                pendingDetectedRocketEvents_.push_back(flightDurationEvent);
            }
            break;
        }
        case TOUCHDOWN:
            break;
    }
}

vector<XYZReading> StateEstimator::pollLocations() {
    return vector<XYZReading>();
}
