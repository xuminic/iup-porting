
# quick decision which GTK is available
ifndef  USE_GTK2
gtkver := $(shell pkg-config gtk+-3.0 --cflags)
ifeq ($(gtkver), )
  export USE_GTK2=1
endif
endif


CC	= gcc
AR	= ar
CP	= cp
RM	= rm -f

IUPDIR	= ../iup

ifdef	USE_GTK2
INCS	= -I. -I$(IUPDIR)/include `pkg-config gtk+-2.0 --cflags`
LIBS	= -L../lib -liup `pkg-config gtk+-2.0 --libs`
else
INCS	= -I. -I$(IUPDIR)/include `pkg-config gtk+-3.0 --cflags`
LIBS	= -L../lib -liup `pkg-config gtk+-3.0 --libs`
endif

CFLAGS	= -Wall -O3

TARGET	= iup_thread_sync_idler2  iup_thread_sync_idler  iup_thread_sync_timer \
	  splitbox list

all: $(TARGET)

% : %.c
	$(CC) $(CFLAGS) $(INCS) -o $@ $< $(LIBS) -lm -lX11

clean:
	$(RM) $(TARGET)



