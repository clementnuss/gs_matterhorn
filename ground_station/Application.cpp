#include <QObject>
#include <iostream>
#include <DataHandlers/TelemetrySimulator.h>
#include <DataHandlers/Receiver/RadioReceiver.h>
#include <DataHandlers/TelemetryReplay.h>
#include "Application.h"

Application::Application(int &argc, char **argv) : qApplication_{argc, argv}, mainWidget_{nullptr}, workerThread_{} {

    /*
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
     */

}

void Application::run() {
    std::string path{R"(D:\EPFL\matterhorn\Eric Launch 18.11.2017\telemetry)"};

    TelemetryHandler *handler;
    try {
        handler = new TelemetryReplay(path);
        handler->startup();
    } catch (std::runtime_error &e) {
        std::cerr << "Error when starting the telemetry handler:\n" << e.what();
        return; // This prevents the worker from being instantiated
    }

    worker_ = new Worker(handler);
    worker_->moveToThread(&workerThread_);

    connectSlotsAndSignals();

    // Initialize UI status fields
    worker_->emitAllStatuses();
}

int Application::exec() {
    workerThread_.start();

    mainWidget_.show();

    qApplication_.exec();
}

void Application::connectSlotsAndSignals() {
    qRegisterMetaType<TelemetryReading>("TelemetryReading");
    qRegisterMetaType<QVector<QCPGraphData>>("QVector<QCPGraphData>&");
    qRegisterMetaType<vector<RocketEvent>>("vector<RocketEvent>&");
    qRegisterMetaType<GraphFeature>("GraphFeature");
    qRegisterMetaType<HandlerStatus>("HandlerStatus");


    QObject::connect(worker_,
                     &Worker::telemetryReady,
                     &mainWidget_,
                     &GSWidget::updateTelemetry);

    QObject::connect(worker_,
                     &Worker::loggingStatusReady,
                     &mainWidget_,
                     &GSWidget::updateLoggingStatus);

    QObject::connect(worker_,
                     &Worker::linkStatusReady,
                     &mainWidget_,
                     &GSWidget::updateLinkStatus);

    QObject::connect(worker_,
                     &Worker::newEventsReady,
                     &mainWidget_,
                     &GSWidget::updateEvents);

    QObject::connect(worker_,
                     &Worker::graphDataReady,
                     &mainWidget_,
                     &GSWidget::updateGraphData);

    QObject::connect(&workerThread_,
                     &QThread::started,
                     worker_,
                     &Worker::run);


    QObject::connect(&mainWidget_,
                     &GSWidget::toggleLogging,
                     worker_,
                     &Worker::updateLoggingStatus);
}


Application::~Application() {
    workerThread_.requestInterruption();
    workerThread_.quit();
    workerThread_.wait();
}