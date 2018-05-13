
#ifndef GS_MATTERHORN_PACKETDISPATCHER_H
#define GS_MATTERHORN_PACKETDISPATCHER_H

#include <DataStructures/Datastructs.h>
#include <qcustomplot.h>
#include <deque>
#include <Loggers/FileLogger.h>

class Worker;

class PacketDispatcher {
public:
    explicit PacketDispatcher(Worker *containerWorker);

    void processDataFlows();

    virtual void dispatch(SensorsPacket *);

    virtual void dispatch(GPSPacket *);

    virtual void dispatch(EventPacket *);

    template<class T>
    void emitIfNonEmpty(const std::vector<T *> &v);

    void displayFreshValues();

    void releaseMemory();

    bool toggleLogging();

private:

    Worker *const worker_;
    std::map<FlyableType, std::vector<SensorsPacket *>> sensorsPacketQueues_;
    std::map<FlyableType, std::vector<GPSPacket *>> gpsPacketQueues_;
    std::map<FlyableType, std::vector<EventPacket *>> eventPacketQueues_;
    std::map<FlyableType, std::chrono::system_clock::time_point> lastUpdates_;

    FileLogger sensorsLogger_, gpsLogger_, eventLogger_;

    bool logEnabled_;
};

#endif //GS_MATTERHORN_PACKETDISPATCHER_H
