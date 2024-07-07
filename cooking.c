#include <GL/glut.h>
#include <stdio.h>
#include <SOIL/SOIL.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int num = 1;
int num2 = 2;
int num3 = 2;
int num4 = 1;
int lives = 3;
int score = 0;
GLuint textureID;
GLuint texture_up, texture_down, texture_left, texture_right;
GLuint texture_ghost1, texture_ghost2, texture_ghost3, texture_ghost4;
GLuint texture_pellets, texture_power;
GLuint texture_heart;
float x = 30.0f;
float y = 30.0f;
float side = 25.0f;
int speed = 5; // keep it multiple of 5
int delay = 15;

float ghost_x = 300.0f;
float ghost_y = 380.0f;
float ghost_side = 25.0f;
int g_speed = 5;
int g_delay = 20;

int activeboostcount = 0;

int window_x = 700;
int window_y = 800;

sem_t ghost1_coords;
sem_t ghost2_coords;
sem_t ghost3_coords;
sem_t ghost4_coords;
sem_t player_coords;
sem_t power_pellets;
sem_t pellets_coords;
sem_t power_sema;
sem_t house_sema;
sem_t heart_sema;
sem_t counting;
sem_t speed1;
sem_t speed2;
sem_t speed3;
sem_t speed4;

float ghost3_x = 360.0f;
float ghost3_y = 380.0f;
float ghost3_side = 25.0f;
int g_speed3 = 5;
int g_delay3 = 20;

float ghost2_x = 330.0f;
float ghost2_y = 380.0f;
float ghost2_side = 25.0f;
int g_speed2 = 5;
int g_delay2 = 20;

float ghost4_x = 390.0f;
float ghost4_y = 380.0f;
float ghost4_side = 25.0f;
int g_speed4 = 5;
int g_delay4 = 20;

float heart_x = 600.0f;
float heart_y = 445.0f;
float heart_side = 20.0f;

int collision[700][800][4];
int direction = -1;
int prev_direction = -1;
int curr_dir = -1;

bool pellets[700][800] = {0};
bool heart[3] = {1, 1, 1};
float p_side = 5.0f;

int house[4] = {0, 0, 0, 0};
bool complete = 0;
struct power
{
    float power_x;
    float power_y;
    bool taken;
};

struct power power_coords[6];

//(150,280),(530,280),(530,450),(150,450),(30,715),(650,710)

float power_x = 650;
float power_y = 710;
float power_side = 20.0f;
bool power_up;
int power_index;
time_t power_up_time = 0;

int speeds[4];
bool boosts[4] = {0};

void initializePelletArray()
{
    sem_wait(&power_pellets);
    power_coords[0].power_x = 150;
    power_coords[0].power_y = 280;
    power_coords[0].taken = 0;

    power_coords[1].power_x = 530;
    power_coords[1].power_y = 280;
    power_coords[1].taken = 0;

    power_coords[2].power_x = 530;
    power_coords[2].power_y = 450;
    power_coords[2].taken = 0;

    power_coords[3].power_x = 150;
    power_coords[3].power_y = 450;
    power_coords[3].taken = 0;

    power_coords[4].power_x = 30;
    power_coords[4].power_y = 715;
    power_coords[4].taken = 0;

    power_coords[5].power_x = 650;
    power_coords[5].power_y = 710;
    power_coords[5].taken = 0;
    sem_post(&power_pellets);

    sem_wait(&pellets_coords);
    for (int i = 25; i < 350; i += 50)
        pellets[i][35] = 1;
    for (int i = 25; i < 700; i += 50)
        pellets[i][140] = 1;
    for (int i = 400; i < 700; i += 50)
        pellets[i][35] = 1;
    for (int i = 550; i < 700; i += 50)
        pellets[i][215] = 1;
    for (int i = 35; i < 150; i += 50)
        pellets[i][215] = 1;
    for (int i = 250; i < 350; i += 50)
        pellets[i][215] = 1;
    for (int i = 400; i < 500; i += 50)
        pellets[i][215] = 1;
    for (int i = 250; i < 500; i += 50)
        pellets[i][300] = 1;
    for (int i = 250; i < 500; i += 50)
        pellets[i][450] = 1;
    for (int i = 35; i < 300; i += 50)
        pellets[i][525] = 1;
    for (int i = 400; i < 700; i += 50)
        pellets[i][525] = 1;
    for (int i = 150; i < 550; i += 50)
        pellets[i][600] = 1;
    for (int i = 35; i < 135; i += 50)
        pellets[i][600] = 1;
    for (int i = 35; i < 150; i += 50)
        pellets[i][675] = 1;
    for (int i = 550; i < 700; i += 50)
        pellets[i][675] = 1;
    for (int i = 600; i < 700; i += 50)
        pellets[i][600] = 1;
    for (int i = 250; i < 350; i += 50)
        pellets[i][675] = 1;
    for (int i = 400; i < 500; i += 50)
        pellets[i][675] = 1;
    for (int i = 35; i < 600; i += 50)
        pellets[i][765] = 1;
    for (int i = 35; i < 250; i += 50)
        pellets[i][375] = 1;
    for (int i = 500; i < 700; i += 50)
        pellets[i][375] = 1;
    sem_post(&pellets_coords);
}

