#include "adhoc.h"
#include "common.h"

// from gamesharing sample


int gsInitDialog(){
  int done = 0;

  pspUtilityNetconfData data;

  memset(&data, 0, sizeof(data));
  data.base.size = sizeof(data);
  sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &data.base.language);
  sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &data.base.buttonSwap);
  data.base.graphicsThread = 17;
  data.base.accessThread = 19;
  data.base.fontThread = 18;
  data.base.soundThread = 16;
  data.action = PSP_NETCONF_ACTION_CONNECT_ADHOC;

  struct pspUtilityNetconfAdhoc adhocparam;
  memset(&adhocparam, 0, sizeof(adhocparam));
  memcpy(&adhocparam.name, "GameShar", 8);
  adhocparam.timeout = 60;
  data.adhocparam = &adhocparam;

  sceUtilityNetconfInitStart(&data);

  while(1){
    clearScreen(BLACK);
    switch(sceUtilityNetconfGetStatus()){
    case PSP_UTILITY_DIALOG_NONE:
      done = 1;
      break;

    case PSP_UTILITY_DIALOG_VISIBLE:
      sceUtilityNetconfUpdate(1);
      break;

    case PSP_UTILITY_DIALOG_QUIT:
      sceUtilityNetconfShutdownStart();
      break;

    case PSP_UTILITY_DIALOG_FINISHED:
      break;

    default:
      break;
    }

    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();

    if(done)
      break;
  }

  return 1;
}

int gsDialog(){
  int done = 0;

  sceNetAdhocMatchingInit(32*1024);

  pspUtilityGameSharingParams params;

  memset(&params, 0, sizeof(params));
  params.base.size = sizeof(params);
  sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &params.base.language);
  sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &params.base.buttonSwap);
  params.base.graphicsThread = 17;
  params.base.accessThread = 19;
  params.base.fontThread = 18;
  params.base.soundThread = 16;

  int fd = sceIoOpen("EBOOT.PBP", PSP_O_RDONLY, 0777);

  params.datasize = sceIoLseek32(fd, 0, PSP_SEEK_END);

  unsigned char *fileBuffer = memalign(64, params.datasize);

  sceIoLseek32(fd, 0, PSP_SEEK_SET);

  sceIoRead(fd, fileBuffer, params.datasize);

  sceIoClose(fd);

  // Manually patch the PARAM.SFO in the EBOOT
  fileBuffer[276] = 0x57;
  // And add a custom filename
  //strncpy((char *) &fileBuffer[320], "BattleShip", 127);

  memcpy(&params.name, "GameShar", 8);

  params.mode = PSP_UTILITY_GAMESHARING_MODE_MULTIPLE;
  params.datatype = PSP_UTILITY_GAMESHARING_DATA_TYPE_MEMORY;

  params.data = fileBuffer;

  sceUtilityGameSharingInitStart(&params);

  while(1){
    clearScreen(BLACK);
    switch(sceUtilityGameSharingGetStatus()){
    case PSP_UTILITY_DIALOG_NONE:
      sceNetAdhocMatchingTerm();
      done = 1;
      break;

    case PSP_UTILITY_DIALOG_VISIBLE:
      sceUtilityGameSharingUpdate(1);
      break;

    case PSP_UTILITY_DIALOG_QUIT:
      sceUtilityGameSharingShutdownStart();
      break;

    case PSP_UTILITY_DIALOG_FINISHED:
      break;

    default:
      break;
    }

    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();

    if(done)
      break;
  }

  free(fileBuffer);

  return 1;
}

void gsInit(void){
  sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);

  sceUtilityLoadNetModule(PSP_NET_MODULE_ADHOC);

  sceNetInit(128*1024, 42, 4*1024, 42, 4*1024);

  sceNetAdhocInit();

  struct productStruct gameProduct;

  gameProduct.unknown = 2;

  memcpy(gameProduct.product, "000000001", 9);

  sceNetAdhocctlInit(32*1024, 0x20, &gameProduct);
}

void gsTerm(void){
  sceNetAdhocctlDisconnect();

  sceNetAdhocctlTerm();

  sceNetAdhocTerm();

  sceNetTerm();

  sceUtilityUnloadNetModule(PSP_NET_MODULE_ADHOC);

  sceUtilityUnloadNetModule(PSP_NET_MODULE_COMMON);
}

void startGameSharing(){
  gsInit();

  gsInitDialog();

  gsDialog();

  gsTerm();
}
