TARGET = Minesweeper
OBJS = main.o
YOURLIBS=

INCDIR =
CFLAGS = -G0 -Wall -O2
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
STDLIBS= -losl -lpng -lz \
		-lpspsdk -lpspctrl -lpspumd -lpsprtc -lpsppower -lpspgu -lpspaudiolib -lpspaudio -lm -lstdc++
LIBS=$(STDLIBS)$(YOURLIBS)

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Minesweeper
PSP_EBOOT_ICON = ICON0.png
#PSP_EBOOT_PIC1 = PIC1.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
