#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include "FileLogger.h"

using namespace std;

FileLogger::FileLogger(std::string path, int entrySize) :
        path{std::move(path)}, entrySize{entrySize}, id{0}, bufferIndex{0}, busyFlag{false} {}

FileLogger::~FileLogger() {
    close();
}

void FileLogger::close() {
    if (bufferIndex != 0) {
        writeFile();
        bufferIndex = 0;
    }
}

void FileLogger::registerData(const vector<reference_wrapper<ILoggable>> &data) {

    for (const auto loggable : data) {

        if (bufferIndex >= bufferSize) {
            writeFile();
            bufferIndex = 0;
        }

        buffer[bufferIndex] = loggable.get().toString();
        bufferIndex++;
    }
}

void FileLogger::writeFile() {

    while (!isReady()) {
        //cout << "Waiting for file to be written " << endl;
    }

    raiseFlag();

    array<string, bufferSize> a = buffer;
    thread t(&FileLogger::writeRoutine, this, a, bufferIndex);

    t.detach();
}

void FileLogger::writeRoutine(array<string, bufferSize> a, size_t tailIndex) {

    //TODO add unit tests for exceptions
    if (isReady()) {
        throw logic_error("A call to write routine was made without raising the write flag");
    }

    if (tailIndex < 1) {
        throw invalid_argument("Call to write routine was made either with an empty array or an invalid tail index");
    }

    stringstream ss;
    ss << id++ << path;
    ofstream fileOutput(ss.str());

    if (!fileOutput) {
        //TODO: find an alternative way to save data ?
        cerr << "Could not open file for writing, data will be lost." << endl;
        return;
    }

    for (size_t i = 0; i < tailIndex; i++) {
        fileOutput << a[i];

        if (i != tailIndex - 1) {
            fileOutput << endl;
        }
    }

    fileOutput.close();
    resetFlag();
}

bool FileLogger::isReady() const {
    return !(this->busyFlag);
}

void FileLogger::raiseFlag() {
    this->busyFlag = true;
}

void FileLogger::resetFlag() {
    this->busyFlag = false;
}