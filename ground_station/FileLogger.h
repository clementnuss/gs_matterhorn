#ifndef FILELOGGER_H
#define FILELOGGER_H

#include <vector>
#include <array>
#include <string>
#include <functional>
#include <atomic>
#include "DataStructures/datastructs.h"

using namespace std;

class FileLogger
{
public:
    FileLogger(std::string, int);

    ~FileLogger();

    void registerData(vector<reference_wrapper<ILoggable>>);
    void close();

    bool isReady();

    static const size_t bufferSize = 1000;

private:
    void writeFile();

    void writeRoutine(array<string, bufferSize>, size_t);

    void resetFlag();

    void raiseFlag();
    const int entrySize;
    const std::string path;

    size_t id;
    size_t bufferIndex;

    std::atomic_bool busyFlag;

    array<string, bufferSize> buffer;
};

#endif // FILELOGGER_H