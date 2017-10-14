#ifndef GS_MATTERHORN_IDATAHANDLER_H
#define GS_MATTERHORN_IDATAHANDLER_H

#include <c++/vector>
#include <c++/memory>

template<class T>
class IDataHandler {

public:
    virtual const std::vector<T> getData() = 0;
};


#endif //GS_MATTERHORN_IDATAHANDLER_H
