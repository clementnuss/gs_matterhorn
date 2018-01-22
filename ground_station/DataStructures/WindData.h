
#ifndef GS_MATTERHORN_WINDDATA_H
#define GS_MATTERHORN_WINDDATA_H

#include <boost/icl/interval_map.hpp>
#include <vector>
#include <QtGui/QVector2D>

struct WindPrediction {
    float altitude;
    float speed;
    float direction;
};

class WindData {
public:
    static WindData fromFile(const std::string &predictionsPath);

    QVector2D operator[](const float &altitude) const;

    std::pair<float, float> speedAndAngleForAltitude(const float &altitude) const;

private:
    explicit WindData(std::vector<WindPrediction> &&preds);

    QVector2D dataToWindVector(const float &windSpeed, const float &windDirection);

    boost::icl::interval_map<float, QVector2D> windTable_;
    std::vector<WindPrediction> rawPreds_;

};


#endif //GS_MATTERHORN_WINDDATA_H
