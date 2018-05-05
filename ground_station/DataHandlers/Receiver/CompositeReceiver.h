
#ifndef GS_MATTERHORN_COMPOSITERECEIVER_H
#define GS_MATTERHORN_COMPOSITERECEIVER_H


#include <DataHandlers/IReceiver.h>
#include <queue>

class CompositeReceiver : public IReceiver {

    friend class CompositeReceiverTests;

public:
    CompositeReceiver(std::unique_ptr<IReceiver>, std::unique_ptr<IReceiver>, const unsigned int);

    ~CompositeReceiver() override;

    std::list<std::unique_ptr<DataPacket>> pollData() override;

    void startup() override;

    bool isReplayReceiver() override;

    void sendCommand(const uint8_t *, size_t) override;

private:
    void mergePacketQueuesStep();

    void addToMergeQueueAndPop(std::unique_ptr<DataPacket> *, std::list<std::unique_ptr<DataPacket>> *);

private:
    std::unique_ptr<IReceiver> primaryReceiver_;
    std::unique_ptr<IReceiver> backupReceiver_;
    std::list<std::unique_ptr<DataPacket>> primaryQueue_;
    std::list<std::unique_ptr<DataPacket>> backupQueue_;
    std::list<std::unique_ptr<DataPacket>> mergeQueue_;

    const unsigned int limitLag_;
    unsigned int sequenceIndex_;
};


#endif //GS_MATTERHORN_COMPOSITERECEIVER_H
