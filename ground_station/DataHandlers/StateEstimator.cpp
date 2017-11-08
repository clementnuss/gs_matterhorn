#include "StateEstimator.h"

StateEstimator::StateEstimator(TelemetryHandler *underlyingHandler) :
        handler_{underlyingHandler}, readingsBuffer_{MOVING_AVERAGE_POINTS} {
    auto replayHandler = dynamic_cast<TelemetryReplayHandler *>(handler_.get());
    if (replayHandler != nullptr) {
        replayHandler_ = unique_ptr<TelemetryReplayHandler>(replayHandler);
    }
}

void StateEstimator::startup() {
    handler_->startup();

}

vector<RocketEvent> StateEstimator::pollEvents() {
    auto polledEvents = handler_->pollEvents();

    return polledEvents;
}

vector<TelemetryReading> StateEstimator::pollData() {
    auto polledData = handler_->pollData();
    vector<TelemetryReading> smoothedReadings;

    for (const auto &telemReading : polledData) {
        smoothedReadings.push_back(computeMA(telemReading));
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