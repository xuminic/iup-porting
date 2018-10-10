
CC	= gcc
AR	= ar
CP	= cp
RM	= rm -f

IUPDIR	= ../iup

INCS	= -I. -I$(IUPDIR)/include `pkg-config gtk+-2.0 --cflags`
LIBS	= -L$(IUPDIR)/lib -liup `pkg-config gtk+-2.0 --libs`
CFLAGS	= -Wall -O3

TARGET	= iup_thread_sync_idler2  iup_thread_sync_idler  iup_thread_sync_timer \
	  splitbox

all: $(TARGET)

% : %.c
	$(CC) $(CFLAGS) $(INCS) -o $@ $< $(LIBS) -lm -lX11

clean:
	$(RM) $(TARGET)



