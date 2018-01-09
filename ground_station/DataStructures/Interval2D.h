
#ifndef GS_MATTERHORN_INTERVAL2D_H
#define GS_MATTERHORN_INTERVAL2D_H


#include <boost/icl/interval.hpp>

class Interval2D {
public:

    explicit Interval2D() :
            longitudeInterval_{boost::icl::construct<boost::icl::discrete_interval<int>>(0, 0)},
            latitudeInterval_{boost::icl::construct<boost::icl::discrete_interval<int>>(0, 0)} {};

    explicit Interval2D(const boost::icl::discrete_interval<int> &latitudeInterval,
                        const boost::icl::discrete_interval<int> &longitudeInterval) :
            longitudeInterval_{longitudeInterval}, latitudeInterval_{latitudeInterval} {};

    bool contains(const int latitudeIndex, const int longitudeIndex) const {
        return boost::icl::contains(latitudeInterval_, latitudeIndex)
               &&
               boost::icl::contains(longitudeInterval_, longitudeIndex);
    }

    int startLongitude() const {
        return longitudeInterval_.lower();
    }

    int endLongitude() const {
        return longitudeInterval_.upper();
    }

    int startLatitude() const {
        return latitudeInterval_.lower();
    }

    int endLatitude() const {
        return latitudeInterval_.upper();
    }

private:
    boost::icl::discrete_interval<int> longitudeInterval_;
    boost::icl::discrete_interval<int> latitudeInterval_;
};

#endif //GS_MATTERHORN_INTERVAL2D_H
