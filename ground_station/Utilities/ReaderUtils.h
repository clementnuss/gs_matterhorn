
#ifndef GS_MATTERHORN_READERUTILS_H
#define GS_MATTERHORN_READERUTILS_H

#include <boost/algorithm/string/split.hpp>
#include <QtGui/QVector3D>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <DataStructures/datastructs.h>
#include <DataStructures/WindData.h>

static QVector3D posFromString(const std::string &str) {

    float x = 0, y = 0, z = 0;

    std::vector<std::string> values;
    boost::split(values, str, boost::is_any_of("\t ,"),
                 boost::algorithm::token_compress_mode_type::token_compress_on);

    if (values.size() != 3) {
        std::cout << "\tInvalid reading, only " << values.size() << " values on the line:" << std::endl;
        std::cout << "\t" << str << std::endl;
    }
    try {

        auto it = values.begin();
        x = std::stof(*it++);
        y = std::stof(*it++);
        z = std::stof(*it++);

    } catch (std::logic_error &e) {
        std::cout << "\tunable to decode this reading:\n\t" << str;
    }

    return {x, z, y};
}

static WindPrediction windPredictionFromString(const std::string &str) {
    WindPrediction prediction{};

    std::string s = boost::trim_left_copy(str);
    std::vector<std::string> values;
    boost::split(values, s, boost::is_any_of("\t ,"),
                 boost::algorithm::token_compress_mode_type::token_compress_on);

    if (values.size() != 11) {
        return prediction;
    }
    try {

        auto it = values.begin();

        // Jump to meter value
        it += 1;
        prediction.altitude = std::stof(*it);

        // Jump to direction value
        it += 5;
        prediction.direction = std::stof(*it++);
        prediction.speed = std::stof(*it++) * UnitsConstants::KNOTS_TO_MS;


    } catch (std::logic_error &e) {
        std::cout << "\tunable to decode this reading:\n\t" << s;
    }

    return prediction;
}


#endif //GS_MATTERHORN_READERUTILS_H
