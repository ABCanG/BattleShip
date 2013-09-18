#ifndef GAME_H
#define GAME_H

void startGameCpu();
void startGameAdhoc();
int selectAndAttack(int *x, int *y);
int finishGame(int person);
int askReturnTitle(int x, int y);
void changeShipPos();
#endif
