
#include "PayloadType.h"
#include "Factories.h"

const PayloadType PayloadType::TELEMETRY{CommunicationsConstants::TELEMETRY_TYPE, 32,
                                         &Factories::telemetryReadingFactory};
const PayloadType PayloadType::EVENT{CommunicationsConstants::EVENT_TYPE, 5, &Factories::telemetryEventFactory};
const PayloadType PayloadType::CONTROL{CommunicationsConstants::CONTROL_TYPE, 7, &Factories::telemetryControlFactory};

const std::map<int, PayloadType> PayloadType::TYPES_TABLE = PayloadType::createPayloadTypesMap();