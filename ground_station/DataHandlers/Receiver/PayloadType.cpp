
#include "PayloadType.h"
#include "Factories.h"

const PayloadType PayloadType::TELEMETRY{0, 32, &Factories::telemetryReadingFactory};
const PayloadType PayloadType::EVENT{1, 1, &Factories::telemetryEventFactory};
const PayloadType PayloadType::CONTROL{2, 3, &Factories::telemetryControlFactory};

const std::map<int, PayloadType> PayloadType::TYPES_TABLE = PayloadType::createPayloadTypesMap();