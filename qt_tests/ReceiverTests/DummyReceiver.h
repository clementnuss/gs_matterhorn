
#ifndef GS_MATTERHORN_DUMMYRECEIVER_H
#define GS_MATTERHORN_DUMMYRECEIVER_H

#include <DataHandlers/IReceiver.h>

class DummyReceiver : public IReceiver {

public:
    DummyReceiver() : internalQueue_{} {}

    virtual ~DummyReceiver() {};

    virtual void startup() {};

    virtual std::list<std::unique_ptr<DataPacket>> pollData() {
        std::list<std::unique_ptr<DataPacket>> l;

        while (!internalQueue_.empty()) {
            l.push_back(std::move(internalQueue_.front()));
            internalQueue_.pop_front();
        }

        return l;
    };

    virtual std::list<std::unique_ptr<ATCommandResponse>> pollATResponses() {
        std::list<std::unique_ptr<ATCommandResponse>> l;
        return l;
    };


    virtual void sendCommand(const uint8_t *, size_t) {};

    virtual bool isReplayReceiver() {};

    void registerPackets(std::vector<DataPacket> v) {
        for (const auto &e : v) {
            internalQueue_.push_back(std::make_unique<DataPacket>(e));
        }
    }

private:
    std::list<std::unique_ptr<DataPacket>> internalQueue_;
};

#endif //GS_MATTERHORN_DUMMYRECEIVER_H
