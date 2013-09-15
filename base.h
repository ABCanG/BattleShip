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

typedef struct Info {
  Board board[10][10];
  int ship[SHIP_NUM];
  int ship_num;
} Info;


void initStatus();
int attack(int x, int y, int person);
int setShip(int x, int y, int person, int type, int direction);
void setShipRandom(int person);
int getShipNum(int person);
Board getBoardStatus(int person, int x, int y);
void debugPrint();

#endif
