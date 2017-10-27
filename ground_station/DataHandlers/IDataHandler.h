#ifndef GS_MATTERHORN_IDATAHANDLER_H
#define GS_MATTERHORN_IDATAHANDLER_H

#include <vector>
#include <memory>

template<class T>
class IDataHandler {

public:
    virtual std::vector<T> pollData() = 0;
};


#endif //GS_MATTERHORN_IDATAHANDLER_H
