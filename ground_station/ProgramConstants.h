
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
    static constexpr int PRECISION = 2;

    // Refresh rates are in milliseconds
    static constexpr int NUMERICAL_VALUES_REFRESH_RATE = 100;
    static constexpr int REFRESH_RATE = 50;

    static constexpr int MSECS_GRAPH_XRANGE = 10000;
}

namespace SimulatorConstants {
    static constexpr size_t MAX_RANDOM_VECTOR_LENGTH = 16;
}

namespace DataConstants {
    static constexpr double DELETION_FACTOR = 0.25;
    static constexpr int MAX_DATA_VECTOR_SIZE =
            (1.0 + DELETION_FACTOR) * (UIConstants::MSECS_GRAPH_XRANGE / UIConstants::REFRESH_RATE);
}

#endif //GS_MATTERHORN_PROGRAMCONSTANTS_H
