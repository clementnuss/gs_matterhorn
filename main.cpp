#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <iostream>
#include <thread>
#include "General.h"


void testFunc() {
    std::cout << "Called from a thread.\t" << simple_test_function(0) << std::endl;
}

int main(int argc, char **argv) {


    std::thread t{testFunc};
    t.join();

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/ground_station/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    std::cout << "Hello World!";
    std::cout << std::flush;

    return app.exec();

}
