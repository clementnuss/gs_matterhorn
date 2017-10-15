
#ifndef GS_MATTERHORN_LOGGABLE_H
#define GS_MATTERHORN_LOGGABLE_H

#include <string>

class ILoggable {

public:
    virtual std::string toString() const = 0;

protected:

};

#endif //GS_MATTERHORN_LOGGABLE_H
