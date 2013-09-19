TARGET = battleship
OBJS = main.o menu.o draw.o adhoc.o base.o game.o
OBJS += image/background.o image/ship2_v.o image/ship2_h.o image/ship3_v.o image/ship3_h.o image/ship4_v.o image/ship4_h.o image/ship5_v.o image/ship5_h.o
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

image/ship2_v.o: image/ship2_v.raw
	bin2o -i image/ship2_v.raw image/ship2_v.o ship2_v
image/ship2_h.o: image/ship2_h.raw
	bin2o -i image/ship2_h.raw image/ship2_h.o ship2_h
image/ship3_v.o: image/ship2_v.raw
	bin2o -i image/ship3_v.raw image/ship3_v.o ship3_v
image/ship3_h.o: image/ship3_h.raw
	bin2o -i image/ship3_h.raw image/ship3_h.o ship3_h
image/ship4_v.o: image/ship3_v.raw
	bin2o -i image/ship4_v.raw image/ship4_v.o ship4_v
image/ship4_h.o: image/ship4_h.raw
	bin2o -i image/ship4_h.raw image/ship4_h.o ship4_h
image/ship5_v.o: image/ship4_v.raw
	bin2o -i image/ship5_v.raw image/ship5_v.o ship5_v
image/ship5_h.o: image/ship5_h.raw
	bin2o -i image/ship5_h.raw image/ship5_h.o ship5_h
