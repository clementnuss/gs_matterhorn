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
    explicit FileLogger(const std::string &);

    ~FileLogger();

    void registerData(const vector<reference_wrapper<ILoggable>> &);

    void registerString(const std::string &);
    void close();

    static const size_t bufferSize = 1000;

private:
    void writeFile();

    void writeRoutine(array<string, bufferSize>, size_t);

    const std::string path;

    std::atomic_size_t id;
    size_t bufferIndex;

    array<string, bufferSize> buffer;
};

#endif // FILELOGGER_H