//
// Created by kenwi on 18.09.17.
//

#ifndef PKU_PK1000_H
#define PKU_PK1000_H

#define BUFF_SIZE 1024

#pragma pack(1)

#include <stdint.h>

/*  BYTES               FLAG            NUM
int8_t buffer[] = {
        0x37, 0x38, // Frame header     0 1
        0x00,       // Tag ID           2
        0x01, 0x1c, // X                3 4
        0x01, 0x1e, // Y                5 6
        0xff, 0xc5, // Z                7 8

        0x00,       // Anchor ID 0 9    9
        0x02, 0x19, // Distance to tag  10 11
        0x01,       // Anchor ID 1      12
        0x01, 0x27, // Distance to tag  13 14
        0x02,       // Anchor ID 2      15
        0x00, 0x39, // Distance to tag  16 17
        0x03,       // Anchor ID 3      18
        0x00, 0x8B, // Distance to tag  19 20

        0x00,       // Anchor ID 0      21
        0x00, 0x00, // X                22 23
        0x00, 0x00, // Y                24 25
        0x00, 0x00, // Z                26 27

        0x01,       // Anchor ID 1 28   28
        0x00, 0x00, // X                29 30
        0x00, 0x00, // Y                31 32
        0x00, 0x00, // Z                33 34

        0x02,       // Anchor ID 2      35
        0x00, 0x00, // X                36 37
        0x00, 0x00, // Y                38 39
        0x00, 0x00, // Z                40 41

        0x03,       // Anchor ID 3      42
        0x00, 0x00, // X                43 44
        0x00, 0x00, // Y                45 46
        0x00, 0x00, // Z                47 48

        0x34,       // Counter          49
        0x27, 0x28  // Frame footer     50 51
};*/

typedef struct {
    int8_t id;
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t distance;
} anchor_t;

typedef struct {
    int8_t id;
    int16_t x;
    int16_t y;
    int16_t z;
} tag_t;

typedef struct {
    int16_t frame_header;
    tag_t   tag;
    anchor_t anchors[4];
    int8_t counts;
    int16_t frame_footer;
} pk1000_t, *ppk1000_t;

typedef struct {
    uint16_t frame_header;
    uint16_t frame_footer;
} pk1000_setup_t;

#endif //PKU_PK1000_H
