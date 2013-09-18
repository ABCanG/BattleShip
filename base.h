#ifndef BASE_H
#define BASE_H

enum {
  NONE = 0,
  AIRCRAFT_CARRIER,
  BATTLESHIP,
  SUBMARINE,
  DESTROYER,
  PATROL_BOAT,
  SHIP_NUM
};

enum {
  SAFE = 0,
  ATTACKED
};

enum {
  YOU = 0,
  RIVAL
};

enum {
  ALREADY = -1,
  MISS,
  HIT
};

#define VERTICAL (0)
#define HORIZON (1)


typedef struct Board {
  int status;
  int type;
} Board;

typedef struct Ship {
  int rest;
  int head_x;
  int head_y;
  int direction;
} Ship;

typedef struct Info {
  Board board[10][10];
  Ship ship[SHIP_NUM];
  int ship_num;
} Info;



void initStatus();
int attack(int x, int y, int person);
int setShip(int x, int y, int person, int type, int direction);
void setShipRandom(int person);
int getShipNum(int person);
Info getYourInfo();
void setRivalInfo(Info info);
Ship getShipInfo(int person, int type);
Board getBoardStatus(int person, int x, int y);
int deleteShip(int person, int type);

#endif
