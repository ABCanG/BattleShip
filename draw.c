#include <time.h>

#include <stdarg.h>
#include "common.h"
#include "draw.h"

static intraFont* ltn0;
static unsigned int __attribute__((aligned(16))) list[262144];
extern unsigned char background_start[];
static int board_pos_x[2];
static int board_pos_y[2];

void initDraw(){
  intraFontInit();

  ltn0 = intraFontLoad("flash0:/font/ltn0.pgf",0);
  intraFontSetStyle(ltn0, 1.0f, WHITE, BLACK, 0);

  sceGuInit();
  sceGuStart(GU_DIRECT, list);
  sceGuDrawBuffer(GU_PSM_8888, (void*)0, BUF_WIDTH);
  sceGuDispBuffer(SCR_WIDTH , SCR_HEIGHT , (void*)0x88000 , BUF_WIDTH );
  sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
  sceGuDepthBuffer( (void*)0x110000 , BUF_WIDTH );
  sceGuOffset(2048 - (SCR_WIDTH/2), 2048 - (SCR_HEIGHT/2));
  sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);
  sceGuDepthRange(0xC350, 0x2710);
  sceGuDepthFunc(GU_GEQUAL);
  sceGuEnable(GU_DEPTH_TEST);
  sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
  sceGuEnable(GU_SCISSOR_TEST);
  sceGuShadeModel(GU_SMOOTH);
  sceGuEnable(GU_CLIP_PLANES);
  sceGuFrontFace(GU_CW);
  sceGuEnable(GU_CULL_FACE);
  //sceGuTexMode(GU_PSM_8888, 0, 0, 0);
  sceGuTexMode(GU_PSM_8888, 0, 0, 1);
  sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
  sceGuTexFilter(GU_NEAREST, GU_NEAREST);
  //sceGuTexFilter(GU_LINEAR, GU_LINEAR);
  sceGuEnable(GU_TEXTURE_2D);
  sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
  sceGuEnable(GU_BLEND);
  sceGuAlphaFunc(GU_GREATER, 0, 0xFF);
  sceGuEnable(GU_ALPHA_TEST);
  sceGuAmbientColor(WHITE);
  sceGuClearColor(GRAY);
  sceGuClearDepth(0);
  sceGuColor(GRAY);
  sceGuAmbientColor(WHITE);
  sceGuFinish();
  sceGuSync(GU_SYNC_WHAT_DONE, GU_SYNC_FINISH);
  sceGuDisplay(GU_TRUE);
}



void clearScreen(int color){
  sceGuStart(GU_DIRECT, list);
  sceGuClearColor(color);
  sceGuClearDepth(0);
  sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
  sceGuFinish();
  sceGuSync(0,0);
}

void startDraw(int color){
  sceGuStart(GU_DIRECT, list);

  sceGumMatrixMode(GU_PROJECTION);
  sceGumLoadIdentity();
  sceGumPerspective( 75.0f, 16.0f/9.0f, 0.5f, 1000.0f);

  sceGumMatrixMode(GU_VIEW);
  sceGumLoadIdentity();

  sceGumMatrixMode(GU_MODEL);
  sceGumLoadIdentity();

  sceGuClearColor(color);
  sceGuClearDepth(0);
  sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
}

void endDraw(){
  sceGuFinish();
  sceGuSync(0,0);

  sceDisplayWaitVblankStart();
  sceGuSwapBuffers();
}

void changeStyle(float size, unsigned int main, unsigned int edge, unsigned int opt){
  intraFontSetStyle(ltn0, size, main, edge, opt);
}

float printText(float x, float y, const char *text, ...){
  char buffer[256];
  va_list ap;

  va_start(ap, text);
  vsnprintf(buffer, 256, text, ap);
  va_end(ap);
  buffer[255] = 0;

  return intraFontPrint(ltn0, x, y, buffer);
}


float measureText(const char *text, ...){
  char buffer[256];
  va_list ap;

  va_start(ap, text);
  vsnprintf(buffer, 256, text, ap);
  va_end(ap);
  buffer[255] = 0;

  return intraFontMeasureText(ltn0, buffer);
}

float printTextCenter(int y, char *text, ...){
  char buffer[256];
  va_list ap;

  va_start(ap, text);
  vsnprintf(buffer, 256, text, ap);
  va_end(ap);
  buffer[255] = 0;
  return printText(240 - (int)measureText(buffer)/2, y, buffer);
}


// thanks SystemMenu v8

u32 getFlashColor(void){
  float t;
  int val;

  t = ((float)(clock() % CLOCKS_PER_SEC)) / ((float)CLOCKS_PER_SEC);
  val = (t < 0.5f) ? t * 511 : (1.0f - t) * 511;

  return ((0x80|val) << 24) + (val << 16) + (val << 8) + (val);
}


//thanks LibImg by Dadrfy
void drawImage(int x, int y, int width, int height, void *tex, int size){
  sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
  sceGuTexImage(0, size, size, size, tex);

  int j = 0;
  while ( j < width ){
    Vertex *vertices = (Vertex*)sceGuGetMemory(2 * sizeof(Vertex));

    int slice = SLICE_SIZE;
    if ( j + slice > width ) {
      slice = width - j;
    }

    vertices[0].u = j;
    vertices[0].v = 0;
    vertices[1].u = j + slice;
    vertices[1].v = height;

    vertices[0].x = x + j;
    vertices[0].y = y;
    vertices[0].z = 0;
    vertices[1].x = x + j + slice;
    vertices[1].y = y + height;
    vertices[1].z = 0;

    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
    j += slice;
  }
}

