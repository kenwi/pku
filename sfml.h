//
// Created by kenwi on 24.09.17.
//

#ifndef PKU_SFML_H
#define PKU_SFML_H

#include <SFML/Graphics.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "pk1000.h"

extern FILE *file;

void init_sfml(struct application *app) {
    app->width = 800;
    app->height = 500;
    sfVideoMode mode = {app->width, app->height, 32};
    app->renderWindow = sfRenderWindow_create(mode, "pku", sfClose, NULL);
}

void render_sfml(struct application *app) {
    sfRenderWindow_clear(app->renderWindow, sfBlue);

    sfRenderWindow_display(app->renderWindow);
}

void run_sfml(struct application *app) {
    int8_t buffer[BUFF_SIZE] = {0};

    while(sfRenderWindow_isOpen(app->renderWindow)) {
        while( sfRenderWindow_pollEvent(app->renderWindow, &app->event) ) {
            if( app->event.type == sfEvtClosed || (app->event.type == sfEvtKeyPressed && app->event.key.code == sfKeyEscape) ) {
                sfRenderWindow_close(app->renderWindow);
            }
        }

        memset(buffer, 0, sizeof buffer);
        if(read(app->sockfd, buffer, sizeof buffer) > 0) {
            pk1000_t pk1000 = make_pk1000(buffer);
            fprintf(file, "%i\n", pk1000.counts);
        }
        render_sfml(app);
    }
}



#endif //PKU_SFML_H
