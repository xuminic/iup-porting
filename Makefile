ifeq ($(OS), Windows_NT)
  WINLIBS = iupole iupfiledlg
  export MINGW4=/c/mingw
  export TEC_UNAME=mingw4
endif

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
ifndef  USE_GTK2
  gtkver := $(shell pkg-config gtk+-3.0 --cflags)
  ifeq ($(gtkver), )
    export USE_GTK2=1
  else
    export USE_GTK3=1
  endif
endif

TARGETS := iup iupimglib $(WINLIBS) 

.PHONY: all $(TARGETS) testing
all: $(TARGETS)
	make -C ./iup $(TARGETS)
	if [ ! -d lib ]; then mkdir lib; fi
	cp ./iup/lib/*/* lib

testing:
	make -C ./testing

clean:
	rm -rf ./iup/lib ./iup/obj ./iup/src/dep ./lib
	rm -f ./iup/srcimglib/iupimglib.dep

