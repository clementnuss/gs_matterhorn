
#ifndef GS_MATTERHORN_HGTDISCRETEELEVATIONMODEL_H
#define GS_MATTERHORN_HGTDISCRETEELEVATIONMODEL_H


#include <DataStructures/Interval2D.h>

class IDiscreteElevationModel {

public:
    virtual float elevationAt(int, int) const = 0;

    virtual Interval2D extent() const = 0;

};


#endif //GS_MATTERHORN_HGTDISCRETEELEVATIONMODEL_H
