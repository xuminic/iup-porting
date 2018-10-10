ifeq ($(OS), Windows_NT)
  WINLIBS = iupole
  export MINGW4=/c/mingw
  export TEC_UNAME=mingw4
endif

# quick decision of which GTK is available
ifndef  USE_GTK2
  gtkver := $(shell pkg-config gtk+-2.0 --cflags)
  ifneq ($(gtkver), )
    export USE_GTK2=1
  endif
endif

.PHONY: do_all iup iupgtk iupmot iupcd iupcontrols iupgl iupglcontrols iup_plot iup_mglplot iup_scintilla iupim iupimglib ledc iupview iuplua5 iupluaconsole iupluascripter iupole iupweb iuptuio
do_all: iup iupimglib $(WINLIBS)

iup iupgtk iupmot:
	@$(MAKE) --no-print-directory -C ./src/ $@
iupcd:
	@$(MAKE) --no-print-directory -C ./srccd/
iupcontrols:
	@$(MAKE) --no-print-directory -C ./srccontrols/
iup_plot:
	@$(MAKE) --no-print-directory -C ./srcplot/
iup_mglplot:
	@$(MAKE) --no-print-directory -C ./srcmglplot/
iup_scintilla:
	@$(MAKE) --no-print-directory -C ./srcscintilla/
iupgl:
	@$(MAKE) --no-print-directory -C ./srcgl/
iupglcontrols:
	@$(MAKE) --no-print-directory -C ./srcglcontrols/
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
iuplua5:
	@$(MAKE) --no-print-directory -C ./srclua5/
iupluaconsole:
	@$(MAKE) --no-print-directory -C ./srcluaconsole/
iupluascripter:
	@$(MAKE) --no-print-directory -C ./srcluascripter/
ledc:
	@$(MAKE) --no-print-directory -C ./srcledc/
iupview: iupcontrols iup
	@$(MAKE) --no-print-directory -C ./srcview/

clean:
	rm -rf ./lib/* ./obj/* VERSION.gtk

cleanall: clean
	rm -rf src/iup.dep srcimglib/iupimglib.dep

