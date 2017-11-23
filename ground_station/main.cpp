#include <Application.h>

int main(int argc, char **argv) {

    // Register QML and Qt types, must be done before loading the UI
    qRegisterMetaType<TelemetryReading>("TelemetryReading");
    qRegisterMetaType<QVector<QCPGraphData>>("QVector<QCPGraphData>&");
    qRegisterMetaType<QVector<QVector3D>>("QVector<QVector3D>&");
    qRegisterMetaType<vector<RocketEvent>>("vector<RocketEvent>&");
    qRegisterMetaType<GraphFeature>("GraphFeature");
    qRegisterMetaType<HandlerStatus>("HandlerStatus");

    Application app{argc, argv};
    app.run();
    return app.exec();

}
