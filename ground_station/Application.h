
#ifndef GS_MATTERHORN_APPLICATION_H
#define GS_MATTERHORN_APPLICATION_H


#include <QtWidgets/QApplication>
#include "UIWidget.h"

class Application {

public:
    Application(int &argc, char **argv);

    ~Application();

    void run();

    int exec();

private:
    QApplication qApplication_;
    GSWidget mainWidget_;
    QThread workerThread_;
    Worker *worker_;

    void connectSlotsAndSignals();
};


#endif //GS_MATTERHORN_APPLICATION_H
