#include <Application.h>
#include <ConfigParser/ConfigParser.h>


int main(int argc, char **argv) {

    qRegisterMetaType<SensorsPacket>("TelemetryReading");
    qRegisterMetaType<QVector<QCPGraphData>>("QVector<QCPGraphData>&");
    qRegisterMetaType<QVector<QVector3D>>("QVector<QVector3D>&");
    qRegisterMetaType<vector<EventPacket>>("vector<RocketEvent>&");
    qRegisterMetaType<GraphFeature>("GraphFeature");
    qRegisterMetaType<HandlerStatus>("HandlerStatus");

    ConfSingleton::instance().loadConfig("config.json");

    Application app{argc, argv};
    app.run();

    return app.exec();

}
