#include <Application.h>
#include <ConfigParser/ConfigParser.h>


int
main(int argc, char **argv) {

    ConfSingleton::instance().loadConfig("config.json");

    Application app{argc, argv};
    app.run();

    return app.exec();

}
