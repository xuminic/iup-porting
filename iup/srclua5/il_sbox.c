/******************************************************************************
 * Automatically generated file (iuplua5). Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Sbox(lua_State *L)
{
  Ihandle *ih = IupSbox(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupsboxlua_open(lua_State * L)
{
  iuplua_register(L, Sbox, "Sbox");


#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/sbox_be64.loh"
#else
#include "loh/sbox_be32.loh"
#endif
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/sbox_le64w.loh"
#else
#include "loh/sbox_le64.loh"
#endif
#else
#include "loh/sbox.loh"
#endif
#endif
#else
  iuplua_dofile(L, "sbox.lua");
#endif

  return 0;
}

