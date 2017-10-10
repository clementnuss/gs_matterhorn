#include "FileReader.h"

#include <utility>

using namespace std;

FileReader::FileReader(string path) : path{std::move(path)} {


}

FileReader::~FileReader() = default;