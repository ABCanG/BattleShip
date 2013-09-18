#ifndef DRAW_H
#define DRAW_H

#include "intraFont/intraFont.h"

enum colors {
  RED =0xFF0000FF,
  GREEN =0xFF00FF00,
  BLUE =0xFFFF0000,
  WHITE =0xFFFFFFFF,
  LITEGRAY = 0xFFBFBFBF,
  GRAY =  0xFF7F7F7F,
  DARKGRAY = 0xFF3F3F3F,
  BLACK = 0xFF000000,
  CADETBLUE = 0xFFA09E5F,
  YELLOW = 0xFF00FFFF,
  TEAL = 0xFF808000
};


typedef struct
{
  unsigned short u, v;
  //  unsigned short color;
  short x, y, z;
} Vertex;

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

#define SLICE_SIZE (64)

void initDraw();
void clearScreen(int color);
void startDraw(int color);
void endDraw();
void changeStyle(float size, unsigned int main, unsigned int edge, unsigned int opt);
float printText(float x, float y, const char *text, ...);
float measureText(const char *text, ...);
float printTextCenter(int y, char *text, ...);
u32 getFlashColor(void);
void drawImage(int x, int y, int width, int height, void *tex, int size);
void drawBox(int x1, int y1, int x2, int y2, unsigned int color);
void drawLine(int x1, int y1, int x2, int y2, unsigned int color);
void drawPoint(int x, int y, unsigned int color);
void drawBackground();
void drawBoard(int person);
void drawShip(int person, int print_ship);
void drawNowPos(int x, int y, int person);
void drawSelecetdShip(int x, int y, int person, int type, int direction);
void setBoardPos(int x, int y, int person);
void drawGameScreen(int x, int y, int finish_flag);

#endif
