#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <getopt.h>

#include "pk1000.h"

#define BUFF_SIZE 1024

int sample_info = 0;
static unsigned short port = 8080;
char *host = "192.168.0.19";

pthread_cond_t console_cv;
pthread_mutex_t console_cv_lock;
FILE *file;

void usage()
{
    printf("pkunwrap, a data receiver and unpacker for the IR-UWB PK-1000 system.\n\n"
           "Usage:\tpku [-options] filename\n"
            "\t[-c connects with default settings]\n"
            "\t[-i only print sample info]\n"
            "\t[-t test casting]\n"
            "\t[-h host (default: 192.168.0.19)]\n"
            "\t[-p port (default: 8080)]\n"
            "\tfilename (default: '-' dumps samples to stdout)\n"
    );
    exit(EXIT_FAILURE);
}

void *receiver(void *sfd)
{
    char buffer[BUFF_SIZE] = {0};
    int sockfd = *(int*)sfd;
    int numsamples = 0;
    ssize_t readlen;

    while(1) {
        memset(buffer, 0, sizeof buffer);
        readlen = read(sockfd, buffer, sizeof buffer);
        if(readlen < 1)
            continue;

        pthread_mutex_lock(&console_cv_lock);
        numsamples += readlen/52;

        if(sample_info) {
            fprintf(file, "Sample [%i] received. length: %i bytes, hex: %s, status: %s\n", numsamples,  (int)readlen, buffer, readlen == 52 ? "OK" : "BAD");
        }
        else if(readlen/52 == 1) {
            ppk1000_t pk1000 = (ppk1000_t)buffer;
            /*fprintf(file, "counts: %i,tag id = %i, x = %i, y = %i, z = %i\n",
                    pk1000->counts, pk1000->tag.id, pk1000->tag.x, pk1000->tag.y, pk1000->tag.z);
            */
            int i;
            fprintf(file, "## Distances to tags ##\n");
            for(i=0; i<4; fprintf(file, "tag id = %i, distance = %i\n", pk1000->tags[i].id, pk1000->tags[i++].distance));

            //fprintf(file, "\n## Positions of anchors ##\n");
            //for(i=0; i<4; fprintf(file, "tag id = %i, x = %i, y = %i, z = %i\n", pk1000->anchors[i].id, pk1000->anchors[i].x, pk1000->anchors[i].y, pk1000->anchors[i++].z));
        }


        pthread_cond_signal(&console_cv);
        pthread_mutex_unlock(&console_cv_lock);
    }
}

void console(int sockfd)
{
    char buffer[BUFF_SIZE];
    memset(buffer, 0, sizeof buffer);

    while(1)     {
        fgets(buffer, sizeof buffer, stdin);
        buffer[strlen(buffer)-1] = '\0';

        if(strcmp(buffer, "") == 0)
            continue;
        if(strncmp(buffer, "exit", 4) == 0) {
            pthread_mutex_destroy(&console_cv_lock);
            pthread_cond_destroy(&console_cv);
            _exit(EXIT_SUCCESS);
        }
    }
}

void connect_pk1000() {
    int sockfd;
    struct sockaddr_in serv_addr;

    pthread_t receiver_thread;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(host);

    printf("Connecting to PK-1000 system host: %s\n", host);
    connect(sockfd, (struct sockaddr*)&serv_addr, sizeof serv_addr);
    pthread_create(&receiver_thread, NULL, receiver, (void*)&sockfd);
    console(sockfd);
}

void test_casting() {
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
}

int main(int argc, char **argv)
{
    char *filename = NULL;
    int connect_to_pk1000 = 0;

    int opt;
    while((opt = getopt(argc, argv, "cith:p:")) != -1) {
        switch(opt) {
            case 'c':
                connect_to_pk1000  = 1;
                break;

            case 'p':
                port = atoi(optarg);
                printf("Setting changed, port: %i\n", port);
                break;

            case 'i':
                sample_info = 1;
                printf("Setting changed, sample_info: %i\n", sample_info);
                break;

            case 't':
                printf("Internal testing of casting\n");
                test_casting();
                exit(EXIT_SUCCESS);

            case 'h':
                host = strdup(optarg);
                connect_to_pk1000 = 1;
                printf("Setting changed, host: %s\n", host);
                break;
            default:
                usage();
                break;
        }
    }
    if(argc == 1)
        usage();

    filename = argc <= optind ? "-" : argv[optind];
    if(strcmp(filename, "-") == 0) {
        file = stdout;
    } else {
        file = fopen(filename, "wb");
        if(!file) {
            fprintf(stderr, "Failed to open %s\n", filename);
            exit(EXIT_FAILURE);
        }
    }

    if(connect_to_pk1000)
        connect_pk1000();

    /* cleanup */
    if(file != stdout)
        fclose(file);

    return 0;
}
