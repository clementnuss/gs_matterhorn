#ifndef GS_MATTERHORN_IRECEIVER_H
#define GS_MATTERHORN_IRECEIVER_H

#include <DataStructures/Datastructs.h>


/**
 * The IReceiver is the base class for telemetry-producing instances
 */
class IReceiver {

public:

    virtual ~IReceiver() {};

    virtual void startup() = 0;

    /**
     *
     * @return The packets received by this IReceiver since the last call to the function
     */
    virtual std::list<std::unique_ptr<DataPacket>> pollData() = 0;

    virtual void sendCommand(const uint8_t *, size_t) = 0;

    virtual bool isReplayReceiver()= 0;

};


#endif //GS_MATTERHORN_IRECEIVER_H
