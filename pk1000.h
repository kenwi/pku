//
// Created by kenwi on 18.09.17.
//

#ifndef PKU_PK1000_H
#define PKU_PK1000_H

#define BUFF_SIZE 1024

#pragma pack(1)

#include <stdint.h>

typedef struct {
    int8_t id;
    int16_t x, y, z;
} position_t;

typedef struct {
    int8_t id;
    int16_t distance;
} distance_t;

typedef struct {
    int16_t frame_header;
    position_t tag;
    distance_t tags[4];
    position_t anchors[4];
    uint8_t counts;
    uint16_t frame_footer;
} pk1000_t, *ppk1000_t;

typedef struct {
    uint16_t frame_header;
    uint16_t frame_footer;
} pk1000_setup_t;

#endif //PKU_PK1000_H
