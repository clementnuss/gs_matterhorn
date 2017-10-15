
#ifndef GS_MATTERHORN_PROGRAMCONSTANTS_H
#define GS_MATTERHORN_PROGRAMCONSTANTS_H

namespace PrintConstants {
    static constexpr int PRECISION = 4;
    static constexpr int FIELD_WIDTH = 12;
    static constexpr char DELIMITER = ' ';
}

namespace TimeConstants {
    static constexpr int MSECS_IN_SEC = 1000;
}

namespace UIConstants {
    static constexpr int REFRESH_RATE = 50;
    static constexpr int MSECS_GRAPH_XRANGE = 10000;
}

namespace SimulatorConstants {
    static constexpr size_t MAX_RANDOM_VECTOR_LENGTH = 16;
}

namespace DataConstants {
    static constexpr int MAX_DATA_VECTOR_SIZE = UIConstants::MSECS_GRAPH_XRANGE
                                                * SimulatorConstants::MAX_RANDOM_VECTOR_LENGTH
                                                / UIConstants::REFRESH_RATE;
}

#endif //GS_MATTERHORN_PROGRAMCONSTANTS_H
