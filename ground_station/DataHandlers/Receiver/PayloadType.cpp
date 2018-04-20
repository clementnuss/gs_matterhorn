
#include "PayloadType.h"

// Please keep this file and the comments up-to-date at all times

/*
 *  [TIMESTAMP | ACC | GYRO | MAG | TEMP | PRES | PITOT]    Field name
 *       4        6     6      6     4      4       2       Size in bytes 32
 */
const PayloadType PayloadType::TELEMETRY{CommunicationsConstants::TELEMETRY_TYPE, 32};

/*
 *  [TIMESTAMP | ACC | EULER | TEMP | PRES | PITOT]    Field name
 *       4       12    12      4      4       0        Size in bytes 36
 */
const PayloadType PayloadType::TELEMETRY_ERT18{CommunicationsConstants::TELEMETRY_ERT18_TYPE, 36};

/*
 *  [TIMESTAMP | EVENT CODE]                                Field name
 *       4          1                                       Size in bytes 5
 */
const PayloadType PayloadType::EVENT{CommunicationsConstants::EVENT_TYPE, 5};

/*
 *  [TIMESTAMP | PART CODE | VALUE]                         Field name
 *       4          1          2                            Size in bytes 7
 */
const PayloadType PayloadType::CONTROL{CommunicationsConstants::CONTROL_TYPE, 7};

/*
 *  [TIMESTAMP | SATS NUM | HDOP | LAT | LON | ALT ]        Field name
 *       4          1         4     4     4     4           Size in bytes 21
 */
const PayloadType PayloadType::GPS{CommunicationsConstants::GPS_TYPE, 21};

const std::map<int, PayloadType> PayloadType::TYPES_TABLE = PayloadType::createPayloadTypesMap();