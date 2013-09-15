#include "common.h"


static Info info[2];
static int ship_size[SHIP_NUM] = {0, 5, 4, 3, 3, 2};

void initStatus(){
  int i;
  for(i=0; i<2; i++){
    memset(&info[i], 0, sizeof(Info));
    memcpy(info[i].ship, ship_size, SHIP_NUM*sizeof(int));
    info[i].ship_num = SHIP_NUM - 1;
  }
}

int attack(int x, int y, int person){
  Board board = info[person].board[x][y];
  int ret;
  if(board.status == ATTACKED){
    ret = ALREADY;
  } else {
    info[person].board[x][y].status = ATTACKED;
    if(board.type != NONE){
      info[person].ship[board.type]--;
      if(info[person].ship[board.type] == 0){
        info[person].ship_num--;
      }
      ret = HIT;
    } else {
      ret = MISS;
    }
  }
  return ret;
}

int getShipNum(int person){
  return info[person].ship_num;
}


void debugPrint(){
  printf("YOU\n");
  int i;
  for(i=0; i<10; i++){
    printf("%d %d %d %d %d %d %d %d %d %d\n", info[YOU].board[i][0].type, info[YOU].board[i][1].type, info[YOU].board[i][2].type, info[YOU].board[i][3].type, info[YOU].board[i][4].type, info[YOU].board[i][5].type, info[YOU].board[i][6].type, info[YOU].board[i][7].type, info[YOU].board[i][8].type, info[YOU].board[i][9].type);
  }
  printf("RIVAL\n");
  for(i=0; i<10; i++){
    printf("%d %d %d %d %d %d %d %d %d %d\n", info[RIVAL].board[i][0].type, info[RIVAL].board[i][1].type, info[RIVAL].board[i][2].type, info[RIVAL].board[i][3].type, info[RIVAL].board[i][4].type, info[RIVAL].board[i][5].type, info[RIVAL].board[i][6].type, info[RIVAL].board[i][7].type, info[RIVAL].board[i][8].type, info[RIVAL].board[i][9].type);
  }
}

int setShip(int x, int y, int person, int type, int direction){
  int i, max;
  if(direction == VERTICAL){
    max = y + ship_size[type];
    if(x < 0 || x > 9 || y < 0 || max > 9){
      return -1;
    }
    for(i=y; i<max; i++){
      if(info[person].board[x][i].type != NONE){
        return -1;
      }
    }
    for(i=y; i<max; i++){
      info[person].board[x][i].type = type;
    }
  } else {
    max = x + ship_size[type];
    if(x < 0 || max > 9 || y < 0 || y > 9){
      return -1;
    }
    for(i=x; i<max; i++){
      if(info[person].board[i][y].type != NONE){
        return -1;
      }
    }
    for(i=x; i<max; i++){
      info[person].board[i][y].type = type;
    }
  }
  return 0;
}

void setShipRandom(int person){
  int i;
  SceKernelUtilsMt19937Context ctx;
  sceKernelUtilsMt19937Init(&ctx, clock());
  for(i=AIRCRAFT_CARRIER; i<SHIP_NUM; i++){
    int ret = 0;
    do {
      u32 rand = sceKernelUtilsMt19937UInt(&ctx);
      int x = rand%10;
      rand /= 10;
      int y = rand%10;
      rand /= 10;
      int direction = rand&1;
      ret = setShip(x, y, person, i, direction);
    } while(ret < 0);
  }
}

Board getBoardStatus(int person, int x, int y){
  return info[person].board[x][y];
}
