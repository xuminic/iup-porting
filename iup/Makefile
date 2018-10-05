ifeq ($(OS), Windows_NT)
  WINLIBS = iupole
  export MINGW4=/c/mingw
  export TEC_UNAME=mingw4
endif

# quick decide which GTK is available
ifndef  USE_GTK2
  gtkver := $(shell pkg-config gtk+-3.0 --cflags)
  ifeq ($(gtkver), )
    export USE_GTK2=1
  endif
endif

.PHONY: do_all iup iupgtk iupmot iupcd iupcontrols iupgl iup_pplot iup_mglplot iup_scintilla iupim iupimglib ledc iupview iuplua3 iuplua5 iupconsole iupole iupweb iuptuio
do_all: iup iupimglib $(WINLIBS)

iup iupgtk iupmot:
	@$(MAKE) --no-print-directory -C ./src/ $@
iupcd:
	@$(MAKE) --no-print-directory -C ./srccd/
iupcontrols:
	@$(MAKE) --no-print-directory -C ./srccontrols/
iup_pplot:
	@$(MAKE) --no-print-directory -C ./srcpplot/
iup_mglplot:
	@$(MAKE) --no-print-directory -C ./srcmglplot/
iup_scintilla:
	@$(MAKE) --no-print-directory -C ./srcscintilla/
iupgl:
	@$(MAKE) --no-print-directory -C ./srcgl/
iupim:
	@$(MAKE) --no-print-directory -C ./srcim/
iupole:
	@$(MAKE) --no-print-directory -C ./srcole/
iupweb:
	@$(MAKE) --no-print-directory -C ./srcweb/
iuptuio:
	@$(MAKE) --no-print-directory -C ./srctuio/
iupimglib:
	@$(MAKE) USE_IUP_IMGLIB_LARGE=1 --no-print-directory -C ./srcimglib/
iuplua3:
	@$(MAKE) --no-print-directory -C ./srclua3/
iuplua5:
	@$(MAKE) --no-print-directory -C ./srclua5/
iupconsole:
	@$(MAKE) --no-print-directory -C ./srcconsole/
ledc:
	@$(MAKE) --no-print-directory -C ./srcledc/
iupview: iupcontrols iup
	@$(MAKE) --no-print-directory -C ./srcview/

clean:
	rm -rf ./lib/* ./obj/* 
	
cleanall: clean
	rm -rf src/iup.dep srcimglib/iupimglib.dep