void drawBox(int x1, int y1, int x2, int y2, unsigned int color){
  Vertex *vertices = (Vertex*)sceGuGetMemory(2 * sizeof(Vertex));

  vertices[0].x = x1;
  vertices[0].y = y1;
  vertices[0].z = 0;
  vertices[1].x = x2;
  vertices[1].y = y2;
  vertices[1].z = 0;

  sceGuDisable(GU_TEXTURE_2D);
  sceGuColor(color);
  sceGuDrawArray(GU_SPRITES, GU_VERTEX_16BIT | GU_TEXTURE_16BIT | GU_TRANSFORM_2D, 2, NULL, vertices);
  sceGuEnable(GU_TEXTURE_2D);

  sceGuColor(0xFFFFFFFF);

  sceKernelDcacheWritebackInvalidateAll();
}

void drawLine(int x1, int y1, int x2, int y2, unsigned int color){
  sceKernelDcacheWritebackInvalidateAll();

  Vertex *vertices = (Vertex*)sceGuGetMemory(2 * sizeof(Vertex));

  vertices[0].x = x1;
  vertices[0].y = y1;
  vertices[0].z = 0;
  vertices[1].x = x2;
  vertices[1].y = y2;
  vertices[1].z = 0;

  sceGuDisable(GU_TEXTURE_2D);
  sceGuColor(color);
  sceGuDrawArray(GU_LINES, GU_VERTEX_16BIT | GU_TEXTURE_16BIT | GU_TRANSFORM_2D, 2, NULL, vertices);
  sceGuEnable(GU_TEXTURE_2D);

  sceGuColor(0xFFFFFFFF);

  sceKernelDcacheWritebackInvalidateAll();

}

void drawPoint(int x, int y, unsigned int color){
  Vertex *vertices = (Vertex*)sceGuGetMemory(sizeof(Vertex));

  vertices[0].x = x;
  vertices[0].y = y;
  vertices[0].z = 0;

  sceGuDisable(GU_TEXTURE_2D);
  sceGuColor(color);
  sceGuDrawArray(GU_POINTS, GU_VERTEX_16BIT | GU_TRANSFORM_2D, 1, NULL, vertices);
  sceGuEnable(GU_TEXTURE_2D);

}

void drawBackground(){
  drawImage(0, 0, 480, 272, background_start, 512);
}

void drawBoard(int person){
  int i;
  int color = BLACK;
  int x = board_pos_x[person];
  int y = board_pos_y[person];
  for(i=0; i<=10; i++){
    drawLine(x + 20*i, y, x + 20*i, y + 201, color);
    drawLine(x, y + 20*i, x + 201, y + 20*i, color);
  }
}

void drawShip(int person, int print_ship){
  int x = board_pos_x[person];
  int y = board_pos_y[person];
  x++; y++;
  int i, j;
  for(i=0; i<10; i++){
    for(j=0; j<10; j++){
      Board board = getBoardStatus(person, i, j);
      if(board.type != NONE){
        if(print_ship){
          drawBox(x + i*20, y + j*20, x + i*20 + 19, y + j*20 + 19, GRAY);
        }
        if(board.status == ATTACKED){
          drawLine(x + i*20, y + j*20, x + i*20 + 19, y + j*20 + 19, RED);
          drawLine(x + i*20 + 19, y + j*20, x + i*20, y + j*20 + 19, RED);
        }
      } else {
        if(board.status == ATTACKED){
          drawBox(x + i*20, y + j*20, x + i*20 + 19, y + j*20 + 19, TEAL);
          //          drawLine(x + i*20, y + j*20, x + i*20 + 19, y + j*20 + 19, BLACK);
          //drawLine(x + i*20 + 19, y + j*20, x + i*20, y + j*20 + 19, BLACK);
        }
      }
    }
  }
}

void drawSelecetdShip(int x, int y, int person, int type, int direction){
  int board_x = board_pos_x[person];
  int board_y = board_pos_y[person];
  board_x++; board_y++;
  int i;
  Ship info = getShipInfo(person, type);
  int size = info.rest;
  if(direction == VERTICAL){
    int max = y + size - 1;
    for(i=y; i<=max; i++){
      drawBox(board_x + x*20, board_y + i*20, board_x + x*20 + 19, board_y + i*20 + 19, 0x80FFFFFF & YELLOW);
    }
  } else {
    int max = x + size - 1;
    for(i=x; i<=max; i++){
      drawBox(board_x + i*20, board_y + y*20, board_x + i*20 + 19, board_y + y*20 + 19, 0x80FFFFFF & YELLOW);
    }
  }
}


void drawNowPos(int x, int y, int person){
  int board_x = board_pos_x[person];
  int board_y = board_pos_y[person];
  board_x++; board_y++;
  drawBox(board_x + x*20, board_y + y*20, board_x + x*20 + 19, board_y + y*20 + 19, 0x80FFFFFF & YELLOW);
}

void setBoardPos(int x, int y, int person){
  board_pos_x[person] = x;
  board_pos_y[person] = y;
}

void drawGameScreen(int x, int y, int finish_flag){
  drawBoard(YOU);
  drawShip(YOU, 1);
  drawBoard(RIVAL);
  drawShip(RIVAL, finish_flag);
  if(!finish_flag)drawNowPos(x, y, RIVAL);
  printText(board_pos_x[YOU], 20, "YOU %d", getShipNum(YOU));
  printText(board_pos_x[RIVAL], 20, "RIVAL %d", getShipNum(RIVAL));
}
