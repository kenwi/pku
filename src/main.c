#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <getopt.h>

#include <SFML/System.h>
#include <SFML/Window.h>
#include <SFML/Graphics.h>
#include <SFML/OpenGL.h>

#include "pk1000.h"
#include "sfml.h"

pthread_cond_t console_cv;
pthread_mutex_t console_cv_lock;
pthread_t receiver_thread;
FILE *file;

void usage() {
    printf("pkunwrap, a data receiver and unpacker for the IR-UWB PK-1000 system.\n\n"
           "Usage:\tpku [-options] filename\n"
            "\t[-c connects with default settings]\n"
            "\t[-i print sample data]\n"
            "\t[-x print sample data as hex]\n"
            "\t[-n collect n samples and terminate]\n"
            "\t[-w output warnings]\n"
            "\t[-h host (default: 192.168.0.19)]\n"
            "\t[-p port (default: 8080)]\n"
            "\tfilename (default: '-' dumps samples to stdout)\n"
    );
    exit(EXIT_FAILURE);
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
            if(app->print_warnings == 1) {
                fprintf(file, "%s: WARNING: more than 1 sample received [%i]\n", t_str, num_samples);
            }
            /*
            if(app->print_warnings == 1) {
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



void init_application(struct application *app, int argc, char **argv) {
    app->num_samples_terminate = 0;
    app->connect_to_pk1000 = 0;
    app->print_raw_sample = 0;
    app->sockfd = 0;
    app->port = 8080;
    app->host = "192.168.0.19";
    app->filename = "-";
    app->print_warnings = 0;

    int opt;
    while((opt = getopt(argc, argv, "cixh:p:n:w")) != -1) {
        switch(opt) {
            case 'c':
                app->connect_to_pk1000 = 1;
                break;
            case 'p':
                app->port = atoi(optarg);
                fprintf(stdout, "Settings changed, port: %i\n", app->port);
                break;
            case 'x':
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
            case 'w':
                app->print_warnings = 1;
                fprintf(stdout, "Settings changed, print_warnings: %i\n", app->print_warnings);
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
    argc == 1 ? usage() : init_application(&app, argc, argv), init_sfml(&app);
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
        run_sfml(&app);
    }


    /* cleanup */
    if(file != stdout) {
        fclose(file);
    }

    return 0;
}
