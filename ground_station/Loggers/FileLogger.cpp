#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <cassert>
#include "FileLogger.h"
#include <boost/filesystem.hpp>


using namespace std;

FileLogger::FileLogger(const std::string &path) :
        path_{path}, id_{0}, bufferIndex_{0} {}

FileLogger::~FileLogger() {
    close();
}

void
FileLogger::close() {
    cout << "Flushing file logger @ " << path_ << endl;
    if (bufferIndex_ != 0) {
        writeFile();
        bufferIndex_ = 0;
    }
}

void
FileLogger::registerData(const vector<reference_wrapper<ILoggable>> &data) {

    for (const auto loggable : data) {
        registerString(loggable.get().toString());
    }
}

void
FileLogger::registerString(const std::string s) {

    if (bufferIndex_ >= bufferSize) {
        cout << "\n Writing log file.." << endl;
        writeFile();
        bufferIndex_ = 0;
    }

    buffer[bufferIndex_] = s;
    bufferIndex_++;

}

void
FileLogger::writeFile() {

    array<string, bufferSize> a = buffer;
    thread t(&FileLogger::writeRoutine, a, bufferIndex_, path_, id_++);

    t.detach();
}

void
FileLogger::writeRoutine(array<string, bufferSize> a, size_t tailIndex, std::string path, size_t id) {

    assert(tailIndex >= 1);

    static std::string delimiter = "/";
    std::string directory_str = path.substr(0, path.find(delimiter)); // find the path of the log file
    boost::filesystem::path dir(directory_str);


    if (!(boost::filesystem::exists(dir))) {
        std::cout <<path << "does not exist" << std::endl;

        if (boost::filesystem::create_directory(dir))
            std::cout << "directory now successfully created !" << std::endl;
    }

    stringstream ss;
    ss << path << "_" << setw(3) << setfill('0') << id;
    ss << "_" << std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    ofstream fileOutput(ss.str());

    if (!fileOutput) {
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