#ifndef FILELOGGER_H
#define FILELOGGER_H

#include <vector>
#include <array>
#include <string>
#include <functional>
#include <atomic>
#include "DataStructures/datastructs.h"

using namespace std;

class FileLogger {
public:
    explicit FileLogger(const std::string &);

    ~FileLogger();

    void registerData(const vector<reference_wrapper<ILoggable>> &);

    void registerString(const std::string);

    void close();

    static const size_t bufferSize = 1000;

private:
    void writeFile();

    static void writeRoutine(array<string, bufferSize>, size_t, std::string, size_t);

    const std::string path_;

    size_t id_;
    size_t bufferIndex_;

    array<string, bufferSize> buffer;
};

#endif // FILELOGGER_H