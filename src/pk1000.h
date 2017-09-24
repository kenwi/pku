//
// Created by kenwi on 18.09.17.
//
/*  BYTES               FLAG            INDEX
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

#ifndef PKU_PK1000_H
#define PKU_PK1000_H

#define BUFF_SIZE 1024

#pragma pack(1)

#include <stdint.h>
#include <stdlib.h>
#include "main.h"

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


//int connect_pk1000(struct application *app);

int16_t to_int16(int8_t a, int8_t b) {
    return ((a & 0xff) << 8) | (b & 0xff);
}

pk1000_t make_pk1000(int8_t buffer[]) {
    pk1000_t pk1000;
    pk1000.frame_header = to_int16(buffer[0], buffer[1]);
    pk1000.tag.id = buffer[3];//(buffer[3] & 0xff) << 8;
    pk1000.tag.x = to_int16(buffer[3], buffer[4]);
    pk1000.tag.y = to_int16(buffer[5], buffer[6]);
    pk1000.tag.z = to_int16(buffer[7], buffer[8]);

    /* Refactor this
    for(int i = 0; i<4; i++) {
        pk1000.anchors[i].id = buffer[9+3*i];
        pk1000.anchors[i].distance = to_int16(buffer[10+3*i], buffer[10+3*i+1]);
        pk1000.anchors[i].x = to_int16(buffer[22+6*i], buffer[23+6*i]);
        pk1000.anchors[i].y = to_int16(buffer[24+6*i], buffer[25+6*i]);
        pk1000.anchors[i].z = to_int16(buffer[26+6*i], buffer[27+6*i]);
    }*/

    pk1000.anchors[0].id = buffer[9];//(buffer[9] & 0xff) << 8;
    pk1000.anchors[0].distance = to_int16(buffer[10], buffer[11]);
    pk1000.anchors[0].x = to_int16(buffer[22], buffer[23]);
    pk1000.anchors[0].y = to_int16(buffer[24], buffer[25]);
    pk1000.anchors[0].z = to_int16(buffer[26], buffer[27]);

    pk1000.anchors[1].id = buffer[12];//(buffer[12] & 0xff) << 8;
    pk1000.anchors[1].distance = to_int16(buffer[13], buffer[14]);
    pk1000.anchors[1].x = to_int16(buffer[29], buffer[30]);
    pk1000.anchors[1].y = to_int16(buffer[31], buffer[32]);
    pk1000.anchors[1].z = to_int16(buffer[33], buffer[34]);

    pk1000.anchors[2].id = buffer[15];//(buffer[15] & 0xff) << 8;
    pk1000.anchors[2].distance = to_int16(buffer[16], buffer[17]);
    pk1000.anchors[2].x = to_int16(buffer[36], buffer[37]);
    pk1000.anchors[2].y = to_int16(buffer[38], buffer[39]);
    pk1000.anchors[2].z = to_int16(buffer[40], buffer[41]);

    pk1000.anchors[3].id = buffer[18];//(buffer[18] & 0xff) << 8;
    pk1000.anchors[3].distance = to_int16(buffer[19], buffer[20]);
    pk1000.anchors[3].x = to_int16(buffer[43], buffer[44]);
    pk1000.anchors[3].y = to_int16(buffer[45], buffer[46]);
    pk1000.anchors[3].z = to_int16(buffer[47], buffer[48]);

    pk1000.counts = buffer[49];//(buffer[49] & 0xff) << 8;
    pk1000.frame_footer = to_int16(buffer[50], buffer[51]);
    return pk1000;
}

int connect_pk1000(struct application *app) {
    app->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(app->port);
    serv_addr.sin_addr.s_addr = inet_addr(app->host);

    //fprintf(file, "Connecting to PK-1000 system host: %s, port: %i\n", app->host, app->port);

    //connect(app->sockfd, (struct sockaddr*)&serv_addr, sizeof serv_addr);
    //pthread_create(&receiver_thread, NULL, receiver, app);//(void*)&sockfd);
    //console(app->sockfd);


    return app->sockfd;
}

#endif //PKU_PK1000_H
