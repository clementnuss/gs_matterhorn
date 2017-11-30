
#include "TraceReader.h"
#include <iostream>
#include <boost/algorithm/string.hpp>


TraceReader::TraceReader(int resolution) : resolution_{resolution} {}

QVector<QVector3D> TraceReader::read(std::string &&path) {
    boost::filesystem::path fpath{path};

    if (boost::filesystem::exists(fpath)) {
        if (boost::filesystem::is_regular_file((fpath))) {
            return parseFile(fpath);
        } else {
            throw (std::runtime_error(fpath.string() + " exists, but is not a regular file or directory\n"));
        }
    } else {
        throw (std::runtime_error(fpath.string() + " does not exist\n"));
    }
}


QVector<QVector3D> TraceReader::parseFile(boost::filesystem::path &p) {
    QVector<QVector3D> traceData{};
    boost::filesystem::ifstream ifs{p, std::ios::in};
    std::cout << "Parsing simulator trace file " << p.string() << std::endl;

    std::string reading;
    int index = 0;
    float x = 0, y = 0, z = 0;
    while (getline(ifs, reading)) {
        std::vector<std::string> values;
        boost::split(values, reading, boost::is_any_of("\t ,"),
                     boost::algorithm::token_compress_mode_type::token_compress_on);

        if (values.size() != 3) {
            std::cout << "\tInvalid reading, only " << values.size() << " values on the line:" << std::endl;
            std::cout << "\t" << reading << std::endl;
        }
        try {

            auto it = values.begin();
            x = std::stof(*it++);
            y = std::stof(*it++);
            z = std::stof(*it++);

            if (index++ % resolution_ == 0) {
                traceData.push_back({x, z, y});
            }

        } catch (std::logic_error &e) {
            std::cout << "\tunable to decode this reading:\n\t" << reading;
        }
    }

    // Make sure we use last data point
    if ((index - 1) % resolution_ != 0) {
        traceData.push_back({x, z, y});
    }


    ifs.close();

    return traceData;
}