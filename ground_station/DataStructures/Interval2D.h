
#ifndef GS_MATTERHORN_INTERVAL2D_H
#define GS_MATTERHORN_INTERVAL2D_H


#include <boost/icl/interval.hpp>

class Interval2D {
public:

    explicit Interval2D() :
            xInterval_{boost::icl::construct<boost::icl::discrete_interval<int>>(0, 0)},
            yInterval_{boost::icl::construct<boost::icl::discrete_interval<int>>(0, 0)} {};

    explicit Interval2D(const boost::icl::discrete_interval<int> &xInterval,
                        const boost::icl::discrete_interval<int> &yInterval) :
            xInterval_{xInterval}, yInterval_{yInterval} {};

    bool contains(const int x, const int y) const {
        return boost::icl::contains(xInterval_, x) && boost::icl::contains(yInterval_, y);
    }

    int fromX() const {
        return xInterval_.lower();
    }

    int toX() const {
        return xInterval_.upper();
    }

    int fromY() const {
        return yInterval_.lower();
    }

    int toY() const {
        return yInterval_.upper();
    }

private:
    boost::icl::discrete_interval<int> xInterval_;
    boost::icl::discrete_interval<int> yInterval_;
};

#endif //GS_MATTERHORN_INTERVAL2D_H
