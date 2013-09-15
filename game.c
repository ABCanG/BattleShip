#include "game.h"
#include "common.h"

int waitButtonUp(u32 buttons){
  SceCtrlData pad;

  sceCtrlPeekBufferPositive( &pad, 1 );
  while( (pad.Buttons & buttons) != 0 ){
    sceCtrlPeekBufferPositive( &pad, 1 );
    sceKernelDelayThread( 50000 );
  }

  return 0;
}


void startGameCpu(){
  initStatus();
  setShipRandom(YOU);
  setShipRandom(RIVAL);
  SceKernelUtilsMt19937Context ctx;
  sceKernelUtilsMt19937Init(&ctx, clock());
  int turn = sceKernelUtilsMt19937UInt(&ctx)&1;
  int x=0, y=0;
  while(1){
    startDraw(CADETBLUE);
    drawBoard(25, 36, BLACK);
    drawShip(25, 36, YOU, 1);
    drawBoard(255, 36, BLACK);
    drawShip(255, 36, RIVAL, 0);
    drawNowPos(255, 36, x, y);
    printText(25, 20, "YOU %d", getShipNum(YOU));
    printText(255, 20, "RIVAL %d", getShipNum(RIVAL));
    endDraw();

    waitButtonUp(PSP_CTRL_CIRCLE);

    if(turn == YOU){
      selectAndAttack(&x, &y);
    } else {
      int result = ALREADY;
      while(result == ALREADY){
        u32 rand = sceKernelUtilsMt19937UInt(&ctx);
        int x = rand%10;
        rand /= 10;
        int y = rand%10;
        result = attack(x, y, YOU);
      }
    }
    turn ^= 1;
    if(getShipNum(turn) == 0 ){
      waitButtonUp(PSP_CTRL_CIRCLE);
      finishGame(turn);
      break;
    }
  }

  debugPrint();
}

void startGameAdhoc(){

}

void finishGame(int person){
  SceCtrlData pad;
  while(1){
    startDraw(CADETBLUE);
    drawBoard(25, 36, BLACK);
    drawShip(25, 36, YOU, 1);
    drawBoard(255, 36, BLACK);
    drawShip(255, 36, RIVAL, 1);
    printText(25, 20, "YOU %d", getShipNum(YOU));
    printText(255, 20, "RIVAL %d", getShipNum(RIVAL));

    if(person == YOU){
      //lose
      changeStyle(2.0, 0xD0FFFFFF & BLUE, 0, 0);
      printTextCenter(136, "YOU LOSE");
    } else {
      //win
      changeStyle(2.0, 0xD0FFFFFF & RED, 0, 0);
      printTextCenter(136, "YOU WIN");
    }
    changeStyle(1.0, WHITE, BLACK, 0);
    endDraw();
    sceCtrlReadBufferPositive(&pad, 1);
    if(pad.Buttons & PSP_CTRL_CIRCLE){
      waitButtonUp(PSP_CTRL_CIRCLE);
      return;
    }
    sceKernelDelayThread(10*1000);
  }
}

void selectAndAttack(int *x, int *y){
  SceCtrlData pad;
  int result = ALREADY;
  while(1){
    startDraw(CADETBLUE);
    drawBoard(25, 36, BLACK);
    drawShip(25, 36, YOU, 1);
    drawBoard(255, 36, BLACK);
    drawShip(255, 36, RIVAL, 0);
    drawNowPos(255, 36, *x, *y);
    printText(25, 20, "YOU %d", getShipNum(YOU));
    printText(255, 20, "RIVAL %d", getShipNum(RIVAL));
    endDraw();

    sceCtrlReadBufferPositive(&pad, 1);
    if(pad.Buttons & PSP_CTRL_DOWN){
      (*y)++;
      if(*y > 9)*y = 0;
      sceKernelDelayThread(100*1000);
    } else if(pad.Buttons & PSP_CTRL_UP){
      (*y)--;
      if(*y < 0)*y = 9;
      sceKernelDelayThread(100*1000);
    }else if(pad.Buttons & PSP_CTRL_RIGHT){
      (*x)++;
      if(*x > 9)*x = 0;
      sceKernelDelayThread(100*1000);
    } else if(pad.Buttons & PSP_CTRL_LEFT){
      (*x)--;
      if(*x < 0)*x = 9;
      sceKernelDelayThread(100*1000);
    } else if(pad.Buttons & PSP_CTRL_CIRCLE){
      result = attack(*x, *y, RIVAL);
      if(result != ALREADY)return;
    }
    sceKernelDelayThread(10*1000);
  }
}
