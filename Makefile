# Quick decision which GTK is available
# By the way please add "STDINCS += $(GTK)/include/harfbuzz" in tecmake.mak:1400
#    ifdef LINK_CAIRO
#      LIBS += pangocairo-1.0 cairo
#    endif
#    ...
#    STDINCS += $(GTK)/include/atk-1.0 $(GTK)/include/gtk-$(GTKSFX).0 $(GTK)/include/gdk-pixbuf-2.0
#    STDINCS += $(GTK)/include/cairo $(GTK)/include/pango-1.0 $(GTK)/include/glib-2.0
#    STDINCS += /usr/include/harfbuzz
#
ifeq ($(OS), Windows_NT)
  UNAME=$(shell uname -s|cut -f1 -d-)
  ifeq ($(MSYSTEM),MINGW64)
    export TEC_UNAME=mingw6
  else ifeq ($(MSYSTEM),MINGW32)
    export TEC_UNAME=mingw4
  else ifeq ($(MSYSTEM),UCRT64)
    export TEC_UNAME=mingw6_64		# actually not mingw6_64, but to reuse the type
  else ifeq ($(MSYSTEM),MSYS)
    export TEC_UNAME=mingw4_64
  else ifeq ($(UNAME),CYGWIN_NT)
    export TEC_UNAME=mingw4_64
  else
    $(error TEC_UNAME NOT defined)
  endif
  WINLIBS = iupole
  #WINLIBS = iupole iupfiledlg
  #export MINGW4=/c/mingw
  #export TEC_UNAME=mingw4
else
  ifndef  USE_GTK2
    gtkver := $(shell pkg-config gtk+-3.0 --cflags)
    ifeq ($(gtkver), )
      export USE_GTK2=1
    else
      export USE_GTK3=1
    endif
  endif
endif

TARGETS := iup iupimglib $(WINLIBS) 

.PHONY: all $(TARGETS) testing
all: $(TARGETS)
	make -C ./iup $(TARGETS)
	if [ ! -d lib ]; then mkdir lib; fi
	cp ./iup/lib/*/*.a lib

testing:
	make -C ./testing

clean:
	rm -rf ./iup/lib ./iup/obj ./iup/src/dep ./lib
	rm -f ./iup/srcimglib/iupimglib.dep
	rm -f ./iup/src/iup.wdep ./iup/srcimglib/iupimglib.wdep ./iup/srcole/iupole.wdep

