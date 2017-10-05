#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

typedef struct{
    float yaw;
    float pitch;
    float roll;
    bool validity;
} ypr_reading_t;

typedef struct{
    float value;
    bool validity;
} data_reading_t;

typedef struct{
    data_reading_t altitude;
    data_reading_t speed;
    data_reading_t acceleration;
    data_reading_t pressure;
    data_reading_t temperature;
    ypr_reading_t ypr;
} telemetry_t;

#endif // DATASTRUCTS_H
