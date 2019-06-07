
#ifndef GS_MATTERHORN_APPLICATION_H
#define GS_MATTERHORN_APPLICATION_H

#include<list>
#include<vector>
#include "MainWorker.h"


class Application {

public:
    Application(int &argc, char **argv) : wrkr_() {};

    ~Application();

    void run();

    int exec();

private:
    Worker wrkr_;
};


#endif //GS_MATTERHORN_APPLICATION_H
