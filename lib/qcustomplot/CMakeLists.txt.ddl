cmake_minimum_required(VERSION 3.7)

project(qcustomplot-download NONE)

include(ExternalProject)
ExternalProject_Add(
        qcustomplot
        URL http://www.qcustomplot.com/release/2.0.0/QCustomPlot-source.tar.gz
        URL_HASH SHA512=66c648877ed0a14cf527ac8526788065f494119cc61993d20268a0046c8e6c5a1c73a61f50a73e6a39ec55435bc5f74c613ee21254293ddab34ba7c2484c4f5a
        DOWNLOAD_NO_PROGRESS 1
        SOURCE_DIR  "${CMAKE_BINARY_DIR}/qcustomplot-src"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
)


