
#ifndef GS_MATTERHORN_IOBSERVABLE_H
#define GS_MATTERHORN_IOBSERVABLE_H

#include <Qt3DCore/QTransform>

class IObservable {
public :
    virtual Qt3DCore::QTransform *getObjectTransform() const = 0;
};

#endif //GS_MATTERHORN_IOBSERVABLE_H
