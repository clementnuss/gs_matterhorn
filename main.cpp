#include <iostream>
#include <ground_station/RadioReceiver.h>
#include <Boost/thread.hpp>
#include <Boost/program_options.hpp>

int runGUI(int argc, char *argv[]);

namespace po = boost::program_options;

int main(int argc, const char **argv) {

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "produce help message")
            ("serial_device,s", po::value<string>(), "System name of the serial port (e.g. COM1 or /dev/tty0");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("serial_device")) {
        cout << "Selected serial port: "
             << vm["serial_device"].as<string>() << ".\n";
    } else {
        cout << "No Serial device specified, exiting..\n";
        return -1;
    }

    boost::asio::io_service io{};
    RadioReceiver radioReceiver{io};

    try {
        radioReceiver.openSerialPort(vm["serial_device"].as<string>(), SERIAL_BAUD_RATE);
        boost::thread t(boost::bind(&boost::asio::io_service::run, &io));
        radioReceiver.asyncRead();
    } catch (runtime_error &e) {
        cerr << "Unable to instantiate the RadioReceiver, exiting software.\n" << e.what();
//        return -1;
    }

    char* empty_argv[1] = {const_cast<char *>("")};
    return runGUI(0, empty_argv);
}
