
#ifndef GS_MATTERHORN_LOGGABLE_H
#define GS_MATTERHORN_LOGGABLE_H

#include <string>

class ILoggable {

public:
    virtual std::string toString() const = 0;

protected:

};

static const std::string DELIMITER = "\t";

#endif //GS_MATTERHORN_LOGGABLE_H
