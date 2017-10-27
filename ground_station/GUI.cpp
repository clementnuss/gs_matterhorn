#include "UIWidget.h"
#include <Boost/thread.hpp>
#include <Boost/program_options.hpp>


namespace po = boost::program_options;

int run(int argc, char **argv) {


    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "produce help message")
            ("serial_device,s", po::value<string>(), "System name of the serial port (e.g. COM1 or /dev/tty0");

    po::variables_map variablesMap;
    po::store(po::parse_command_line(argc, argv, desc), variablesMap);
    po::notify(variablesMap);

    if (variablesMap.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    if (variablesMap.count("serial_device")) {
        cout << "Selected serial port: "
             << variablesMap["serial_device"].as<string>() << ".\n";
    } else {
        cout << "No Serial device specified, exiting..\n";
        return -1;
    }

    char *empty_argv[1] = {const_cast<char *>("")};


    QApplication a(argc, argv);
    GSWidget w{nullptr, variablesMap["serial_device"].as<string>()};
    w.show();

    return a.exec();
}
