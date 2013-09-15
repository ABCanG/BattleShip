#ifndef MENU_H
#define MENU_H

enum {
  CPU = 0,
  ADHOC,
  GAME_SHARING
};

void disableGameSharing();
int startMenu();

#endif
