#include <ground_station/Application.h>


int main(int argc, char **argv) {

    Application app{argc, argv};
    app.run();
    return app.exec();

}
