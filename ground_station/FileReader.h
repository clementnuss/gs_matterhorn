#ifndef GS_MATTERHORN_FILEREADER_H
#define GS_MATTERHORN_FILEREADER_H

#include <string>
#include <fstream>

class FileReader {

public:
    FileReader(std::string path);

    ~FileReader();

private:
    std::string path;
    std::ifstream inputStream;
};


#endif //GS_MATTERHORN_FILEREADER_H
