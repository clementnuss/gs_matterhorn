
#ifndef GS_MATTERHORN_HGTDISCRETEELEVATIONMODEL_H
#define GS_MATTERHORN_HGTDISCRETEELEVATIONMODEL_H


#include <DataStructures/Interval2D.h>
#include "ProgramConstants.h"
#include <iostream>

class IDiscreteElevationModel {

public:
    virtual float elevationAt(int, int) const = 0;

    virtual Interval2D extent() const = 0;

    static GeoPoint topLeftFrom(const std::string &path) {

        std::string p = path.substr(0, path.rfind('.'));

        auto pos = p.rfind('/');
        if (pos != std::string::npos) {
            p.erase(0, pos + 1);
        }

        if (p.size() != FileConstants::HGT_FILENAME_LENGTH_MIN) {
            throw std::invalid_argument("Given DEM path has incorrect name format; bad number of characters");
        }

        int signLat, signLon;

        if (p[0] == 'N') {
            signLat = 1;
        } else {
            signLat = -1;
        }

        if (p[3] == 'E') {
            signLon = 1;
        } else {
            signLon = -1;
        }

        int lat, lon;

        try {
            lat = std::stoi(p.substr(1, 2));
            lon = std::stoi(p.substr(4, 3));
        } catch (const std::invalid_argument &) {
            throw std::invalid_argument(
                    "Given DEM path has incorrect name format; impossible to parse latitude/longitude");
        }

        return GeoPoint{{signLat * lat, 0, 0},
                        {signLon * lon, 0, 0}};
    }
};


#endif //GS_MATTERHORN_HGTDISCRETEELEVATIONMODEL_H
