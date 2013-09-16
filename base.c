#include "common.h"


static Info info[2];
static int ship_size[SHIP_NUM] = {0, 5, 4, 3, 3, 2};

void initStatus(){
  int i, j;
  for(i=0; i<2; i++){
    memset(&info[i], 0, sizeof(Info));
    for(j=0; j<SHIP_NUM; j++){
      info[i].ship[j].rest = ship_size[j];
    }
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
      info[person].ship[board.type].rest--;
      if(info[person].ship[board.type].rest == 0){
        info[person].ship_num--;
      }
      ret = HIT;
    } else {
      ret = MISS;
    }
  }
  return ret;
}

Ship getShipInfo(int person, int type){
  return info[person].ship[type];
}

int getShipNum(int person){
  return info[person].ship_num;
}


int setShip(int x, int y, int person, int type, int direction){
  int i, max;
  if(direction == VERTICAL){
    max = y + ship_size[type] - 1;
    if(x < 0 || x > 9 || y < 0 || max > 9){
      return -1;
    }
    for(i=y; i<=max; i++){
      if(info[person].board[x][i].type != NONE){
        return -1;
      }
    }
    for(i=y; i<=max; i++){
      info[person].board[x][i].type = type;
    }
  } else {
    max = x + ship_size[type] - 1;
    if(x < 0 || max > 9 || y < 0 || y > 9){
      return -1;
    }
    for(i=x; i<=max; i++){
      if(info[person].board[i][y].type != NONE){
        return -1;
      }
    }
    for(i=x; i<=max; i++){
      info[person].board[i][y].type = type;
    }
  }
  info[person].ship[type].head_x = x;
  info[person].ship[type].head_y = y;
  info[person].ship[type].direction = direction;
  return 0;
}

int deleteShip(int person, int type){
  if(type == NONE){
    return -1;
  }
  int x = info[person].ship[type].head_x;
  int y = info[person].ship[type].head_y;
  if(info[person].board[x][y].type == NONE){
    return -1;
  }
  int direction = info[person].ship[type].direction;
  int i;
  if(direction == VERTICAL){
    int max = y + ship_size[type] - 1;
    for(i=y; i<=max; i++){
      info[person].board[x][i].type = NONE;
    }
  } else {
    int max = x + ship_size[type] - 1;
    for(i=x; i<=max; i++){
      info[person].board[i][y].type = NONE;
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
