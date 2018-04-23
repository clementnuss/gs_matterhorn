
#ifndef GS_MATTERHORN_CONFIGPARSERTESTS_H
#define GS_MATTERHORN_CONFIGPARSERTESTS_H

#include <QtTest/QtTest>
#include <ConfigParser/ConfigParser.h>
#include <QtCore/qt_windows.h>
#include <iostream>


class ConfigParserTests : public QObject {
Q_OBJECT

private slots:

    void configReadIsCorrect() {

        ConfigParserSingleton::getInstance().loadConfig("unittest_config.json");

        // Test return of default values
        QCOMPARE(ConfigParserSingleton::getInstance().getValue("", 0), 0);
        QCOMPARE(ConfigParserSingleton::getInstance().getValue("this.does.not.exists", std::string{"default"}),
                 std::string{"default"});

        // Test simple variables
        QCOMPARE(ConfigParserSingleton::getInstance().getValue("integer", 0), 1);
        QCOMPARE(ConfigParserSingleton::getInstance().getValue("nested.one", 0), 1);
        QCOMPARE(ConfigParserSingleton::getInstance().getValue("string", std::string{""}), std::string{"hello"});
        QCOMPARE(ConfigParserSingleton::getInstance().getValue("nested.string", std::string{""}), std::string{"world"});

        // Test list
        std::vector<int> v1 = ConfigParserSingleton::getInstance().getList<int>("array");
        QCOMPARE(v1[0], 1);
        QCOMPARE(v1[1], 2);
        QCOMPARE(v1[2], 3);

        std::vector<std::string> v2 = ConfigParserSingleton::getInstance().getList<std::string>("string_array");
        QCOMPARE(v2[0], "abc");
        QCOMPARE(v2[1], "def");
        QCOMPARE(v2[2], "ghi");
    }

};

#endif //GS_MATTERHORN_CONFIGPARSERTESTS_H
