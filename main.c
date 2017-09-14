#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#pragma pack(1)
typedef struct {
    uint8_t id;
    uint16_t x, y, z;
} position_t;

typedef struct {
    uint8_t id;
    uint16_t distance;
} distance_t;

typedef struct {
    uint16_t frame_header;
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


void usage()
{
    printf("pkunwrap, a data receiver and unpacker for the IR-UWB PK-1000 system.\n\n"
                   "Usage:\tpku [-c connection (default:ip)] [host]\n");
    exit(1);
}

int main()
{
    usage();

    char pBuffer[] = { 0x37, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x19, 0x01,
                       0x01, 0x27, 0x02, 0x00, 0x39, 0x03, 0x00, 0x8B, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x27, 0x28 };
    ppk1000_t pk1000 = (ppk1000_t)pBuffer;

    printf("pk1000 size = %i\n", (int)sizeof(pk1000_t));
    printf("buffer size = %i\n", (int)sizeof(pBuffer));
    printf("frame = %i\n\n", pk1000->frame_header);

    printf("## pk1000 info ##\n");
    printf("counts = %i\n", pk1000->counts);
    printf("tag id = %i, x = %i, y = %i, z = %i\n\n", pk1000->tag.id, pk1000->tag.x, pk1000->tag.y, pk1000->tag.z);

    int i;
    printf("## Distances to tags ##\n");
    for(i=0; i<4; printf("tag id = %i, distance = %i\n", pk1000->tags[i].id, pk1000->tags[i++].distance));

    printf("\n## Positions of anchors ##\n");
    for(i=0; i<4; printf("tag id = %i, x = %i, y = %i, z = %i\n", pk1000->anchors[i].id, pk1000->anchors[i].x, pk1000->anchors[i].y, pk1000->anchors[i++].z));

    return 0;
}
