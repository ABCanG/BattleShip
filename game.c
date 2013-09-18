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
 REPLAY:
  initStatus();
  setShipRandom(YOU);
  changeShipPos();
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
        int y = (rand/10)%10;
        result = attack(x, y, YOU);
      }
    }
    turn ^= 1;
    if(getShipNum(turn) == 0 ){
      waitButtonUp(PSP_CTRL_CIRCLE);
      int ret = finishGame(turn);
      if(ret){
        goto REPLAY;
      } else {
        break;
      }
    }
  }
}

int server;
int turn;
int info_exchange_flag;
int set_flag;
int attack_flag;
int attacked_flag;
int exchange_x;
int exchange_y;
int game_set;

int adhoc_thread(SceSize args, void *argp){
  Info info;
  int length;
  ScreenInit();
  if(server){
    sceKernelDelayThread(5*1000000);
    SceKernelUtilsMt19937Context ctx;
    sceKernelUtilsMt19937Init(&ctx, clock());
    //send turn
    turn = sceKernelUtilsMt19937UInt(&ctx)&1;
    int rival_turn = turn^1;
    adhocSendRecvAck(&rival_turn, 4);

    while(!set_flag){
      sceKernelDelayThread(100*1000);
    }

    //send your info
    info = getYourInfo();
    adhocSendRecvAck(&info, sizeof(Info));

    //receive rival info
    length = sizeof(Info);
    adhocRecvSendAck(&info, &length);
    setRivalInfo(info);
  } else {
    //receive turn
    length = 4;
    adhocRecvSendAck(&turn, &length);

    //receive rival info
    length = sizeof(Info);
    adhocRecvSendAck(&info, &length);
    setRivalInfo(info);

    while(!set_flag){
      sceKernelDelayThread(100*1000);
    }
    //send your info
    info = getYourInfo();
    adhocSendRecvAck(&info, sizeof(Info));
  }
  info_exchange_flag = true;
  int next_turn;
  while(!game_set){
    next_turn = turn^1;
    if(turn == YOU){
      while(!attack_flag){
        sceKernelDelayThread(100*1000);
      }
      int data = exchange_x + exchange_y*10;
      adhocSendRecvAck(&data, sizeof(int));
      attack_flag = false;
    } else {
      int data;
      length = sizeof(int);
      adhocRecvSendAck(&data, &length);
      exchange_x = data%10;
      exchange_y = (data/10)%10;
      attacked_flag = true;
    }
    while(turn != next_turn){
      sceKernelDelayThread(100*1000);
    }
  }
  return sceKernelExitDeleteThread( 0 );
}

void startGameAdhoc(){
  pspDebugScreenInit();
  pspSdkLoadAdhocModules();
  int x=0, y=0;

  ScreenInit();
  if ((adhocInit("Battle Ship") < 0) || ((server = adhocSelect()) < 0)){
    adhocTerm();
    return;
  }

 REPLAY:
  initStatus();
  set_flag = false;
  info_exchange_flag = false;
  attack_flag = false;
  attacked_flag = false;
  game_set = false;

  SceUID thid = sceKernelCreateThread("Achoc_Thread", adhoc_thread, 0x18, 256 * 1024, PSP_THREAD_ATTR_USER, NULL);
  sceKernelStartThread(thid, 0, 0);

  setShipRandom(YOU);
  changeShipPos();
  set_flag = true;

  while(!info_exchange_flag){
    startDraw(CADETBLUE);
    drawBoard(25, 36, BLACK);
    drawShip(25, 36, YOU, 1);
    drawBoard(255, 36, BLACK);
    drawShip(255, 36, RIVAL, 0);
    drawNowPos(255, 36, x, y);
    printText(25, 20, "YOU %d", getShipNum(YOU));
    printText(255, 20, "RIVAL %d", getShipNum(RIVAL));
    printTextCenter(265, "please wait...");
    endDraw();
    sceKernelDelayThread(100*1000);
  }
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
      exchange_x = x;
      exchange_y = y;
      attack_flag = true;
    } else {
      SceCtrlData pad;
      while(!attacked_flag){
        startDraw(CADETBLUE);
        drawBoard(25, 36, BLACK);
        drawShip(25, 36, YOU, 1);
        drawBoard(255, 36, BLACK);
        drawShip(255, 36, RIVAL, 0);
        drawNowPos(255, 36, x, y);
        printText(25, 20, "YOU %d", getShipNum(YOU));
        printText(255, 20, "RIVAL %d", getShipNum(RIVAL));
        printTextCenter(265, "Rival turn");
        endDraw();

        sceCtrlReadBufferPositive(&pad, 1);
        if(pad.Buttons & PSP_CTRL_DOWN){
          y++;
          if(y > 9)y = 0;
          sceKernelDelayThread(100*1000);
        } else if(pad.Buttons & PSP_CTRL_UP){
          y--;
          if(y < 0)y = 9;
          sceKernelDelayThread(100*1000);
        }else if(pad.Buttons & PSP_CTRL_RIGHT){
          x++;
          if(x > 9)x = 0;
          sceKernelDelayThread(100*1000);
        } else if(pad.Buttons & PSP_CTRL_LEFT){
          x--;
          if(x < 0)x = 9;
          sceKernelDelayThread(100*1000);
        }
        sceKernelDelayThread(10*1000);
      }
      attack(exchange_x, exchange_y, YOU);
      attacked_flag = false;
    }
    turn ^= 1;
    if(getShipNum(turn) == 0 ){
      game_set = true;
      waitButtonUp(PSP_CTRL_CIRCLE);
      int ret = finishGame(turn);
      if(ret){
        goto REPLAY;
      } else {
        adhocTerm();
        break;
      }
    }
  }
}

