//
// Created by clement on 05/12/2017.
//

#include "StateEstimator.h"

StateEstimator::StateEstimator(TelemetryHandler *underlyingHandler) :
        handler_{underlyingHandler} {
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


    return polledData;
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

