#include <iostream>
#include <thread>
#include "General.h"

int runGUI(int argc, char *argv[]);

void testFunc() {
    std::cout << "Called from a thread.\t" << simple_test_function(0) << std::endl;
}

int main(int argc, char **argv) {

    std::thread t{testFunc};
    t.join();

    return runGUI(argc, argv);
}
