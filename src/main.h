//
// Created by kenwi on 24.09.17.
//

#ifndef PKU_MAIN_H_H
#define PKU_MAIN_H_H

#include <SFML/Window.h>
#include <SFML/Graphics/Types.h>

struct application {
    int num_samples_terminate;
    int connect_to_pk1000;
    int print_raw_sample;
    int port;
    int sockfd;
    int print_warnings;
    int print_sample_data;

    char *host;
    char *filename;
    sfEvent event;
    sfRenderWindow* renderWindow;
    int width;
    int height;
};

void init_application(struct application *app, int argc, char **argv);

char getTime(char *str) {
    time_t time_now;
    struct tm *cal_time;

    time_now = time(NULL);
    cal_time = localtime(&time_now);
    strftime(str, 50, "%Y-%m-%d, %H:%M:%S", cal_time);
}

#endif //PKU_MAIN_H_H
