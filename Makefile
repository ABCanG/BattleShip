TARGET = battleship
OBJS = main.o menu.o draw.o adhoc.o base.o game.o
OBJS += image/background.o
OBJS += intraFont/intraFont.o intraFont/libccc.o

ifeq ($(SIGN),1)
OPTION_FLAGS = -DSIGN
endif

INCDIR =
CFLAGS = -O2 -G0 -Wall -g $(OPTION_FLAGS)
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1

LIBDIR =
LDFLAGS =
LIBS = -lpspgum -lpspgu -lm -lpspge -lpspdisplay -lc -lpspuser
LIBS += -lpspwlan -lpspnet_adhoc -lpspnet_adhocctl -lpspnet_adhocmatching -lpsputility

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = BattleShip

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak


image/background.o: image/background.raw
	bin2o -i image/background.raw image/background.o background
