#ifndef GS_MATTERHORN_FILEREADER_H
#define GS_MATTERHORN_FILEREADER_H

#include <boost/filesystem.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string/split.hpp>

template<class U>
class FileReader {

public:
    explicit FileReader(const std::string &path, U (*lineReaderFunc)(const std::string &)) :
            filePath_{path}, inputStream_{nullptr}, lineReaderFunc_{lineReaderFunc} {

        if (boost::filesystem::exists(filePath_)) {
            if (boost::filesystem::is_regular_file((filePath_))) {

                inputStream_ = new boost::filesystem::ifstream(filePath_, std::ios::in);

            } else {
                throw (std::runtime_error(filePath_.string() + " exists, but is not a regular file or directory\n"));
            }
        } else {
            throw (std::runtime_error(filePath_.string() + " does not exist\n"));
        }
    }

    virtual ~FileReader() {
        delete inputStream_;
    };

    virtual std::vector<U> readFile() {
        std::vector<U> v{};
        std::string receptacle{};
        while (getline(*inputStream_, receptacle)) {
            v.push_back(lineReaderFunc_(receptacle));
        }
        return v;
    }

    virtual U interpretLine(std::string &str) {
        return lineReaderFunc_(str);
    }

    const boost::filesystem::path filePath() {
        return filePath_;
    }

private:
    boost::filesystem::path filePath_;
    boost::filesystem::ifstream *inputStream_;

    U (*lineReaderFunc_)(const std::string &);
};


#endif //GS_MATTERHORN_FILEREADER_H
