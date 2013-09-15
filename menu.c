#include "common.h"
#include "menu.h"
#include <time.h>

#define MENU_NUM (3)

static int gamesharing = 0;

void disableGameSharing(){
  gamesharing = 1;
}

int startMenu(){
  SceCtrlData pad;
  int i, pos = 0;
  char *menu_text[MENU_NUM] = {"CPU", "ADHOC", "GAME SHARING"};
  int menu_num = MENU_NUM - gamesharing;
  while(1){
    sceCtrlReadBufferPositive(&pad, 1);
    startDraw(BLACK);
    drawBackground();
    changeStyle(2.0, WHITE, BLACK, 0);
    printText(10, 30, "Battleship");

    int flash_color = getFlashColor();
    for(i=0; i<menu_num; i++){
      changeStyle(1.0, i == pos ? CADETBLUE : WHITE, i == pos ? flash_color : BLACK, 0);
      printText(480 - 10 - measureText(menu_text[i]), 200 + i*24, menu_text[i]);
    }
    endDraw();

    if(pad.Buttons & PSP_CTRL_DOWN){
      pos++;
      if(pos > menu_num - 1)pos = 0;
      sceKernelDelayThread(100*1000);
    } else if(pad.Buttons & PSP_CTRL_UP){
      pos--;
      if(pos < 0)pos = menu_num - 1;
      sceKernelDelayThread(100*1000);
    } else if(pad.Buttons & PSP_CTRL_CIRCLE){
      break;
    }
    sceKernelDelayThread(10*1000);
  }
  return pos;
}
