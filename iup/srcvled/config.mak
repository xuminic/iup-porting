PROJNAME = iup
APPNAME := iupvled
OPT = YES

SRC = iup_vled.c

IUP := ..

LINKER = $(CPPC)

USE_IUPCONTROLS = Yes
USE_IUP3 = Yes

ifeq "$(TEC_SYSNAME)" "Haiku"
  USE_HAIKU = Yes
else
  USE_STATIC = Yes
ifdef GTK_DEFAULT
  ifdef USE_MOTIF
    # Build Motif version in Linux and BSD
    APPNAME := $(APPNAME)mot
  endif
else  
  ifdef USE_GTK
    # Build GTK version in IRIX,SunOS,AIX,Win32
    APPNAME := $(APPNAME)gtk
  endif
endif
endif

USE_IM = Yes
ifdef USE_IM
  DEFINES += USE_IM  
  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    LIBS += iupim
  else
    ifdef USE_STATIC
      ifdef DBG_DIR
        IUPLIB = $(IUP)/lib/$(TEC_UNAME)d
      else
        IUPLIB = $(IUP)/lib/$(TEC_UNAME)
      endif  
      SLIB += $(IUPLIB)/libiupim.a
    else
      LIBS += iupim
    endif             
  endif             
endif 

ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  LIBS += iupimglib iup_scintilla imm32
else
  ifdef USE_STATIC
    ifdef DBG_DIR
      IUPLIB = $(IUP)/lib/$(TEC_UNAME)d
    else
      IUPLIB = $(IUP)/lib/$(TEC_UNAME)
    endif  
    SLIB += $(IUPLIB)/libiupimglib.a $(IUPLIB)/libiup_scintilla.a
  else
    LIBS += iupimglib iup_scintilla
    LIBS += atk-1.0
  endif             
endif

#USE_NO_OPENGL=Yes
ifndef USE_NO_OPENGL
  USE_OPENGL = Yes
  USE_FTGL = Yes
  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    LIBS += iupglcontrols
  else
    ifdef USE_STATIC
      ifdef DBG_DIR
        IUPLIB = $(IUP)/lib/$(TEC_UNAME)d
      else
        IUPLIB = $(IUP)/lib/$(TEC_UNAME)
      endif  
      SLIB += $(IUP_LIB)/libiupglcontrols.a 
    else
      LIBS += iupglcontrols
    endif             
  endif
else
  DEFINES += USE_NO_OPENGL
endif

#USE_NO_WEB=Yes
ifndef USE_NO_WEB
  USE_IUPWEB = Yes
else
  DEFINES += USE_NO_WEB
endif

#USE_NO_PLOT=Yes
ifndef USE_NO_PLOT
  LINKER = g++
  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    LIBS += iup_plot cdcontextplus cdgl gdiplus
    LDIR += $(CD)/lib/$(TEC_UNAME)
  else
    ifdef USE_STATIC
      ifdef DBG_DIR
        IUPLIB = $(IUP)/lib/$(TEC_UNAME)d
      else
        IUPLIB = $(IUP)/lib/$(TEC_UNAME)
      endif  
      SLIB += $(IUP_LIB)/libiup_plot.a
      SLIB += $(CD_LIB)/libcdgl.a
      SLIB += $(CD_LIB)/libcdcontextplus.a
    else
      LIBS += iupglcontrols
      LIBS += cdgl
    endif             
  endif
else
  DEFINES += USE_NO_PLOT
endif

ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  SRC += ../etc/iup.rc
endif

ifneq ($(findstring cygw, $(TEC_UNAME)), )
  LIBS += fontconfig
endif

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  LIBS += fontconfig
endif

INCLUDES = ../src
