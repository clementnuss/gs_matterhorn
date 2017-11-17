#include <Application.h>
#include <QtQml/QtQml>
#include <3D/Line.h>


int main(int argc, char **argv) {

    // Register QML and Qt types, must be done before loading the UI
    qmlRegisterType<TraceData>("ch.epfl.ert.groundstation", 1, 0, "TraceData");
    qmlRegisterType<Line>("ch.epfl.ert.groundstation", 1, 0, "LineCPP");
    qRegisterMetaType<TelemetryReading>("TelemetryReading");
    qRegisterMetaType<QVector<QCPGraphData>>("QVector<QCPGraphData>&");
    qRegisterMetaType<vector<RocketEvent>>("vector<RocketEvent>&");
    qRegisterMetaType<GraphFeature>("GraphFeature");
    qRegisterMetaType<HandlerStatus>("HandlerStatus");

    Application app{argc, argv};
    app.run();
    return app.exec();

}
