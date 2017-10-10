cmake_minimum_required(VERSION 2.8.2)

project(qcustomplot-download NONE)

include(ExternalProject)
ExternalProject_Add(
        qcustomplot
        URL http://www.qcustomplot.com/release/2.0.0/QCustomPlot-source.tar.gz
        URL_HASH SHA512=66C648877ED0A14CF527AC8526788065F494119CC61993D20268A0046C8E6C5A1C73A61F50A73E6A39EC55435BC5F74C613EE21254293DDAB34BA7C2484C4F5A
        DOWNLOAD_NO_PROGRESS 1
        SOURCE_DIR  "${CMAKE_BINARY_DIR}/qcustomplot-src"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
)