void initializeCollisionArray()
{
    for (int i = 0; i < 700; i++)
    {
        for (int j = 0; j < 800; j++)
        {
            collision[i][j][0] = 1; // right
            collision[i][j][1] = 1; // left
            collision[i][j][2] = 1; // up
            collision[i][j][3] = 1; // down

            if (i == 30 && j > 30 && j < 210)
            {
                collision[i][30][1] = 0;
                collision[i][30][2] = 0;
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            collision[30][130][0] = 1;
            collision[30][210][1] = 0;
            collision[30][210][3] = 0;

            if (i > 30 && i < 150 && j == 210)
            {
                collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            if (i == 150 && j < 670 && j > 30)
            {
                if (j != 670 && j != 520 && j != 370 && j != 210 && j != 130)
                {
                    collision[i][j][0] = 0;
                    collision[i][j][1] = 0;
                }
            }
            collision[150][30][2] = 0;
            collision[150][670][0] = 0;
            collision[150][670][3] = 0;
            collision[150][210][0] = 0;

            if (j == 30 && i > 30 && i < 300)
            {
                collision[i][j][2] = 0;
                if (i != 150)
                    collision[i][j][3] = 0;
            }

            collision[310][30][2] = 0;
            collision[310][30][0] = 0;

            if (i == 310 && j > 30 && j < 130)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            if (j == 130 && i > 30 && i < 660)
            {
                if (i != 150 && i != 520)
                {
                    if (i != 310 && i != 380)
                        collision[i][j][2] = 0;
                    if (i != 230 && i != 460)
                        collision[i][j][3] = 0;
                }
            }

            collision[660][130][0] = 0;

            if (i == 380 && j > 30 && j < 130)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }
            collision[380][30][2] = 0;
            collision[380][30][1] = 0;

            if (j == 30 && i > 380 && i < 660)
            {
                collision[i][j][2] = 0;
                if (i != 530)
                    collision[i][j][3] = 0;
            }

            collision[660][30][0] = 0;
            collision[660][30][2] = 0;
            collision[530][130][3] = 1;
            collision[530][130][2] = 1;

            if (i == 530 && j > 30 && j < 670)
            {
                if (j != 130 && j != 370 && j != 520)
                {
                    if (j != 210)
                        collision[i][j][0] = 0;
                    if (j != 600)
                        collision[i][j][1] = 0;
                }
            }

            collision[530][670][1] = 0;
            collision[530][670][3] = 0;

            if (i == 660 && j > 30 && j <= 210)
            {
                collision[i][j][0] = 0;
                if (j != 130)
                    collision[i][j][1] = 0;
            }

            collision[660][210][3] = 0;
            collision[660][210][1] = 1;

            if (i == 230 && j > 130 && j < 210)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            collision[230][210][3] = 0;
            collision[230][210][1] = 0;

            if (j == 210 && i > 230 && i < 310)
            {
                collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            collision[230][210][2] = 0;
            collision[230][210][0] = 0;

            if (i == 310 && j > 210 && j < 290)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }
            collision[310][290][3] = 0;

            if (j == 290 && i > 230 && i < 460)
            {
                if (i != 310 && i != 380)
                    collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            collision[230][210][2] = 1;
            collision[230][210][0] = 1;
            collision[310][210][2] = 0;
            collision[310][210][0] = 0;

            collision[230][290][1] = 0;
            collision[230][290][2] = 0;
            collision[460][290][0] = 0;
            collision[460][290][2] = 0;

            if (i == 460 && j > 130 && j < 210)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            collision[460][210][3] = 0;
            collision[460][210][0] = 0;
            collision[460][210][1] = 1;

            if (j == 210 && i < 460 && i > 380)
            {
                collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            collision[380][210][1] = 0;
            collision[380][210][2] = 0;

            if (i == 380 && j > 210 && j < 290)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            collision[520][130][2] = 0;
            collision[520][130][3] = 0;

            if (i == 230 && j > 290 && j < 520)
            {
                if (j != 440)
                    collision[i][j][0] = 0;
                if (j != 370)
                    collision[i][j][1] = 0;
            }
            if (i == 460 && j > 290 && j < 520)
            {
                if (j != 370)
                    collision[i][j][0] = 0;
                if (j != 440)
                    collision[i][j][1] = 0;
            }

            collision[230][520][3] = 0;
            collision[460][520][3] = 0;

            if (j == 370 && i < 230 && i > 20)
            {
                if (i != 150)
                    collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            if (j == 370 && i > 460 && i < 670)
            {
                if (i != 530)
                {
                    collision[i][j][2] = 0;
                    collision[i][j][3] = 0;
                }
            }

            if (j == 440 && i > 230 && i < 460)
            {
                collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            collision[150][370][3] = 1;

            if (j == 520 && i > 30 && i < 310)
            {
                if (i != 150)
                {
                    if (i != 230)
                        collision[i][j][2] = 0;
                    collision[i][j][3] = 0;
                }
            }

            collision[30][520][1] = 0;
            collision[30][520][2] = 0;

            collision[310][520][0] = 0;
            collision[310][520][2] = 0;

            if (j == 520 && i > 380 && i < 660)
            {
                if (i != 530)
                {
                    if (i != 460)
                        collision[i][j][2] = 0;
                    collision[i][j][3] = 0;
                }
            }

            collision[380][520][1] = 0;
            collision[380][520][2] = 0;

            collision[660][520][0] = 0;
            collision[660][520][2] = 0;

            if (i == 30 && j > 520 && j < 600)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            collision[30][600][1] = 0;
            collision[30][600][3] = 0;

            if (i == 310 && j > 520 && j < 600)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            collision[310][600][3] = 0;
            collision[150][600][0] = 1;

            if (j == 600 && i > 150 && i < 530)
            {
                if (i != 310 && i != 380)
                    collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            if (i == 660 && j > 520 && j < 600)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            collision[660][600][0] = 0;
            collision[660][600][3] = 0;

            if (i == 80 && j > 600 && j < 670)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            collision[80][600][0] = 0;
            collision[80][600][2] = 0;

            collision[80][670][3] = 0;

            if (j == 670 && i < 150 && i > 30)
            {
                collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            collision[80][670][2] = 1;
            collision[30][670][1] = 0;
            collision[30][670][2] = 0;

            if (i == 380 && j > 520 && j < 600)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            if (j == 600 && i < 660 && i > 600)
            {
                collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            collision[600][600][1] = 0;
            collision[600][600][2] = 0;

            if (i == 600 && j > 600 && j < 670)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            collision[600][670][3] = 0;

            if (j == 670 && i > 530 && i < 660)
            {
                if (i != 600)
                    collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            collision[660][670][0] = 0;
            collision[660][670][2] = 0;

            collision[30][750][1] = 0;
            collision[30][750][3] = 0;

            if (j == 750 && i > 30 && i < 660)
            {
                collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            if (i == 30 && j > 670 && j < 750)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            if (i == 660 && j > 670 && j < 750)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }
            collision[660][750][0] = 0;
            collision[660][750][3] = 0;

            collision[230][600][3] = 1;
            collision[460][600][3] = 1;

            if (i == 230 && j > 600 && j < 680)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            collision[230][680][1] = 0;
            collision[230][680][3] = 0;
            if (i == 460 && j > 600 && j < 680)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }
            collision[460][680][0] = 0;
            collision[460][680][3] = 0;

            if (j == 680 && i > 230 && i < 310)
            {
                collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            collision[310][680][0] = 0;
            collision[310][680][2] = 0;

            if (j == 680 && i > 380 && i < 460)
            {
                collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            collision[460][680][0] = 0;
            collision[460][680][2] = 0;

            collision[380][680][1] = 0;
            collision[380][680][2] = 0;

            if (i == 380 && j > 680 && j < 750)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            if (i == 310 && j > 680 && j < 750)
            {
                collision[i][j][0] = 0;
                collision[i][j][1] = 0;
            }

            collision[310][750][2] = 1;
            collision[380][750][2] = 1;

            collision[460][680][2] = 1;

            if (j == 210 && i < 660 && i > 530)
            {
                collision[i][j][2] = 0;
                collision[i][j][3] = 0;
            }

            collision[20][370][2] = 0;
            collision[20][370][3] = 0;

            collision[670][370][2] = 0;
            collision[670][370][3] = 0;

            for (int k = 0; k < 10; k++)
            {
                collision[300 + k][30][2] = 0;
            }
            collision[310][30][0] = 0;
            collision[310][30][1] = 1;
            collision[310][30][2] = 0;
            collision[310][30][3] = 1;
        }
    }
}

void displayBackground()
{
    // Draw the background image
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void displaylives()
{
    sem_wait(&heart_sema);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_heart);

    for (int i = 0; i < 3; i++)
    {
        if (heart[i])
        {
            float pos = 600;
            pos += i * 20;
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex2f((pos / window_x) * 2 - 1, 1 - (heart_y / window_y) * 2);
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(((pos + heart_side) / window_x) * 2 - 1, 1 - (heart_y / window_y) * 2);
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(((pos + heart_side) / window_x) * 2 - 1, 1 - ((heart_y + heart_side) / window_y) * 2);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f((pos / window_x) * 2 - 1, 1 - ((heart_y + heart_side) / window_y) * 2);
            glEnd();
        }
    }

    glDisable(GL_TEXTURE_2D);

    sem_post(&heart_sema);
}

void displayrun(float xx, float yy, float ss)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_ghost4);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f((xx / window_x) * 2 - 1, 1 - (yy / window_y) * 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(((xx + ss) / window_x) * 2 - 1, 1 - (yy / window_y) * 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(((xx + ss) / window_x) * 2 - 1, 1 - ((yy + ss) / window_y) * 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f((xx / window_x) * 2 - 1, 1 - ((yy + ss) / window_y) * 2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void displayPlayer()
{

    sem_wait(&player_coords);
    glEnable(GL_TEXTURE_2D);

    switch (curr_dir)
    {
    case 0: // Right
        glBindTexture(GL_TEXTURE_2D, texture_right);
        break;
    case 1: // Left
        glBindTexture(GL_TEXTURE_2D, texture_left);
        break;
    case 2: // Up
        glBindTexture(GL_TEXTURE_2D, texture_up);
        break;
    case 3: // Down
        glBindTexture(GL_TEXTURE_2D, texture_down);
        break;
    }

    // Draw the moving square
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f((x / window_x) * 2 - 1, 1 - (y / window_y) * 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(((x + side) / window_x) * 2 - 1, 1 - (y / window_y) * 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(((x + side) / window_x) * 2 - 1, 1 - ((y + side) / window_y) * 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f((x / window_x) * 2 - 1, 1 - ((y + side) / window_y) * 2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    sem_post(&player_coords);
}

void displaypellets()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_pellets);
    complete = 0;
    sem_wait(&pellets_coords);
    for (int i = 0; i < 700; i++)
    {
        for (int j = 0; j < 800; j++)
        {
            if (pellets[i][j])
            {
                complete = 1;
                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 1.0f);
                glVertex2f(((float)i / window_x) * 2 - 1, 1 - ((float)j / window_y) * 2);
                glTexCoord2f(1.0f, 1.0f);
                glVertex2f((((float)i + p_side) / window_x) * 2 - 1, 1 - ((float)j / window_y) * 2);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2f((((float)i + p_side) / window_x) * 2 - 1, 1 - (((float)j + p_side) / window_y) * 2);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2f(((float)i / window_x) * 2 - 1, 1 - (((float)j + p_side) / window_y) * 2);
                glEnd();
            }
        }
    }
    if (complete == 0)
    {
        for (int i = 25; i < 350; i += 50)
            pellets[i][35] = 1;
        for (int i = 25; i < 700; i += 50)
            pellets[i][140] = 1;
        for (int i = 400; i < 700; i += 50)
            pellets[i][35] = 1;
        for (int i = 550; i < 700; i += 50)
            pellets[i][215] = 1;
        for (int i = 35; i < 150; i += 50)
            pellets[i][215] = 1;
        for (int i = 250; i < 350; i += 50)
            pellets[i][215] = 1;
        for (int i = 400; i < 500; i += 50)
            pellets[i][215] = 1;
        for (int i = 250; i < 500; i += 50)
            pellets[i][300] = 1;
        for (int i = 250; i < 500; i += 50)
            pellets[i][450] = 1;
        for (int i = 35; i < 300; i += 50)
            pellets[i][525] = 1;
        for (int i = 400; i < 700; i += 50)
            pellets[i][525] = 1;
        for (int i = 150; i < 550; i += 50)
            pellets[i][600] = 1;
        for (int i = 35; i < 135; i += 50)
            pellets[i][600] = 1;
        for (int i = 35; i < 150; i += 50)
            pellets[i][675] = 1;
        for (int i = 550; i < 700; i += 50)
            pellets[i][675] = 1;
        for (int i = 600; i < 700; i += 50)
            pellets[i][600] = 1;
        for (int i = 250; i < 350; i += 50)
            pellets[i][675] = 1;
        for (int i = 400; i < 500; i += 50)
            pellets[i][675] = 1;
        for (int i = 35; i < 600; i += 50)
            pellets[i][765] = 1;
        for (int i = 35; i < 250; i += 50)
            pellets[i][375] = 1;
        for (int i = 500; i < 700; i += 50)
            pellets[i][375] = 1;
    }
    sem_post(&pellets_coords);
    glDisable(GL_TEXTURE_2D);
    // printf("\nending display...\n");
}

void displayghost3()
{
    sem_wait(&ghost3_coords);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_ghost3);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f((ghost3_x / window_x) * 2 - 1, 1 - (ghost3_y / window_y) * 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(((ghost3_x + ghost3_side) / window_x) * 2 - 1, 1 - (ghost3_y / window_y) * 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(((ghost3_x + ghost3_side) / window_x) * 2 - 1, 1 - ((ghost3_y + ghost3_side) / window_y) * 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f((ghost3_x / window_x) * 2 - 1, 1 - ((ghost3_y + ghost3_side) / window_y) * 2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    sem_post(&ghost3_coords);
}

void displayghost4()
{
    sem_wait(&ghost4_coords);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_ghost4);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f((ghost4_x / window_x) * 2 - 1, 1 - (ghost4_y / window_y) * 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(((ghost4_x + ghost4_side) / window_x) * 2 - 1, 1 - (ghost4_y / window_y) * 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(((ghost4_x + ghost4_side) / window_x) * 2 - 1, 1 - ((ghost4_y + ghost4_side) / window_y) * 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f((ghost4_x / window_x) * 2 - 1, 1 - ((ghost4_y + ghost4_side) / window_y) * 2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    sem_post(&ghost4_coords);
}

void displayghost2()
{
    sem_wait(&ghost2_coords);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_ghost2);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f((ghost2_x / window_x) * 2 - 1, 1 - (ghost2_y / window_y) * 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(((ghost2_x + ghost2_side) / window_x) * 2 - 1, 1 - (ghost2_y / window_y) * 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(((ghost2_x + ghost2_side) / window_x) * 2 - 1, 1 - ((ghost2_y + ghost2_side) / window_y) * 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f((ghost2_x / window_x) * 2 - 1, 1 - ((ghost2_y + ghost2_side) / window_y) * 2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    sem_post(&ghost2_coords);
}

void displayghost()
{
    sem_wait(&ghost1_coords);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_ghost1);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f((ghost_x / window_x) * 2 - 1, 1 - (ghost_y / window_y) * 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(((ghost_x + ghost_side) / window_x) * 2 - 1, 1 - (ghost_y / window_y) * 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(((ghost_x + ghost_side) / window_x) * 2 - 1, 1 - ((ghost_y + ghost_side) / window_y) * 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f((ghost_x / window_x) * 2 - 1, 1 - ((ghost_y + ghost_side) / window_y) * 2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    sem_post(&ghost1_coords);
}

void displaypower()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_power);

    sem_wait(&power_pellets);
    for (int i = 0; i < 6; i++)
    {
        if (!power_coords[i].taken)
        {
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex2f((power_coords[i].power_x / window_x) * 2 - 1, 1 - (power_coords[i].power_y / window_y) * 2);
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(((power_coords[i].power_x + power_side) / window_x) * 2 - 1, 1 - (power_coords[i].power_y / window_y) * 2);
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(((power_coords[i].power_x + power_side) / window_x) * 2 - 1, 1 - ((power_coords[i].power_y + power_side) / window_y) * 2);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f((power_coords[i].power_x / window_x) * 2 - 1, 1 - ((power_coords[i].power_y + power_side) / window_y) * 2);
            glEnd();
        }
    }
    sem_post(&power_pellets);
    glDisable(GL_TEXTURE_2D);
}

void displayscore()
{
    glColor3f(1.0, 1.0, 1.0);

    glRasterPos2f(-0.95, 0.22);
    char scr[20];
    sprintf(scr, "Score: %d", score);
    for (int i = 0; scr[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scr[i]);
    }
    glFlush();
}

void displayp()
{
    glColor3f(1.0, 1.0, 1.0);

    glRasterPos2f(-0.98, -0.16);
    const char *text = "Power up!";
    for (int i = 0; text[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
    }
    glFlush();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    displayBackground();
    displaypellets();
    displaypower();
    displayPlayer();
    if (!power_up)
    {
        displayghost();
        displayghost2();
        displayghost3();
        displayghost4();
    }
    displaylives();
    displayscore();
    if (power_up)
    {
        displayp();
        sem_wait(&ghost1_coords);
        displayrun(ghost_x, ghost_y, ghost_side);
        sem_post(&ghost1_coords);

        sem_wait(&ghost2_coords);
        displayrun(ghost2_x, ghost2_y, ghost2_side);
        sem_post(&ghost2_coords);

        sem_wait(&ghost3_coords);
        displayrun(ghost3_x, ghost3_y, ghost3_side);
        sem_post(&ghost3_coords);

        sem_wait(&ghost4_coords);
        displayrun(ghost4_x, ghost4_y, ghost4_side);
        sem_post(&ghost4_coords);
    }
    glutSwapBuffers();
}

void keyboard(int key, int xx, int yy)
{
    switch (key)
    {
    case GLUT_KEY_RIGHT:
        prev_direction = direction;
        direction = 0;
        break;
    case GLUT_KEY_LEFT:
        prev_direction = direction;
        direction = 1;
        break;
    case GLUT_KEY_UP:
        prev_direction = direction;
        direction = 2;
        break;
    case GLUT_KEY_DOWN:
        prev_direction = direction;
        direction = 3;
        break;
    }
}

void movement()
{
    sem_wait(&player_coords);
    switch (prev_direction)
    {
    case 0: // Right
        if (direction != 1 && x < 670 && collision[(int)(x)][(int)y][0])
        {
            x += speed;
            curr_dir = prev_direction;
        }
        break;
    case 1: // Left
        if (direction != 0 && x > 20 && collision[(int)(x)][(int)y][1])
        {
            x -= speed;
            curr_dir = prev_direction;
        }
        break;
    case 2: // Up
        if (direction != 3 && y > 20 && collision[(int)x][(int)(y)][2])
        {
            y -= speed;
            curr_dir = prev_direction;
        }
        break;
    case 3: // Down
        if (direction != 2 && y < 760 && collision[(int)x][(int)(y)][3])
        {
            curr_dir = prev_direction;
            y += speed;
        }
        break;
    }
    sem_post(&player_coords);
    //  printf("%d\t%d\n",x,y);
}

void timer(int value) // player
{
    sem_wait(&player_coords);
    switch (direction)
    {
    case 0: // Right
        if (x < 670 && collision[(int)(x)][(int)y][0])
        {
            x += speed;
            curr_dir = direction;
        }
        else
        {
            sem_post(&player_coords);
            movement();
        }
        break;
    case 1: // Left
        if (x > 20 && collision[(int)(x)][(int)y][1])
        {
            x -= speed;
            curr_dir = direction;
        }
        else
        {
            sem_post(&player_coords);
            movement();
        }
        break;
    case 2: // Up
        if (y > 20 && collision[(int)x][(int)(y)][2])
        {
            y -= speed;
            curr_dir = direction;
        }
        else
        {
            sem_post(&player_coords);
            movement();
        }
        break;
    case 3: // Down
        if (y < 760 && collision[(int)x][(int)(y)][3])
        {
            y += speed;
            curr_dir = direction;
        }
        else
        {
            sem_post(&player_coords);
            movement();
        }
        break;
    }
    if ((int)x == 670 && (int)y == 370)
    {
        x = 30;
    }

    if ((int)x == 20 && (int)y == 370)
    {
        x = 660;
    }
    sem_post(&player_coords);
    // printf("%f %f \n",x,y);
    glutPostRedisplay();
    glutTimerFunc(delay, timer, 0); // Call the timer function again after 50 milliseconds
}


void dumbghost()
{
    sem_wait(&ghost1_coords);
    sem_wait(&house_sema);
    int h = house[0];
    if (house[0] == 0)
    {
        if (house[3] != 2 && house[1] != 2) // both are not leaving (not eating)
            house[0] = 2;                   // now in the process of leaving as both resources are available
        // else
        // sleep(0.5);						//resources not available
    }
    else if (house[0] == 2) // if leaving (eating)
    {
        ghost_y -= g_speed;
        if (ghost_y == 290) // check if finished leaving (eating)
        {
            house[0] = 1; // left (finished eating now "thinking")
            h = 1;
        }
    }

    sem_post(&house_sema);
    if (h == 1)
    {
        if (ghost_x == 290 && ghost_y == 30) // some weird bug temp fix
        {
            num = 2;
        }

        if (ghost_x == 310 && ghost_y == 50)
        {
            num = 4;
        }

        if (ghost_x == 30 && ghost_y == 580)
        {
            num = 3;
        }

        if (ghost_x == 80 && ghost_y == 620)
        {
            num = 4;
        }

        if (num == 1 || num == 2)
        {
            int temp = 1 + rand() % 2;
            if (temp == 2) // decides whether to keep current direction
            {

                if (collision[(int)(ghost_x)][(int)ghost_y][2])
                {
                    if (collision[(int)(ghost_x)][(int)ghost_y][3])
                        num = 1 + rand() % 2 + 2;
                    else
                        num = 3;
                }

                else if (collision[(int)(ghost_x)][(int)ghost_y][3])
                {
                    num = 4;
                }
            }
        }

        else if (num == 3 || num == 4)
        {
            int temp = 1 + rand() % 2;
            if (temp == 2)
            {
                if (collision[(int)(ghost_x)][(int)ghost_y][0])
                {
                    if (collision[(int)(ghost_x)][(int)ghost_y][1])
                        num = 1 + rand() % 2;

                    else
                        num = 1;
                }

                else if (collision[(int)(ghost_x)][(int)ghost_y][1])
                {
                    num = 2;
                }
            }
        }
        switch (num)
        {
        case 1: // Right

            if (ghost_x < 670 && collision[(int)(ghost_x)][(int)ghost_y][0])
                ghost_x += g_speed;

            else
                num = 1 + rand() % 4;

            break;
        case 2: // Left

            if (ghost_x > 20 && collision[(int)(ghost_x)][(int)ghost_y][1])
                ghost_x -= g_speed;
            else
                num = 1 + rand() % 4;

            break;
        case 3: // Up

            if (ghost_y < 760 && collision[(int)ghost_x][(int)ghost_y][2])
                ghost_y -= g_speed;
            else
                num = 1 + rand() % 4; // Change direction

            break;
        case 4: // Down
            if (ghost_y > 20 && collision[(int)ghost_x][(int)ghost_y][3])
                ghost_y += g_speed;
            else
                num = 1 + rand() % 4; // Change direction

            break;
        }
        
    }
    // printf("%f,%f \n",ghost_x,ghost_y);

    sem_post(&ghost1_coords);

    glutPostRedisplay();

    sem_wait(&speed1);
    glutTimerFunc(g_delay, dumbghost, 0);
    sem_post(&speed1);
}

void smartghost()
{
    float px, py;
    sem_wait(&player_coords);
    px = x;
    py = y;
    sem_post(&player_coords);
    sem_wait(&ghost4_coords);
    sem_wait(&house_sema);
    int h = house[3];
    if (house[3] == 0)
    {
        if (house[0] != 2 && house[2] != 2) // both are outside (not hungry)
            house[3] = 2;                   // now in the process of leaving as both resources are available
        // else
        // sleep(1);						//resources not available
    }
    else if (house[3] == 2) // if leaving (eating)
    {
        ghost4_y -= g_speed4;
        if (ghost4_y == 290) // check if finished leaving (eating)
        {
            house[3] = 1; // left (finished eating now "thinking")
            h = 1;
        }
    }
    sem_post(&house_sema);
    if (h == 1)
    {
        if (power_up == 0)
        {
            if (px == ghost4_x || py == ghost4_y)
            {
                if (px == ghost4_x)
                {
                    // printf("hello 1\n");
                    if (py < ghost4_y)
                    {
                        num4 = 3;
                    }
                    else if (py > ghost4_y)
                    {
                        num4 = 4;
                    }
                }
                else if (py == ghost4_y)
                {
                    // printf("hello 2\n");
                    if (px < ghost4_x)
                    {
                        num4 = 2;
                    }
                    else if (px > ghost4_x)
                    {
                        num4 = 1;
                    }
                }
            }

            else
            {
                if (px > ghost4_x && py < ghost4_y && px - ghost4_x > 40 && ghost4_y - py > 40)
                {
                    if (collision[(int)(ghost4_x)][(int)ghost4_y][2])
                    {
                        num4 = 3;
                    }
                    else if (collision[(int)(ghost4_x)][(int)ghost4_y][0])
                        num4 = 1;

                    else
                    {
                        while (!collision[(int)(ghost4_x)][(int)ghost4_y][2])
                        {
                            ghost4_x -= g_speed4;
                            usleep(1000);
                            glutPostRedisplay();
                        }
                        num4 = 3;
                    }
                    // printf("1 quad, num : %d \n \n ",num2);
                }

                else if (px < ghost4_x && py < ghost4_y && ghost4_x - px > 40 && ghost4_y - py > 40)
                {
                    if (collision[(int)(ghost4_x)][(int)ghost4_y][2])
                    {
                        num4 = 3;
                    }
                    else if ((collision[(int)(ghost4_x)][(int)ghost4_y][1]))
                        num4 = 2;
                    else
                    {
                        while (!collision[(int)(ghost4_x)][(int)ghost4_y][2])
                        {
                            ghost4_x += g_speed4;
                            usleep(1000);
                            glutPostRedisplay();
                        }
                        num4 = 3;
                    }

                    // printf("2 quad, num : %d \n \n",num2);
                }

                else if (px < ghost4_x && py > ghost4_y && ghost4_x - px > 40 && py - ghost4_x > 40)
                {
                    if (collision[(int)(ghost4_x)][(int)ghost4_y][3])
                    {
                        num4 = 4;
                    }
                    else if ((collision[(int)(ghost4_x)][(int)ghost4_y][1]))
                        num4 = 2;

                    else
                    {
                        while (!collision[(int)(ghost4_x)][(int)ghost4_y][3])
                        {
                            ghost4_x += g_speed4;
                            usleep(1000);
                            glutPostRedisplay();
                        }
                        num4 = 4;
                    }
                    // printf("3 quad, num : %d \n \n",num2);
                }

                else if (px > ghost4_x && py > ghost4_y && px - ghost4_x > 40 && py - ghost4_y > 40)
                {
                    if (collision[(int)(ghost4_x)][(int)ghost4_y][3])
                    {
                        num4 = 4;
                    }
                    else if ((collision[(int)(ghost4_x)][(int)ghost4_y][0]))
                        num4 = 1;

                    else
                    {
                        while (!collision[(int)(ghost4_x)][(int)ghost4_y][3])
                        {
                            ghost4_x -= g_speed4;
                            usleep(1000);
                            glutPostRedisplay();
                        }
                        num4 = 4;
                    }

                    // printf("4 quad, num : %d \n \n",num2);
                }
            }
        }
        else if (power_up)
        {
            if (px == ghost4_x || py == ghost4_y)
            {
                if (px == ghost4_x)
                {
                    // printf("hello 1\n");
                    if (py < ghost4_y)
                    {
                        num4 = 4;
                    }
                    else if (py > ghost4_y)
                    {
                        num4 = 3;
                    }
                }
                else if (py == ghost4_y)
                {
                    // printf("hello 2\n");
                    if (px < ghost4_x)
                    {
                        num4 = 1;
                    }
                    else if (px > ghost4_x)
                    {
                        num4 = 2;
                    }
                }
            }

            else
            {
                if (px > ghost4_x && py < ghost4_y && px - ghost4_x > 40 && ghost4_y - py > 40)
                {
                    if (collision[(int)(ghost4_x)][(int)ghost4_y][3])
                    {
                        num4 = 4;
                    }
                    else if (collision[(int)(ghost4_x)][(int)ghost4_y][1])
                        num4 = 2;

                    // printf("1 quad, num : %d \n \n ",num2);
                }

                else if (px < ghost4_x && py < ghost4_y && ghost4_x - px > 40 && ghost4_y - py > 40)
                {
                    if (collision[(int)(ghost4_x)][(int)ghost4_y][3])
                    {
                        num4 = 4;
                    }
                    else if ((collision[(int)(ghost4_x)][(int)ghost4_y][2]))
                        num4 = 1;

                    // printf("2 quad, num : %d \n \n",num2);
                }

                else if (px < ghost4_x && py > ghost4_y && ghost4_x - px > 40 && py - ghost4_x > 40)
                {
                    if (collision[(int)(ghost4_x)][(int)ghost4_y][2])
                    {
                        num4 = 3;
                    }
                    else if ((collision[(int)(ghost4_x)][(int)ghost4_y][0]))
                        num4 = 1;

                    // printf("3 quad, num : %d \n \n",num2);
                }

                else if (px > ghost4_x && py > ghost4_y && px - ghost4_x > 40 && py - ghost4_y > 40)
                {
                    if (collision[(int)(ghost4_x)][(int)ghost4_y][2])
                    {
                        num4 = 3;
                    }
                    else if ((collision[(int)(ghost4_x)][(int)ghost4_y][1]))
                        num4 = 2;

                    // printf("4 quad, num : %d \n \n",num2);
                }
            }
        }
        switch (num4)
        {
        case 1: // Right

            if (ghost4_x < 670 && collision[(int)(ghost4_x)][(int)ghost4_y][0])
                ghost4_x += g_speed4;

            break;
        case 2: // Left

            if (ghost4_x > 20 && collision[(int)(ghost4_x)][(int)ghost4_y][1])
                ghost4_x -= g_speed4;

            break;
        case 3: // Up

            if (ghost4_y < 760 && collision[(int)ghost4_x][(int)ghost4_y][2])
                ghost4_y -= g_speed4;

            break;
        case 4: // Down
            if (ghost4_y > 20 && collision[(int)ghost4_x][(int)ghost4_y][3])
                ghost4_y += g_speed4;

            break;
        }
        
    }
    sem_post(&ghost4_coords);

    glutPostRedisplay();

    sem_wait(&speed4);
    glutTimerFunc(g_delay4, smartghost, 0);
    sem_post(&speed4);
}

void dumbghost2()
{
    sem_wait(&ghost3_coords);
    sem_wait(&house_sema);
    int h = house[2];
    if (house[2] == 0)
    {
        if (house[3] != 2 && house[1] != 2) // both are outside (not hungry)
            house[2] = 2;                   // now in the process of leaving as both resources are available
        // else
        //	sleep(1);						//resources not available
    }
    else if (house[2] == 2) // if leaving (eating)
    {
        ghost3_y -= g_speed3;
        if (ghost3_y == 290) // check if finished leaving (eating)
        {
            house[2] = 1; // left (finished eating now "thinking")
            h = 1;
        }
    }
    sem_post(&house_sema);
    if (h == 1)
    {
        if (ghost3_x == 290 && ghost3_y == 30)
        {
            num3 = 2;
        }

        if (ghost3_x == 310 && ghost3_y == 50)
        {
            num3 = 4;
        }

        if (ghost3_x == 30 && ghost3_y == 580)
        {
            num3 = 3;
        }

        if (ghost3_x == 80 && ghost3_y == 620)
        {
            num3 = 4;
        }

        if (num3 == 1 || num3 == 2)
        {
            int temp = 1 + rand() % 2;
            if (temp == 2)
            {
                if (collision[(int)(ghost3_x)][(int)ghost3_y][2])
                {
                    if (collision[(int)(ghost3_x)][(int)ghost3_y][3])
                        num3 = 1 + rand() % 2 + 2;
                    else
                        num3 = 3;
                }

                else if (collision[(int)(ghost3_x)][(int)ghost3_y][3])
                {
                    num3 = 4;
                }
            }
            // printf("num : %d \n",num);
        }

        else if (num3 == 3 || num3 == 4)
        {
            int temp = 1 + rand() % 2;
            if (temp == 2)
            {
                if (collision[(int)(ghost3_x)][(int)ghost3_y][0])
                {
                    if (collision[(int)(ghost3_x)][(int)ghost3_y][1])
                        num3 = 1 + rand() % 2;

                    else
                        num3 = 1;
                }

                else if (collision[(int)(ghost3_x)][(int)ghost3_y][1])
                {
                    num3 = 2;
                }
            }
            // printf("num : %d \n",num);
        }

        switch (num3)
        {
        case 1: // Right

            if (ghost3_x < 670 && collision[(int)(ghost3_x)][(int)ghost3_y][0])
                ghost3_x += g_speed3;

            else
                num3 = 1 + rand() % 4;

            break;
        case 2: // Left

            if (ghost3_x > 20 && collision[(int)(ghost3_x)][(int)ghost3_y][1])
                ghost3_x -= g_speed3;
            else
                num3 = 1 + rand() % 4;

            break;
        case 3: // Up

            if (ghost3_y < 760 && collision[(int)ghost3_x][(int)ghost3_y][2])
                ghost3_y -= g_speed3;
            else
                num3 = 1 + rand() % 4; // Change direction

            break;
        case 4: // Down
            if (ghost3_y > 20 && collision[(int)ghost3_x][(int)ghost3_y][3])
                ghost3_y += g_speed3;
            else
                num3 = 1 + rand() % 4; // Change direction
            break;
        }
    }
    sem_post(&ghost3_coords);

    glutPostRedisplay();

    sem_wait(&speed3);
    glutTimerFunc(g_delay3, dumbghost2, 0);
    sem_post(&speed3);
    
}

void smartghost2()
{

    float px, py;
    sem_wait(&player_coords);
    px = x;
    py = y;
    sem_post(&player_coords);

    sem_wait(&ghost2_coords);
    // printf("px : %f, py: %f, gx: %f, gy: %f\n", px,py,ghost2_x,ghost2_y);
    sem_wait(&house_sema);
    int h = house[1];
    if (house[1] == 0)
    {
        if (house[0] != 2 && house[2] != 2) // both are outside (not hungry)
            house[1] = 2;                   // now in the process of leaving as both resources are available
        //    else
        //  	sleep(1);						//resources not available
    }
    else if (house[1] == 2) // if leaving (eating)
    {
        ghost2_y -= g_speed2;
        if (ghost2_y == 290) // check if finished leaving (eating)
        {
            house[1] = 1; // left (finished eating now "thinking")
            h = 1;
        }
    }

    sem_post(&house_sema);
    if (h == 1)
    {
        if (power_up == 0)
        {
            if (px == ghost2_x || py == ghost2_y)
            {
                if (px == ghost2_x)
                {
                    // printf("hello 1\n");
                    if (py < ghost2_y)
                    {
                        num2 = 3;
                    }
                    else if (py > ghost2_y)
                    {
                        num2 = 4;
                    }
                }
                else if (py == ghost2_y)
                {
                    // printf("hello 2\n");
                    if (px < ghost2_x)
                    {
                        num2 = 2;
                    }
                    else if (px > ghost2_x)
                    {
                        num2 = 1;
                    }
                }
            }

            else
            {
                if (px > ghost2_x && py < ghost2_y && px - ghost2_x > 40 && ghost2_y - py > 40)
                {
                    if (collision[(int)(ghost2_x)][(int)ghost2_y][2])
                    {
                        num2 = 3;
                    }
                    else if (collision[(int)(ghost2_x)][(int)ghost2_y][0])
                        num2 = 1;

                    else
                    {
                        while (!collision[(int)(ghost2_x)][(int)ghost2_y][2])
                        {
                            ghost2_x -= g_speed2;
                            usleep(1000);
                            glutPostRedisplay();
                        }
                        num2 = 3;
                    }
                    // printf("1 quad, num : %d \n \n ",num2);
                }

                else if (px < ghost2_x && py < ghost2_y && ghost2_x - px > 40 && ghost2_y - py > 40)
                {
                    if (collision[(int)(ghost2_x)][(int)ghost2_y][2])
                    {
                        num2 = 3;
                    }
                    else if ((collision[(int)(ghost2_x)][(int)ghost2_y][1]))
                        num2 = 2;
                    else
                    {
                        while (!collision[(int)(ghost2_x)][(int)ghost2_y][2])
                        {
                            ghost2_x += g_speed2;
                            usleep(1000);
                            glutPostRedisplay();
                        }
                        num2 = 3;
                    }

                    // printf("2 quad, num : %d \n \n",num2);
                }

                else if (px < ghost2_x && py > ghost2_y && ghost2_x - px > 40 && py - ghost2_x > 40)
                {
                    if (collision[(int)(ghost2_x)][(int)ghost2_y][3])
                    {
                        num2 = 4;
                    }
                    else if ((collision[(int)(ghost2_x)][(int)ghost2_y][1]))
                        num2 = 2;

                    else
                    {
                        while (!collision[(int)(ghost2_x)][(int)ghost2_y][3])
                        {
                            ghost2_x += g_speed2;
                            usleep(1000);
                            glutPostRedisplay();
                        }
                        num2 = 4;
                    }
                    // printf("3 quad, num : %d \n \n",num2);
                }

                else if (px > ghost2_x && py > ghost2_y && px - ghost2_x > 40 && py - ghost2_y > 40)
                {
                    if (collision[(int)(ghost2_x)][(int)ghost2_y][3])
                    {
                        num2 = 4;
                    }
                    else if ((collision[(int)(ghost2_x)][(int)ghost2_y][0]))
                        num2 = 1;

                    else
                    {
                        while (!collision[(int)(ghost2_x)][(int)ghost2_y][3])
                        {
                            ghost2_x -= g_speed2;
                            usleep(1000);
                            glutPostRedisplay();
                        }
                        num2 = 4;
                    }

                    // printf("4 quad, num : %d \n \n",num2);
                }
            }
        }
        else if (power_up)
        {
            if (px == ghost2_x || py == ghost2_y)
            {
                if (px == ghost2_x)
                {
                    // printf("hello 1\n");
                    if (py < ghost2_y)
                    {
                        num2 = 4;
                    }
                    else if (py > ghost2_y)
                    {
                        num2 = 3;
                    }
                }
                else if (py == ghost2_y)
                {
                    // printf("hello 2\n");
                    if (px < ghost2_x)
                    {
                        num2 = 1;
                    }
                    else if (px > ghost2_x)
                    {
                        num2 = 2;
                    }
                }
            }

            else
            {
                if (px > ghost2_x && py < ghost2_y && px - ghost2_x > 40 && ghost2_y - py > 40)
                {
                    if (collision[(int)(ghost2_x)][(int)ghost2_y][3])
                    {
                        num2 = 4;
                    }
                    else if (collision[(int)(ghost2_x)][(int)ghost2_y][1])
                        num2 = 2;

                    // printf("1 quad, num : %d \n \n ",num2);
                }

                else if (px < ghost2_x && py < ghost2_y && ghost2_x - px > 40 && ghost2_y - py > 40)
                {
                    if (collision[(int)(ghost2_x)][(int)ghost2_y][3])
                    {
                        num2 = 4;
                    }
                    else if ((collision[(int)(ghost2_x)][(int)ghost2_y][2]))
                        num2 = 1;

                    // printf("2 quad, num : %d \n \n",num2);
                }

                else if (px < ghost2_x && py > ghost2_y && ghost2_x - px > 40 && py - ghost2_x > 40)
                {
                    if (collision[(int)(ghost2_x)][(int)ghost2_y][2])
                    {
                        num2 = 3;
                    }
                    else if ((collision[(int)(ghost2_x)][(int)ghost2_y][0]))
                        num2 = 1;

                    // printf("3 quad, num : %d \n \n",num2);
                }

                else if (px > ghost2_x && py > ghost2_y && px - ghost2_x > 40 && py - ghost2_y > 40)
                {
                    if (collision[(int)(ghost2_x)][(int)ghost2_y][2])
                    {
                        num2 = 3;
                    }
                    else if ((collision[(int)(ghost2_x)][(int)ghost2_y][1]))
                        num2 = 2;

                    // printf("4 quad, num : %d \n \n",num2);
                }
            }
        }
        switch (num2)
        {
        case 1: // Right

            if (ghost2_x < 670 && collision[(int)(ghost2_x)][(int)ghost2_y][0])
                ghost2_x += g_speed2;

            break;
        case 2: // Left

            if (ghost2_x > 20 && collision[(int)(ghost2_x)][(int)ghost2_y][1])
                ghost2_x -= g_speed2;

            break;
        case 3: // Up

            if (ghost2_y < 760 && collision[(int)ghost2_x][(int)ghost2_y][2])
                ghost2_y -= g_speed2;

            break;
        case 4: // Down
            if (ghost2_y > 20 && collision[(int)ghost2_x][(int)ghost2_y][3])
                ghost2_y += g_speed2;

            break;
        }
        
    }
    sem_post(&ghost2_coords);

    glutPostRedisplay();

    sem_wait(&speed2);
    glutTimerFunc(g_delay2, smartghost2, 0);
    sem_post(&speed2);
    
}

void end_power_up()
{

    sem_wait(&power_sema);
    sem_wait(&power_pellets);
    power_up = 0;
    power_coords[power_index].taken = 0;
    sem_post(&power_sema);
    sem_post(&power_pellets);
}

void gameover()
{
    sem_wait(&heart_sema);
    if (lives == 0)
        exit(0);
    sem_post(&heart_sema);
}

void handleBoostTimeout(int index)
{
    sem_wait(&counting);
    activeboostcount--;
    boosts[index] = 0;
    sem_post(&counting);
    //printf("Slow index : %d\n", index);

    switch (index)
    {
    case 0:
        sem_wait(&speed1);
        g_delay += 5;
        sem_post(&speed1);
        break;
    case 1:
        sem_wait(&speed2);
        g_delay2 += 5;
        sem_post(&speed2);
        break;
    case 2:
        sem_wait(&speed3);
        g_delay3 += 5;
        sem_post(&speed3);
        break;
    case 3:
        sem_wait(&speed4);
        g_delay4 += 5;
        sem_post(&speed4);
        break;
    }
}


void handleSpeedBoost()
{


    int index, max;
    index = rand() % 4;
    
    
    
    if(activeboostcount < 2 && boosts[index] == 0)
    {    
        sem_wait(&counting);
        activeboostcount++;
        boosts[index] = 1;
        sem_post(&counting);
        
        //printf("Boost index : %d\n", index);
    switch (index)
    {
        case 0:
            sem_wait(&speed1);
            g_delay -= 5;
            sem_post(&speed1);
               break;
        case 1:
            sem_wait(&speed2);
            g_delay2 -= 5;
            sem_post(&speed2);
                break;
        case 2:
            sem_wait(&speed3);
            g_delay3 -= 5;
            sem_post(&speed3);
                break;
        case 3:
            sem_wait(&speed4);
            g_delay4 -= 5;
            sem_post(&speed4);
                break;
    }
    glutTimerFunc(5000, handleBoostTimeout, index);
    }
    

    glutTimerFunc(1000, handleSpeedBoost, 0);

}

void collisions()
{
    sem_wait(&player_coords);
    sem_wait(&pellets_coords);
    for (int i = -20; i < 20; i++)
    {
        for (int j = -20; j < 20; j++)
        {
            if (pellets[(int)x + i][(int)y + j])
            {
                pellets[(int)x + i][(int)y + j] = 0;
                score += 10;
            }
        }
    }
    sem_post(&player_coords);
    sem_post(&pellets_coords);
    float px, py, gx, gy;
    sem_wait(&player_coords);
    px = x;
    py = y;
    sem_post(&player_coords);

    sem_wait(&ghost1_coords);
    gx = ghost_x;
    gy = ghost_y;
    sem_post(&ghost1_coords);

    if (gx > (px - 20) && gx < (px + 20) && gy > (py - 20) && gy < (py + 20))
    {
        sem_wait(&power_sema);
        bool p = power_up;
        sem_post(&power_sema);

        if (p)
        {
            sem_wait(&ghost1_coords);
            ghost_x = 300.0;
            ghost_y = 380;
            sem_post(&ghost1_coords);
            sem_wait(&house_sema);
            house[0] = 0;
            sem_post(&house_sema);
        }
        else
        {
            sem_wait(&heart_sema);
            lives--;
            heart[lives] = 0;
            sem_post(&heart_sema);
            sem_wait(&player_coords);
            x = 30.0f;
            y = 30.0f;
            sem_post(&player_coords);
        }
    }
    sem_wait(&ghost2_coords);
    gx = ghost2_x;
    gy = ghost2_y;
    sem_post(&ghost2_coords);

    if (gx > (px - 20) && gx < (px + 20) && gy > (py - 20) && gy < (py + 20))
    {
        sem_wait(&power_sema);
        bool p = power_up;
        sem_post(&power_sema);

        if (p)
        {
            sem_wait(&ghost2_coords);
            ghost2_x = 330.0;
            ghost2_y = 380;
            sem_post(&ghost2_coords);
            sem_wait(&house_sema);
            house[1] = 0;
            sem_post(&house_sema);
        }
        else
        {
            sem_wait(&heart_sema);
            lives--;
            heart[lives] = 0;
            sem_post(&heart_sema);
            sem_wait(&player_coords);
            x = 30.0f;
            y = 30.0f;
            sem_post(&player_coords);
        }
    }

    sem_wait(&ghost3_coords);
    gx = ghost3_x;
    gy = ghost3_y;
    sem_post(&ghost3_coords);

    if (gx > (px - 20) && gx < (px + 20) && gy > (py - 20) && gy < (py + 20))
    {
        sem_wait(&power_sema);
        bool p = power_up;
        sem_post(&power_sema);

        if (p)
        {
            sem_wait(&ghost3_coords);
            ghost3_x = 360.0;
            ghost3_y = 380;
            sem_post(&ghost3_coords);
            sem_wait(&house_sema);
            house[2] = 0;
            sem_post(&house_sema);
        }
        else
        {
            sem_wait(&heart_sema);
            lives--;
            heart[lives] = 0;
            sem_post(&heart_sema);
            sem_wait(&player_coords);
            x = 30.0f;
            y = 30.0f;
            sem_post(&player_coords);
        }
    }

    sem_wait(&ghost4_coords);
    gx = ghost4_x;
    gy = ghost4_y;
    sem_post(&ghost4_coords);

    if (gx > (px - 20) && gx < (px + 20) && gy > (py - 20) && gy < (py + 20))
    {
        sem_wait(&power_sema);
        bool p = power_up;
        sem_post(&power_sema);

        if (p)
        {
            sem_wait(&ghost4_coords);
            ghost4_x = 390.0;
            ghost4_y = 380;
            sem_post(&ghost4_coords);
            sem_wait(&house_sema);
            house[3] = 0;
            sem_post(&house_sema);
        }
        else
        {
            sem_wait(&heart_sema);
            lives--;
            heart[lives] = 0;
            sem_post(&heart_sema);
            sem_wait(&player_coords);
            x = 30.0f;
            y = 30.0f;
            sem_post(&player_coords);
        }
    }

    sem_wait(&player_coords);
    px = x;
    py = y;
    sem_post(&player_coords);
    sem_wait(&power_pellets);
    sem_wait(&power_sema);
    bool temp = power_up;
    sem_post(&power_sema);
    if (!temp)
    {
        for (int i = 0; i < 6; i++)
        {
            if (power_coords[i].power_x > (x - 20) && power_coords[i].power_x < (x + 20) && power_coords[i].power_y > (py - 20) && power_coords[i].power_y < (py + 20))
            {
                power_coords[i].taken = 1;
                sem_wait(&power_sema);
                power_up = 1;
                power_index = i;
                sem_post(&power_sema);
                power_up_time = time(NULL);
                glutTimerFunc(5000, end_power_up, 0);
                break;
            }
        }
    }
    sem_wait(&ghost2_coords);
    int gx2 = ghost2_x;
    int gy2 = ghost2_y;
    sem_post(&ghost2_coords);

    sem_wait(&ghost4_coords);
    int gx4 = ghost4_x;
    int gy4 = ghost4_y;
    sem_post(&ghost4_coords);

    sem_wait(&ghost2_coords);
    if (gx2 == gx4 && gy2 == gy4)
    {
        if (collision[(int)ghost2_x][(int)ghost2_y][2])
            while (collision[(int)ghost2_x][(int)ghost2_y][2])
            {
                sem_wait(&speed2);
                ghost2_y -= g_speed2;
                sem_post(&speed2);
                usleep(1000);
                glutPostRedisplay();
            }
        // else
        // {
        //     while (collision[(int)ghost2_x][(int)ghost2_y][0])
        //     {
        //         ghost2_x += g_speed;
        //         usleep(1000);
        //         glutPostRedisplay();
        //     }
        // }
    }
    sem_post(&ghost2_coords);

    gameover();

    sem_post(&power_pellets);
}

void initOpenGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    textureID = SOIL_load_OGL_texture(
        "Untitled.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    texture_up = SOIL_load_OGL_texture(
        "up.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    texture_down = SOIL_load_OGL_texture(
        "down.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    texture_left = SOIL_load_OGL_texture(
        "left.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    texture_right = SOIL_load_OGL_texture(
        "right.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    texture_ghost1 = SOIL_load_OGL_texture(
        "ghost1.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    texture_ghost2 = SOIL_load_OGL_texture(
        "ghost2.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
    texture_pellets = SOIL_load_OGL_texture(
        "pellets.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    texture_power = SOIL_load_OGL_texture(
        "power.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    texture_ghost3 = SOIL_load_OGL_texture(
        "ghost3.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    texture_ghost4 = SOIL_load_OGL_texture(
        "ghost4.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    texture_heart = SOIL_load_OGL_texture(
        "heart.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    if (textureID == 0 || texture_up == 0 || texture_down == 0 || texture_left == 0 || texture_right == 0 || texture_ghost1 == 0 || texture_ghost2 == 0 || texture_pellets == 0 || texture_power == 0 || texture_ghost3 == 0 || texture_ghost4 == 0 || texture_heart == 0)
    {
        printf("Error loading image: %s\n", SOIL_last_result());
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void *ghost_movement(void *yes)
{
    glutTimerFunc(g_delay, dumbghost, 0);
    pthread_exit(NULL);
}

void *ghost_movement2(void *yes)
{
    glutTimerFunc(g_delay, smartghost2, 0);
    pthread_exit(NULL);
}

void *ghost_movement3(void *yes)
{
    glutTimerFunc(g_delay, dumbghost2, 0);
    pthread_exit(NULL);
}

void *ghost_movement4(void *yes)
{
    glutTimerFunc(g_delay, smartghost, 0);
    pthread_exit(NULL);
}

void *player_movement(void *yes)
{
    glutSpecialFunc(keyboard);
    glutTimerFunc(delay, timer, 0);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    // main UI
    sem_init(&power_pellets, 0, 1);
    sem_init(&pellets_coords, 0, 1);
    sem_init(&house_sema, 0, 1);
    sem_init(&power_sema, 0, 1);
    sem_init(&heart_sema, 0, 1);

    sem_init(&speed1, 0, 1);
    sem_init(&speed2, 0, 1);
    sem_init(&speed3, 0, 1);
    sem_init(&speed4, 0, 1);
    sem_init(&counting, 0, 1);

    srand(time(0));
    initializeCollisionArray();
    initializePelletArray();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(window_x, window_y);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("PACMAN");
    glutDisplayFunc(display);
    initOpenGL();

    pthread_t player;
    sem_init(&player_coords, 0, 1);

    pthread_create(&player, NULL, player_movement, NULL);
    pthread_t ghost1, ghost2, ghost3, ghost4;

    sem_init(&ghost1_coords, 0, 1);
    pthread_create(&ghost1, NULL, ghost_movement, NULL);
    sem_init(&ghost2_coords, 0, 1);
    pthread_create(&ghost2, NULL, ghost_movement2, NULL);

    sem_init(&ghost3_coords, 0, 1);
    pthread_create(&ghost3, NULL, ghost_movement3, NULL);

    sem_init(&ghost4_coords, 0, 1);
    pthread_create(&ghost4, NULL, ghost_movement4, NULL);

    glutTimerFunc(2000, handleSpeedBoost, 0);
    glutIdleFunc(collisions);
    glutMainLoop();
    return 0;
}