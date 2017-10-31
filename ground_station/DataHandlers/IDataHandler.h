#ifndef GS_MATTERHORN_IDATAHANDLER_H
#define GS_MATTERHORN_IDATAHANDLER_H

#include <vector>
#include <memory>

enum class HandlerStatus {
    NOMINAL, LOSSY, DOWN
};

template<class T>
class IDataHandler {

public:

    IDataHandler() {}

    virtual ~IDataHandler() {}

    virtual std::vector<T> pollData() = 0;

};


#endif //GS_MATTERHORN_IDATAHANDLER_H
