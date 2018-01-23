#include <Readers/FileReader.h>
#include <Utilities/ReaderUtils.h>
#include <3D/Utils.h>

std::pair<float, float> vectorToPair(const QVector2D &v) {
    float y = v.y();
    float x = v.x();
    float speed = v.length();
    float angle = toDegrees(std::atan2(v.y(), v.x()));
    return {speed, angle};
};

WindData WindData::fromFile(const std::string &predictionsPath) {
    FileReader<WindPrediction> predictionReader{predictionsPath, windPredictionFromString};
    return WindData(predictionReader.readFile());
}


WindData::WindData(std::vector<WindPrediction> &&preds) {

    for (size_t i = 1; i < preds.size(); i++) {
        windTable_.insert(std::make_pair(
                boost::icl::interval<float>::type{preds[i - 1].altitude, preds[i].altitude},
                dataToWindVector(preds[i].speed, preds[i].direction)
        ));
        rawPreds_.push_back(preds[i]);
    }
}

QVector2D WindData::operator[](const float &altitude) const {
    return (windTable_.find(altitude) != windTable_.end()) ? windTable_(altitude) : QVector2D{0, 0};
}

std::pair<float, float> WindData::speedAndAngleForAltitude(const float &altitude) const {
    return (windTable_.find(altitude) != windTable_.end()) ? vectorToPair(windTable_(altitude))
                                                           : std::make_pair<float, float>(0, 0);
};

/**
 * Creates a QVector2D given a wind direction and wind speed
 *
 * @param windSpeed Wind speed in meters per second
 * @param windDirection Wind direction in degrees relative to North, clockwise
 * @return A QVector2D representing the wind speed on each OpenGL axis
 */
QVector2D WindData::dataToWindVector(const float &windSpeed, const float &windDirection) {

    // OpenGL x axis is aligned with north. Direction angles are specified relative to north, going clockwise
    double rads = ((M_PI / 180) * windDirection);
    return {static_cast<float>(cos(rads) * windSpeed), static_cast<float>(sin(rads) * windSpeed)};
}