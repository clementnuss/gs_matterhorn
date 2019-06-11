
#include "Application.h"
#include "MainWorker.h"

void
Application::run() {

    wrkr_.run();

}

int
Application::exec() {

    return 0;
}

Application::~Application() {
    puts("done !");

};