int finishGame(int person){
  SceCtrlData pad;
  int ret;
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
      changeStyle(2.0, BLUE, 0x80FFFFFF & BLACK, 0);
      printTextCenter(136, "YOU LOSE");
    } else {
      //win
      changeStyle(2.0, RED, 0x80FFFFFF & BLACK, 0);
      printTextCenter(136, "YOU WIN");
    }
    changeStyle(1.0, WHITE, BLACK, 0);
    printTextCenter(170, "return title to press O");
    printTextCenter(200, "replay to press start");
    endDraw();
    sceCtrlReadBufferPositive(&pad, 1);
    if(pad.Buttons & PSP_CTRL_CIRCLE){
      waitButtonUp(PSP_CTRL_CIRCLE);
      ret = 0;
      break;
    } else if(pad.Buttons & PSP_CTRL_START){
      waitButtonUp(PSP_CTRL_START);
      ret = 1;
      break;
    }
    sceKernelDelayThread(10*1000);
  }
  return ret;
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
    printTextCenter(265, "Your turn");
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

void changeShipPos(){
  SceCtrlData pad;
  int x=0, y=0;
  int direction = VERTICAL;
  int type = NONE;
  int max_x = 9, max_y = 9;
  while(1){
    startDraw(CADETBLUE);
    drawBoard(25, 36, BLACK);
    drawShip(25, 36, YOU, 1);
    if(type == NONE){
      drawNowPos(25, 36, x, y);
    } else {
      drawSelecetdShip(25, 36, x, y, YOU, type, direction);
    }

    printTextCenter(20, "Setting Ship's Position");
    printText(255, 80, "select / release to \npress O");
    printText(255, 130, "move pointer to press \narrow buttons");
    printText(255, 180, "change direction to \npress R or L");
    printText(255, 230, "start game to press \nstart");

    endDraw();

    sceCtrlReadBufferPositive(&pad, 1);
    if(pad.Buttons & PSP_CTRL_DOWN){
      y++;
      if(y > max_y)y = 0;
      sceKernelDelayThread(100*1000);
    } else if(pad.Buttons & PSP_CTRL_UP){
      y--;
      if(y < 0)y = max_y;
      sceKernelDelayThread(100*1000);
    }else if(pad.Buttons & PSP_CTRL_RIGHT){
      x++;
      if(x > max_x)x = 0;
      sceKernelDelayThread(100*1000);
    } else if(pad.Buttons & PSP_CTRL_LEFT){
      x--;
      if(x < 0)x = max_x;
      sceKernelDelayThread(100*1000);
    } else if(pad.Buttons & PSP_CTRL_CIRCLE){
      if(type != NONE){
        int ret = setShip(x, y, YOU, type, direction);
        if(ret == 0){
          type = NONE;
          max_x = max_y = 9;
        }
      } else {
        Board board = getBoardStatus(YOU, x, y);
        type = board.type;
        if(type != NONE){
          Ship info = getShipInfo(YOU, type);
          direction = info.direction;
          x = info.head_x;
          y = info.head_y;
          if(direction == VERTICAL){
            max_y = 9 - info.rest + 1;
          } else {
            max_x = 9 - info.rest + 1;
          }
          deleteShip(YOU, type);
        }
      }
      waitButtonUp(PSP_CTRL_CIRCLE);
    } else if(pad.Buttons & (PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER)){
      if(type != NONE){
        direction ^= HORIZON;
        int tmp = max_x;
        max_x = max_y;
        max_y = tmp;
        if(direction == VERTICAL){
          y++;
          if(y > max_y)y = 0;
          y--;
          if(y < 0)y = max_y;
        } else {
          x++;
          if(x > max_x)x = 0;
          x--;
          if(x < 0)x = max_x;
        }
        waitButtonUp(PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER);
      }
    } else if(pad.Buttons & PSP_CTRL_START){
      if(type == NONE){
        waitButtonUp(PSP_CTRL_START);
        return;
      }
    }
    sceKernelDelayThread(10*1000);
  }


}
