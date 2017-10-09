#include <fstream>
#include <c++/iostream>
#include <sstream>
#include "FileLogger.h"

using namespace std;

FileLogger::FileLogger(std::string path, int entrySize) : path{path}, entrySize{entrySize}, id{0}, bufferIndex{0} {}

void FileLogger::writeData(vector<reference_wrapper<ILoggable>> data) {


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

    stringstream ss;
    ss << id << path;

    id++;

    std::ofstream outf(ss.str());

    if (!outf) {
        cerr << "Could not open file for writing." << endl;
        return;
    }

    for (int i = 0; i < bufferSize; i++) {
        outf << buffer[i] << endl;
    }

    outf.close();
    return;
}
