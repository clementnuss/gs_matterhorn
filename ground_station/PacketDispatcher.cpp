
#include <iostream>
#include "PacketDispatcher.h"
#include "MainWorker.h"

PacketDispatcher::PacketDispatcher(Worker *const containerWorker) :
        worker_{containerWorker},
        rssiResponsesQueue_{},
        sensorsPacketQueues_{},
        gpsPacketQueues_{},
        eventPacketQueues_{},
        lastUpdates_{},
        atLogger_{LogConstants::WORKER_ATCOMMAND_LOG_PATH},
        sensorsLogger_{LogConstants::WORKER_TELEMETRY_LOG_PATH},
        gpsLogger_{LogConstants::WORKER_GPS_LOG_PATH},
        eventLogger_{LogConstants::WORKER_EVENT_LOG_PATH},
        logEnabled_{true} {

    // Build queues for each flyable type
    for (size_t i = 0; i < FlyableType::Count; i++) {
        sensorsPacketQueues_.emplace(
                std::make_pair<FlyableType, std::vector<SensorsPacket *>>(static_cast<FlyableType>(i), {})
        );
        gpsPacketQueues_.emplace(
                std::make_pair<FlyableType, std::vector<GPSPacket *>>(static_cast<FlyableType>(i), {})
        );
        eventPacketQueues_.emplace(
                std::make_pair<FlyableType, std::vector<EventPacket *>>(static_cast<FlyableType>(i), {})
        );
    }
}

void
PacketDispatcher::dispatch(SensorsPacket *p) {
    sensorsPacketQueues_[p->flyableType_].emplace_back(p);

    if (logEnabled_)
        sensorsLogger_.registerString(p->toString());
}

void
PacketDispatcher::dispatch(GPSPacket *p) {
    gpsPacketQueues_[p->flyableType_].emplace_back(p);

    if (logEnabled_)
        gpsLogger_.registerString(p->toString());
}

void
PacketDispatcher::dispatch(EventPacket *p) {
    eventPacketQueues_[p->flyableType_].emplace_back(p);

    if (logEnabled_)
        eventLogger_.registerString(p->toString());
}

void
PacketDispatcher::dispatch(RSSIResponse *r) {
    rssiResponsesQueue_.emplace_back(r);

    if (logEnabled_)
        atLogger_.registerString(r->toString());
}



void
PacketDispatcher::processDataFlows() {

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    for (auto *reading : sensorsPacketQueues_[FlyableType::ROCKET]) {
        stringstream ss{};
        ss << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER);
        ss << "SENSOR# ";
        ss << reading->toString();
        std::cout << ss.str() << std::endl;
    }

    for (auto *reading : gpsPacketQueues_[FlyableType::ROCKET]) {
        stringstream ss{};
        ss << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER);
        ss << "GPS# ";
        ss << reading->toString();
        std::cout << ss.str() << std::endl;
    }

    for (auto *reading : rssiResponsesQueue_) {
        stringstream ss{};
        ss << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER);
        ss << "RSSI# ";
        ss << static_cast<int>(reading->value_);
        std::cout << ss.str() << std::endl;
    }

    releaseMemory();
}


template<class T>
void
freeQueue(std::vector<T> &v) {

    for (auto *dp : v) {
        delete dp;
    }

    v.clear();
}

void
PacketDispatcher::releaseMemory() {

    freeQueue<RSSIResponse *>(rssiResponsesQueue_);

    for (auto &entry : sensorsPacketQueues_) {
        freeQueue<SensorsPacket *>(entry.second);
    }

    for (auto &entry : gpsPacketQueues_) {
        freeQueue<GPSPacket *>(entry.second);
    }

    for (auto &entry : eventPacketQueues_) {
        freeQueue<EventPacket *>(entry.second);
    }
}

bool
PacketDispatcher::toggleLogging() {
    logEnabled_ = !logEnabled_;
    return logEnabled_;
}
