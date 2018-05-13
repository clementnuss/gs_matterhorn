
#ifndef GS_MATTERHORN_PACKETDISPATCHER_H
#define GS_MATTERHORN_PACKETDISPATCHER_H

#include <DataStructures/Datastructs.h>
#include <qcustomplot.h>
#include <deque>

class Worker;

class PacketDispatcher {
public:
    explicit PacketDispatcher(Worker *containerWorker);

    void processDataFlows();

    virtual void dispatch(SensorsPacket *);

    virtual void dispatch(GPSPacket *);

    virtual void dispatch(EventPacket *);

    void displayFreshValues();

    void releaseMemory();

    template<class T>
    void emitIfNonEmpty(const std::vector<T *> &v);

private:

    QVector<QCPGraphData> extractGraphData(std::vector<SensorsPacket *> &, QCPGraphData (*)(SensorsPacket));

    Worker *const worker_;
    std::map<FlyableType, std::vector<SensorsPacket *>> sensorsPacketQueues_;
    std::map<FlyableType, std::vector<GPSPacket *>> gpsPacketQueues_;
    std::map<FlyableType, std::vector<EventPacket *>> eventPacketQueues_;
    std::map<FlyableType, std::chrono::system_clock::time_point> lastUpdates_;
};

#endif //GS_MATTERHORN_PACKETDISPATCHER_H
