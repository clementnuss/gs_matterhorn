
#ifndef GS_MATTERHORN_TRACEREADER_H
#define GS_MATTERHORN_TRACEREADER_H

#include <boost/filesystem.hpp>
#include <QtGui/QVector3D>
#include <QtCore/QVector>

//TODO: Define common class hierarchy for FileReader/TelemetryReplay
class TraceReader {
public:
    TraceReader(int resolution);

    QVector<QVector3D> read(std::string &&path);

private:
    QVector<QVector3D> parseFile(boost::filesystem::path &);

    int resolution_;
};


#endif //GS_MATTERHORN_TRACEREADER_H
