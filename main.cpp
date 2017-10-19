#include <iostream>
#include <thread>
#include <ground_station/RadioReceiver.h>
#include "General.h"

int runGUI(int argc, char *argv[]);

void testFunc() {
    std::cout << "Called from a thread.\t" << simple_test_function(0) << std::endl;
}

int main(int argc, char **argv) {

    RadioReceiver radioReceiver("COM11", SERIAL_BAUD_RATE);

    std::thread t{testFunc};
    t.join();

    return runGUI(argc, argv);
}
