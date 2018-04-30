#ifndef GS_MATTERHORN_IDATAHANDLER_H
#define GS_MATTERHORN_IDATAHANDLER_H

#include <vector>
#include <memory>

enum class HandlerStatus {
    NOMINAL, LOSSY, DOWN
};

/**
 * Interface class for all objects which periodically receives data from various types of input
 * such as sensors or serial ports.
 *
 * @tparam T The type of data produced by the DataHandler
 */
template<class T>
class IDataHandler {

public:

    IDataHandler() {}

    virtual ~IDataHandler() {};

    /**
     *
     * @return The data produced by this DataHandler since the last call to the function
     */
    virtual std::vector<T> pollSensorsData() = 0;

};


#endif //GS_MATTERHORN_IDATAHANDLER_H
