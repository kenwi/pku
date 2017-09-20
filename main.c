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

pthread_cond_t console_cv;
pthread_mutex_t console_cv_lock;
pthread_t receiver_thread;
FILE *file;

struct application {
    int num_samples_terminate;
    int connect_to_pk1000;
    int sample_info;
    int port;
    int sockfd;

    char *host;
    char *filename;
};

void usage();
void test_casting();
char getTime(char *str);
void *receiver(void *sfd);
void console(int sockfd);
int connect_pk1000(struct application *app);
void init_application(struct application *app, int argc, char **argv);

void usage()
{
    printf("pkunwrap, a data receiver and unpacker for the IR-UWB PK-1000 system.\n\n"
           "Usage:\tpku [-options] filename\n"
            "\t[-c connects with default settings]\n"
            "\t[-i only print sample info]\n"
            "\t[-t test casting]\n"
            "\t[-n collect n samples and terminated]\n"
            "\t[-h host (default: 192.168.0.19)]\n"
            "\t[-p port (default: 8080)]\n"
            "\tfilename (default: '-' dumps samples to stdout)\n"
    );
    exit(EXIT_FAILURE);
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

char getTime(char *str){
    time_t time_now;
    struct tm *cal_time;

    time_now = time(NULL);
    cal_time = localtime(&time_now);
    strftime(str, 50, "%Y-%m-%d, %H:%M:%S", cal_time);
}

void *receiver(void *sfd)
{
    struct application *app = (struct application*)sfd;

    char buffer[BUFF_SIZE] = {0};
    int num_samples = 0;
    int run = 1;
    ssize_t readlen;
    char t_str[50];

    while(run) {
        memset(buffer, 0, sizeof buffer);
        readlen = read(app->sockfd, buffer, sizeof buffer);

        if(readlen < 1) {
            fprintf(file, "Error in receiving data from PK-1000. Cleaning up thread.\n");
            close(app->sockfd);
            pthread_exit(0);
        }

        pthread_mutex_lock(&console_cv_lock);

        num_samples += readlen/52;
        getTime(t_str);
        fprintf(file, "%s: Sample [%i] received. length: %i bytes, hex: %s, status: %s\n", t_str, num_samples,  (int)readlen, buffer, readlen == 52 ? "OK" : "BAD");

        /*
        if(sample_info) {
            fprintf(file, "%s: Sample [%i] received. length: %i bytes, hex: %s, status: %s\n", t_str, num_samples,  (int)readlen, buffer, readlen == 52 ? "OK" : "BAD");
        }
        else if(readlen/52 == 1) {
            ppk1000_t pk1000 = (ppk1000_t)buffer;
            fprintf(file, "## Distances to tags ##\n");
            for(int i=0; i<4; fprintf(file, "%s: tag id = %i, distance = %i\n", t_str, pk1000->tags[i].id, pk1000->tags[i++].distance));
        }*/

        pthread_cond_signal(&console_cv);
        pthread_mutex_unlock(&console_cv_lock);
    }
}

void console(int sockfd)
{
    char buffer[BUFF_SIZE];
    memset(buffer, 0, sizeof buffer);

    while(1) {
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

int connect_pk1000(struct application *app) {
    void *ptr = app;
    struct sockaddr_in serv_addr;

    app->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(app->port);
    serv_addr.sin_addr.s_addr = inet_addr(app->host);


    printf("Connecting to PK-1000 system host: %s\n", app->host);
    connect(app->sockfd, (struct sockaddr*)&serv_addr, sizeof serv_addr);
    pthread_create(&receiver_thread, NULL, receiver, ptr);//(void*)&sockfd);
    console(app->sockfd);

    return app->sockfd;
}

void init_application(struct application *app, int argc, char **argv)
{
    app->num_samples_terminate = 0;
    app->connect_to_pk1000 = 0;
    app->sample_info = 0;
    app->sockfd = 0;
    app->port = 8080;
    app->host = "192.168.0.19";
    app->filename = "-";


    int opt;
    while((opt = getopt(argc, argv, "cith:p:n:")) != -1) {
        switch(opt) {
            case 'c':
                app->connect_to_pk1000 = 1;
                break;

            case 'p':
                app->port = atoi(optarg);
                break;

            case 'i':
                app->sample_info = 1;
                break;

            case 't':
                test_casting();
                exit(EXIT_SUCCESS);

            case 'h':
                app->connect_to_pk1000 = 1;
                app->host = strdup(optarg);
                break;

            case 'n':
                fprintf(stdout, "Settings changed, num_samples_terminate: %i\n", app->num_samples_terminate);
                app->num_samples_terminate = atoi(optarg);
                break;
            default:
                usage();
                break;
        }
    }
}

int main(int argc, char **argv)
{
    struct application app;
    init_application(&app, argc, argv);

    if(argc == 1)
        usage();

    app.filename = argc <= optind ? "-" : argv[optind];
    if(strcmp(app.filename, "-") == 0) {
        file = stdout;
    } else {
        file = fopen(app.filename, "wb");
        if(!file) {
            fprintf(stderr, "Failed to open %s\n", app.filename);
            exit(EXIT_FAILURE);
        }
    }

    if(app.connect_to_pk1000) {
        connect_pk1000(&app);
    }

    /* cleanup */
    if(file != stdout)
        fclose(file);

    return 0;
}
