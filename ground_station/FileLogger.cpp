#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <cassert>
#include "FileLogger.h"

using namespace std;

FileLogger::FileLogger(const std::string &path) :
        path{path}, id{0}, bufferIndex{0}, busyFlag{false} {}

FileLogger::~FileLogger() {
    close();
}

void FileLogger::close() {
    cout << "Flushing file logger @ " << path << endl << flush;
    if (bufferIndex != 0) {
        writeFile();
        bufferIndex = 0;
    }
}

void FileLogger::registerData(const vector<reference_wrapper<ILoggable>> &data) {

    for (const auto loggable : data) {

        if (bufferIndex >= bufferSize) {
            cout << "Writing log file.." << endl;
            writeFile();
            bufferIndex = 0;
        }

        buffer[bufferIndex] = loggable.get().toString();
        bufferIndex++;
    }
}

void FileLogger::registerString(const std::string &s) {

    if (bufferIndex >= bufferSize) {
        cout << "Writing log file.." << endl;
        writeFile();
        bufferIndex = 0;
    }

    buffer[bufferIndex] = s;
    bufferIndex++;

}

void FileLogger::writeFile() {

    while (busyFlag) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    busyFlag = true;

    array<string, bufferSize> a = buffer;
    thread t(&FileLogger::writeRoutine, this, a, bufferIndex);

    t.detach();
}

void FileLogger::writeRoutine(array<string, bufferSize> a, size_t tailIndex) {

    // The busy write flag should have been raised at this point
    //assert(busyFlag);
    assert(tailIndex >= 1);

    stringstream ss;
    ss << path << "_" << ++id;
    ofstream fileOutput(ss.str());

    if (!fileOutput) {
        //TODO: find an alternative way to save data ?
        cerr << "Could not open file for writing, data will be lost." << endl << flush;
        return;
    }

    for (size_t i = 0; i < tailIndex; i++) {
        fileOutput << a[i];

        if (i != tailIndex - 1) {
            fileOutput << endl;
        }
    }

    fileOutput.close();
    busyFlag = false;
}