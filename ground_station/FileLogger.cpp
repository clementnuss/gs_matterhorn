#include <fstream>
#include <c++/iostream>
#include <sstream>
#include <thread>
#include "FileLogger.h"

using namespace std;

FileLogger::FileLogger(std::string path, int entrySize) :
        path{path}, entrySize{entrySize}, id{0}, bufferIndex{0}, busyFlag{false} {}

FileLogger::~FileLogger() {
    close();
}

void FileLogger::close() {
    if (bufferIndex != 0) {
        writeFile();
        bufferIndex = 0;
    }
}

void FileLogger::registerData(vector<reference_wrapper<ILoggable>> data) {

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
    thread t(FileLogger::writeRoutine, this, a, bufferIndex);

    t.detach();
}

void FileLogger::writeRoutine(array<string, bufferSize> a, size_t tailIndex) {

    if (isReady()) {
        throw logic_error("A call to write routine was made without raising the write flag");
    }

    if (a.size() < 1 || tailIndex < 1) {
        throw invalid_argument("Call to write routine was made either with an empty array or an invalid tail index");
    }

    stringstream ss;
    ss << id++ << path;
    ofstream fileOutput(ss.str());

    if (!fileOutput) {
        cerr << "Could not open file for writing." << endl;
        return;
    }

    for (int i = 0; i < tailIndex; i++) {
        fileOutput << a[i];

        if (i != tailIndex - 1) {
            fileOutput << endl;
        }
    }

    fileOutput.close();
    resetFlag();
}

bool FileLogger::isReady() {
    return !(this->busyFlag);
}

void FileLogger::raiseFlag() {
    this->busyFlag = true;
}

void FileLogger::resetFlag() {
    this->busyFlag = false;
}