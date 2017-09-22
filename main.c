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
    int print_raw_sample;
    int port;
    int sockfd;
    int verbose;
    int print_sample_data;

    char *host;
    char *filename;
};

void usage();
char getTime(char *str);
void *receiver(void *sfd);
void console(int sockfd);
void init_application(struct application *app, int argc, char **argv);
int connect_pk1000(struct application *app);

int16_t to_int(int8_t a, int8_t b) {
    return ((a & 0xff) << 8) | (b & 0xff);
}

pk1000_t make_pk1000(int8_t buffer[]) {
    pk1000_t pk1000;
    pk1000.frame_header = to_int(buffer[0], buffer[1]);
    pk1000.tag.id = buffer[3];//(buffer[3] & 0xff) << 8;
    pk1000.tag.x = to_int(buffer[3], buffer[4]);
    pk1000.tag.y = to_int(buffer[5], buffer[6]);
    pk1000.tag.z = to_int(buffer[7], buffer[8]);

    pk1000.anchors[0].id = buffer[9];//(buffer[9] & 0xff) << 8;
    pk1000.anchors[0].distance = to_int(buffer[10], buffer[11]);
    pk1000.anchors[0].x = to_int(buffer[22], buffer[23]);
    pk1000.anchors[0].y = to_int(buffer[24], buffer[25]);
    pk1000.anchors[0].z = to_int(buffer[26], buffer[27]);

    pk1000.anchors[1].id = buffer[12];//(buffer[12] & 0xff) << 8;
    pk1000.anchors[1].distance = to_int(buffer[13], buffer[14]);
    pk1000.anchors[1].x = to_int(buffer[29], buffer[30]);
    pk1000.anchors[1].y = to_int(buffer[31], buffer[32]);
    pk1000.anchors[1].z = to_int(buffer[33], buffer[34]);

    pk1000.anchors[2].id = buffer[15];//(buffer[15] & 0xff) << 8;
    pk1000.anchors[2].distance = to_int(buffer[16], buffer[17]);
    pk1000.anchors[2].x = to_int(buffer[36], buffer[37]);
    pk1000.anchors[2].y = to_int(buffer[38], buffer[39]);
    pk1000.anchors[2].z = to_int(buffer[40], buffer[41]);

    pk1000.anchors[3].id = buffer[18];//(buffer[18] & 0xff) << 8;
    pk1000.anchors[3].distance = to_int(buffer[19], buffer[20]);
    pk1000.anchors[3].x = to_int(buffer[43], buffer[44]);
    pk1000.anchors[3].y = to_int(buffer[45], buffer[46]);
    pk1000.anchors[3].z = to_int(buffer[47], buffer[48]);

    pk1000.counts = buffer[49];//(buffer[49] & 0xff) << 8;
    pk1000.frame_footer = to_int(buffer[50], buffer[51]);
    return pk1000;
}
/*
position_t make_position(int8_t bytes[]) {
    position_t position;
    position.id = (bytes[0] & 0xff) << 8;
    position.x = to_int(bytes[1], bytes[2]);
    position.y = to_int(bytes[3], bytes[4]);
    position.z = to_int(bytes[5], bytes[6]);
    return position;
}

distance_t make_distance(int8_t bytes[]) {
    distance_t distance;
    distance.id = bytes[0];
    distance.distance = to_int(bytes[1], bytes[2]);
    return distance;
}
*/

void usage() {
    printf("pkunwrap, a data receiver and unpacker for the IR-UWB PK-1000 system.\n\n"
           "Usage:\tpku [-options] filename\n"
            "\t[-c connects with default settings]\n"
            "\t[-r print sample hex]\n"
            "\t[-i print sample data]\n"
            "\t[-n collect n samples and terminate]\n"
            "\t[-v verbose output]\n"
            "\t[-h host (default: 192.168.0.19)]\n"
            "\t[-p port (default: 8080)]\n"
            "\tfilename (default: '-' dumps samples to stdout)\n"
    );
    exit(EXIT_FAILURE);
}

char getTime(char *str) {
    time_t time_now;
    struct tm *cal_time;

    time_now = time(NULL);
    cal_time = localtime(&time_now);
    strftime(str, 50, "%Y-%m-%d, %H:%M:%S", cal_time);
}

