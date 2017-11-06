#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <cassert>
#include "FileLogger.h"

using namespace std;

FileLogger::FileLogger(const std::string &path) :
        path{path}, id{0}, bufferIndex{0} {}

FileLogger::~FileLogger() {
    close();
}

void FileLogger::close() {
    cout << "\n Flushing file logger @ " << path << endl;
    if (bufferIndex != 0) {
        writeFile();
        bufferIndex = 0;
    }
}

void FileLogger::registerData(const vector<reference_wrapper<ILoggable>> &data) {

    for (const auto loggable : data) {

        if (bufferIndex >= bufferSize) {
            cout << "\n Writing log file.." << endl;
            writeFile();
            bufferIndex = 0;
        }

        buffer[bufferIndex] = loggable.get().toString();
        bufferIndex++;
    }
}

void FileLogger::registerString(const std::string s) {

    if (bufferIndex >= bufferSize) {
        cout << "\n Writing log file.." << endl;
        writeFile();
        bufferIndex = 0;
    }

    buffer[bufferIndex] = s;
    bufferIndex++;

}

void FileLogger::writeFile() {

    array<string, bufferSize> a = buffer;
    thread t(&FileLogger::writeRoutine, this, a, bufferIndex);

    t.detach();
}

void FileLogger::writeRoutine(array<string, bufferSize> a, size_t tailIndex) {

    assert(tailIndex >= 1);

    stringstream ss;
    ss << path << "_" << ++id << "_";
    ss << std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
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
}