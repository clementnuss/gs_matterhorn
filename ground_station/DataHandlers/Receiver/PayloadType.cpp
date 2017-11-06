
#include "PayloadType.h"
#include "Factories.h"

const PayloadType PayloadType::TELEMETRY{0, 34, &Factories::telemetryReadingFactory};