
#include "PayloadType.h"
#include "Factories.h"

const PayloadType PayloadType::TELEMETRY{0, 32, &Factories::telemetryReadingFactory};