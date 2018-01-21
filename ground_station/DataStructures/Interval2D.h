
#ifndef GS_MATTERHORN_INTERVAL2D_H
#define GS_MATTERHORN_INTERVAL2D_H


#include <boost/icl/interval.hpp>

// Heavily inspired from
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

    int size() {
        return boost::icl::size(latitudeInterval_) * boost::icl::size(longitudeInterval_);
    }

    int sizeOfIntersectionWith(const Interval2D &that) {
        return boost::icl::size(this->latitudeInterval_ & that.latitudeInterval_) *
               boost::icl::size(this->longitudeInterval_ & that.longitudeInterval_);
    }

    Interval2D boundingUnion(const Interval2D &that) {
        return Interval2D(
                boost::icl::hull(this->latitudeInterval_, that.latitudeInterval_),
                boost::icl::hull(this->longitudeInterval_, that.longitudeInterval_)
        );
    }

    bool isUnionableWith(Interval2D that) {
        int unionSize = this->size() + that.size() - this->sizeOfIntersectionWith(that);
        return boundingUnion(that).size() == unionSize;
    }

    Interval2D unionWith(const Interval2D &that) {
        if (!isUnionableWith(that)) {
            throw std::invalid_argument("These intervals are not unionable");
        }
        return boundingUnion(that);
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
