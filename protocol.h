#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t header;      
    float target_x;       
    float target_y;      
    uint8_t checksum;     
} TelemetryPacket;
#pragma pack(pop)


uint8_t calculate_checksum(uint8_t *data, int length) {
    uint8_t sum = 0;
    for(int i = 0; i < length; i++) {
        sum ^= data[i];
    }
    return sum;
}

#endif