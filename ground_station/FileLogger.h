#ifndef FILELOGGER_H
#define FILELOGGER_H

#include <vector>
#include <array>
#include <string>
#include <functional>
#include "DataStructures/datastructs.h"

using namespace std;

class FileLogger
{
public:
    FileLogger(std::string, int);

    void writeData(vector<reference_wrapper<ILoggable>>);

private:
    void writeFile();

    const int entrySize;
    const std::string path;
    static const size_t bufferSize = 1000;

    size_t id;
    size_t bufferIndex;

    array<string, bufferSize> buffer;
};

#endif // FILELOGGER_H