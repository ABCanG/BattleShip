#include "common.h"

PSP_MODULE_INFO("BattleShip", PSP_MODULE_USER, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
PSP_HEAP_SIZE_KB(1024);


int exitCallback(int arg1, int arg2, void *common) {
  sceKernelExitGame();
  return 0;
}

int callbackThread(SceSize args, void *argp) {
  int cbid = sceKernelCreateCallback("Exit Callback", exitCallback, NULL);
  sceKernelRegisterExitCallback(cbid);
  sceKernelSleepThreadCB();
  return 0;
}

int setupCallbacks(void) {
  int thid = sceKernelCreateThread("CallbackThread", callbackThread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);
  if (thid >= 0) sceKernelStartThread(thid, 0, 0);
  return thid;
}



int main(int argc, char *argv[]) {
#ifdef SIGN
  disableGameSharing();
#else
  if(argc > 1 && strcmp(argv[2], "GameShar") == 0){
    disableGameSharing();
  }
#endif
  setupCallbacks();
  initDraw();

  while(1) {
    switch(startMenu()){
    case CPU:
      startGameCpu();
      break;
    case ADHOC:
      startGameAdhoc();
      break;
    case GAME_SHARING:
      startGameSharing();
      break;
    }
  }
  return 0;
}
