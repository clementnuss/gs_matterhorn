
#ifndef GS_MATTERHORN_APPLICATION_H
#define GS_MATTERHORN_APPLICATION_H


#include <QtWidgets/QApplication>
#include "UI/UIWidget.h"

class Application {

public:
    Application(int &argc, char **argv);

    ~Application();

    void run();

    int exec();

private:
    QApplication qApplication_;
    GSMainwindow gsMainWindow_;
    QThread workerThread_;
    Worker *worker_;

    void connectSlotsAndSignals();
};


#endif //GS_MATTERHORN_APPLICATION_H
