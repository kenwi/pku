//
// Created by kenwi on 24.09.17.
//

#ifndef PKU_SFML_H
#define PKU_SFML_H

#include <SFML/Graphics.h>
#include <SFML/OpenGL.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <SFML/Window.h>
#include "pk1000.h"

extern FILE *file;

void init_sfml(struct application *app) {
    app->width = 800;
    app->height = 500;
    sfVideoMode mode = {app->width, app->height, 32};
    app->renderWindow = sfRenderWindow_create(mode, "pku", sfClose, NULL);

    glClearDepth(1.f);
    glClearColor(0.3f, 0.3f, 0.3f, 0.f);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.f, 1.f, 1.f, 300.0f);
}

bool rotate=true;
float angle;


void render_sfml(struct application *app) {
    sfClock *Clock = sfClock_create();

    //sfRenderWindow_clear(app->renderWindow, sfBlue);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply some transformations for the cube
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -200.f);

    if(rotate){
        angle += 0.001; //sfClock_getElapsedTime(Clock).microseconds/10;
    }
    glRotatef(angle * 50, 1.f, 0.f, 0.f);
    glRotatef(angle * 30, 0.f, 1.f, 0.f);
    glRotatef(angle * 90, 0.f, 0.f, 1.f);

    //Draw a cube
    glBegin(GL_QUADS);//draw some squares
    glColor3i(0,1,1);
    glVertex3f(-50.f, -50.f, -50.f);
    glVertex3f(-50.f,  50.f, -50.f);
    glVertex3f( 50.f,  50.f, -50.f);
    glVertex3f( 50.f, -50.f, -50.f);

    glColor3f(0,0,1);
    glVertex3f(-50.f, -50.f, 50.f);
    glVertex3f(-50.f,  50.f, 50.f);
    glVertex3f( 50.f,  50.f, 50.f);
    glVertex3f( 50.f, -50.f, 50.f);

    glColor3f(1,0,1);
    glVertex3f(-50.f, -50.f, -50.f);
    glVertex3f(-50.f,  50.f, -50.f);
    glVertex3f(-50.f,  50.f,  50.f);
    glVertex3f(-50.f, -50.f,  50.f);

    glColor3f(0,1,0);
    glVertex3f(50.f, -50.f, -50.f);
    glVertex3f(50.f,  50.f, -50.f);
    glVertex3f(50.f,  50.f,  50.f);
    glVertex3f(50.f, -50.f,  50.f);

    glColor3f(1,1,0);
    glVertex3f(-50.f, -50.f,  50.f);
    glVertex3f(-50.f, -50.f, -50.f);
    glVertex3f( 50.f, -50.f, -50.f);
    glVertex3f( 50.f, -50.f,  50.f);

    glColor3f(1,0,0);
    glVertex3f(-50.f, 50.f,  50.f);
    glVertex3f(-50.f, 50.f, -50.f);
    glVertex3f( 50.f, 50.f, -50.f);
    glVertex3f( 50.f, 50.f,  50.f);

    glEnd();
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