void *receiver(void *sfd) {
    struct application *app = (struct application*)sfd;

    int8_t buffer[BUFF_SIZE] = {0};
    int total_samples = 0;
    int num_samples = 0;
    int run = 1;
    ssize_t readlen;
    char t_str[50];

    while(run) {
        memset(buffer, 0, sizeof buffer);
        readlen = read(app->sockfd, buffer, sizeof buffer);
        getTime(t_str);
        pthread_mutex_lock(&console_cv_lock);

        if(readlen < 1) {
            fprintf(file, "%s: Error in receiving data from PK-1000. Cleaning up thread.\n", t_str);
            close(app->sockfd);
            pthread_exit(0);
        }
        num_samples = readlen/sizeof(pk1000_t);
        total_samples += num_samples;
        if(num_samples > 1) {
            if(app->verbose == 1) {
                fprintf(file, "%s: WARNING: more than 1 sample received [%i]\n", t_str, num_samples);
            }
            /*
            if(app->verbose == 1) {
                fprintf(file, "%s: Sample [%i] received. length: %i bytes, hex 0: %02X, hex 1: %02X status: %s\n", t_str, total_samples,  (int)readlen, buffer[0], buffer[1], readlen == 52 ? "OK" : "BAD");
            }*/
        }
        if(app->print_raw_sample == 1) {
            fprintf(file, "%s: ", t_str);
            for(int i=0; i<readlen; i+=2) {
                fprintf(file, "%02x%02x ", buffer[i], buffer[i+1]);
            }
            fprintf(file, "\n");
        }
        if(app->print_sample_data == 1) {
            pk1000_t pk1000 = make_pk1000(buffer);
            fprintf(file, "\t\tTag0 \t\tAnc%i\t\tAnc%i\t\tAnc%i\t\tAnc%i\n", pk1000.anchors[0].id, pk1000.anchors[1].id, pk1000.anchors[2].id, pk1000.anchors[3].id);
            fprintf(file, "Range(cm)\t\t\t%i\t\t%i\t\t%i\t\t%i\n",pk1000.anchors[0].distance, pk1000.anchors[1].distance, pk1000.anchors[2].distance, pk1000.anchors[3].distance);
            fprintf(file, "X(cm)\t\t%i\t\t%i\t\t%i\t\t%i\t\t%i\n", pk1000.tag.x, pk1000.anchors[0].x, pk1000.anchors[1].x, pk1000.anchors[2].x, pk1000.anchors[3].x);
            fprintf(file, "Y(cm)\t\t%i\t\t%i\t\t%i\t\t%i\t\t%i\n", pk1000.tag.y, pk1000.anchors[0].y, pk1000.anchors[1].y, pk1000.anchors[2].y, pk1000.anchors[3].y);
            fprintf(file, "Z(cm)\t\t%i\t\t%i\t\t%i\t\t%i\t\t%i\n\n", pk1000.tag.z, pk1000.anchors[0].z, pk1000.anchors[1].z, pk1000.anchors[2].z, pk1000.anchors[3].z);
        }
        if(app->num_samples_terminate > 0) {
            if(total_samples >= app->num_samples_terminate) {
                fprintf(file, "%s: Max number of samples collected: %i. Terminating.\n", t_str, total_samples);
                close(app->sockfd);
                pthread_exit(0);
            }
        }
        pthread_cond_signal(&console_cv);
        pthread_mutex_unlock(&console_cv_lock);
    }
}

void console(int sockfd) {
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
    app->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(app->port);
    serv_addr.sin_addr.s_addr = inet_addr(app->host);

    fprintf(file, "Connecting to PK-1000 system host: %s, port: %i\n", app->host, app->port);

    connect(app->sockfd, (struct sockaddr*)&serv_addr, sizeof serv_addr);
    pthread_create(&receiver_thread, NULL, receiver, app);//(void*)&sockfd);
    console(app->sockfd);

    return app->sockfd;
}

void init_application(struct application *app, int argc, char **argv) {
    app->num_samples_terminate = 0;
    app->connect_to_pk1000 = 0;
    app->print_raw_sample = 0;
    app->sockfd = 0;
    app->port = 8080;
    app->host = "192.168.0.19";
    app->filename = "-";
    app->verbose = 0;

    int opt;
    while((opt = getopt(argc, argv, "cirh:p:n:v")) != -1) {
        switch(opt) {
            case 'c':
                app->connect_to_pk1000 = 1;
                break;
            case 'p':
                app->port = atoi(optarg);
                fprintf(stdout, "Settings changed, port: %i\n", app->port);
                break;
            case 'r':
                app->print_raw_sample = 1;
                fprintf(stdout, "Settings changed, print_raw_sample: %i\n", app->print_raw_sample);
                break;
            case 'h':
                app->connect_to_pk1000 = 1;
                app->host = strdup(optarg);
                fprintf(stdout, "Settings changed, host: %s\n", app->host);
                break;
            case 'n':
                app->num_samples_terminate = atoi(optarg);
                fprintf(stdout, "Settings changed, num_samples_terminate: %i\n", app->num_samples_terminate);
                break;
            case 'v':
                app->verbose = 1;
                fprintf(stdout, "Settings changed, verbose: %i\n", app->verbose);
                break;
            case 'i':
                app->print_sample_data = 1;
                fprintf(stdout, "Settings changed, print_sample_data: %i\n", app->print_sample_data);
                break;
            default:
                usage();
                break;
        }
    }
}

int main(int argc, char **argv) {
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
    if(file != stdout) {
        fclose(file);
    }

    return 0;
}
