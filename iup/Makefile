
# quick decision which GTK is available
ifndef  USE_GTK
  gtkver := $(shell pkg-config gtk+-3.0 --cflags)
  ifeq ($(gtkver), )
    export USE_GTK=1
  endif
endif

.PHONY: do_all iup iupgtk iupcd iupcontrols iupgl iup_pplot iupim iupimglib ledc iupview iuplua3 iuplua5 iupconsole
#do_all: iup iupgtk iupcd iupcontrols iupgl iup_pplot iupim iupimglib ledc iupview iuplua3 iuplua5 iupconsole
do_all: iup iupgtk iupimglib 

iup iupgtk:
	@$(MAKE) --no-print-directory -C ./src/ $@
iupcd:
	@$(MAKE) --no-print-directory -C ./srccd/
iupcontrols:
	@$(MAKE) --no-print-directory -C ./srccontrols/
iup_pplot:
	@$(MAKE) --no-print-directory -C ./srcpplot/
iupgl:
	@$(MAKE) --no-print-directory -C ./srcgl/
iupim:
	@$(MAKE) --no-print-directory -C ./srcim/
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
