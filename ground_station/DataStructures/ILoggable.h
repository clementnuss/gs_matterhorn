
#ifndef GS_MATTERHORN_LOGGABLE_H
#define GS_MATTERHORN_LOGGABLE_H

#include <string>

class ILoggable {

public:
    virtual std::string toString() = 0;

protected:
    static const std::string DELIMITER;
};


#endif //GS_MATTERHORN_LOGGABLE_H
