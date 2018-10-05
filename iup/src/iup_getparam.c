/** \file
 * \brief IupGetParam
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "iup.h"

#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_strmessage.h"
#include "iup_layout.h"
#include "iup_drvfont.h"


#define RAD2DEG  57.296   /* radians to degrees */


static void iParamSetDoublePrec(Ihandle* ih, const char* name, double num, int prec)
{
  char value[80];
  char format[30];
  if (prec <= 0) prec = IupGetInt(NULL, "DEFAULTPRECISION");
  sprintf(format, "%%.%df", prec);
  iupStrPrintfDoubleLocale(value, format, num, IupGetGlobal("DEFAULTDECIMALSYMBOL"));

  IupStoreAttribute(ih, name, value);
}

/*******************************************************************************************
                    Internal Callbacks
*******************************************************************************************/

static int iParamDlgClose_CB(Ihandle* dlg)
{
  Ihandle* param_box = (Ihandle*)iupAttribGetInherit(dlg, "PARAMBOX");
  Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");
  iupAttribSet(param_box, "STATUS", "0");
  if (cb && !cb(param_box, IUP_GETPARAM_CLOSE, (void*)iupAttribGet(param_box, "USERDATA")))
    return IUP_IGNORE;
  else
    return IUP_CLOSE;
}

static int iParamButton1_CB(Ihandle* self)
{
  Ihandle* param_box = (Ihandle*)iupAttribGetInherit(self, "PARAMBOX");
  Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");
  iupAttribSet(param_box, "STATUS", "1");
  if (cb && !cb(param_box, IUP_GETPARAM_BUTTON1, (void*)iupAttribGet(param_box, "USERDATA")))
    return IUP_DEFAULT;
  else
    return IUP_CLOSE;
}

static int iParamButton2_CB(Ihandle* self)
{
  Ihandle* param_box = (Ihandle*)iupAttribGetInherit(self, "PARAMBOX");
  Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");
  iupAttribSet(param_box, "STATUS", "0");
  if (cb && !cb(param_box, IUP_GETPARAM_BUTTON2, (void*)iupAttribGet(param_box, "USERDATA")))
    return IUP_DEFAULT;
  else
    return IUP_CLOSE;
}

static int iParamButton3_CB(Ihandle* self)
{
  Ihandle* param_box = (Ihandle*)iupAttribGetInherit(self, "PARAMBOX");
  Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");
  if (cb) 
    cb(param_box, IUP_GETPARAM_BUTTON3, (void*)iupAttribGet(param_box, "USERDATA"));
  return IUP_DEFAULT;
}


/***********************************************************************/


static int iParamToggleAction_CB(Ihandle *self, int v)
{
  Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "PARAM");
  Ihandle* param_box = (Ihandle*)iupAttribGetInherit(self, "PARAMBOX");
  Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");
  int old_v = iupAttribGetInt(param, "VALUE");

  if (v == 1)
    iupAttribSet(param, "VALUE", "1");
  else
    iupAttribSet(param, "VALUE", "0");

  if (cb && !cb(param_box, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(param_box, "USERDATA"))) 
  {
    /* Undo */
    if (old_v == 1)
    {
      iupAttribSet(param, "VALUE", "1");
      IupSetAttribute(self, "VALUE", "1");
    }
    else
    {
      iupAttribSet(param, "VALUE", "0");
      IupSetAttribute(self, "VALUE", "0");
    }

    /* there is no IUP_IGNORE for IupToggle */
    return IUP_DEFAULT;
  }

  /* update the interface */
  if (v == 1)
    IupStoreAttribute(self, "TITLE", iupAttribGet(param, "TRUE"));
  else
    IupStoreAttribute(self, "TITLE", iupAttribGet(param, "FALSE"));

  return IUP_DEFAULT;
}

static int iParamTextAction_CB(Ihandle *self, int c, char *after)
{
  Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "PARAM");
  Ihandle* param_box = (Ihandle*)iupAttribGetInherit(self, "PARAMBOX");
  Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");
  Ihandle* aux = (Ihandle*)iupAttribGet(param, "AUXCONTROL");
  (void)c;
 
  if (iupStrEqual(iupAttribGet(param, "TYPE"), "REAL"))
  {
    double val = 0;
    if (iupStrToDoubleLocale(after, &val, IupGetGlobal("DEFAULTDECIMALSYMBOL")))
      iupAttribSetDouble(param, "VALUE", val);
    else
      iupAttribSetStr(param, "VALUE", after);
  }
  else
    iupAttribSetStr(param, "VALUE", after);

  if (cb && !cb(param_box, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(param_box, "USERDATA"))) 
  {
    /* Undo */
    iupAttribSetStr(param, "VALUE", IupGetAttribute(self, "VALUE"));
    return IUP_IGNORE;
  }

  if (aux)
  {
    if (iupStrEqual(iupAttribGet(param, "TYPE"), "COLOR"))
      IupStoreAttribute(aux, "BGCOLOR", after);
    else
      IupStoreAttribute(aux, "VALUE", after);
  }

  if (IupGetInt(self, "SPIN"))
  {
    if (iupAttribGet(self, "_IUPGP_SPINREAL"))
    {
      double min = iupAttribGetDouble(param, "MIN");
      double step = iupAttribGetDouble(self, "_IUPGP_INCSTEP");
      double val = iupAttribGetDouble(param, "VALUE");
      IupSetInt(self, "SPINVALUE", (int)((val-min)/step + 0.5));
    }
    else
    {
      int val;
      if (iupStrToInt(after, &val))
        IupSetInt(self, "SPINVALUE", val);
    }
  }

  return IUP_DEFAULT;
}

static int iParamValAction_CB(Ihandle *self)
{
  Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "PARAM");
  Ihandle* ctrl = (Ihandle*)iupAttribGetInherit(self, "CONTROL");
  Ihandle* param_box = (Ihandle*)iupAttribGetInherit(self, "PARAMBOX");
  Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");
  double old_value = iupAttribGetDouble(param, "VALUE");
  double val = IupGetDouble(self, "VALUE");

  char* type = iupAttribGet(param, "TYPE");
  if (iupStrEqual(type, "INTEGER"))
  {
    iupAttribSetInt(param, "VALUE", (int)val);
  }
  else
  {
    if (iupAttribGetInt(param, "ANGLE"))
    {
      double old_angle;

      if (val == 0)
      {
        old_angle = iupAttribGetDouble(param, "VALUE");
        iupAttribSetDouble(param, "_IUPGP_OLD_ANGLE", old_angle);
      }
      else
        old_angle = iupAttribGetDouble(param, "_IUPGP_OLD_ANGLE");

      val = old_angle + val*RAD2DEG;

      if (iupAttribGetInt(param, "INTERVAL"))
      {
        double min = iupAttribGetDouble(param, "MIN");
        double max = iupAttribGetDouble(param, "MAX");
        if (val < min)
          val = min;
        if (val > max)
          val = max;
      }
      else if (iupAttribGetInt(param, "PARTIAL"))
      {
        double min = iupAttribGetDouble(param, "MIN");
        if (val < min)
          val = min;
      }
    }

    iupAttribSetDouble(param, "VALUE", val);
  }

  if (cb && !cb(param_box, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(param_box, "USERDATA"))) 
  {
    /* Undo */
    iupAttribSetDouble(param, "VALUE", old_value);

    if (!iupAttribGetInt(param, "ANGLE"))
      IupSetDouble(self, "VALUE", old_value);

    /* there is no IUP_IGNORE for IupVal */
    return IUP_DEFAULT;
  }

  type = iupAttribGet(param, "TYPE");
  if (iupStrEqual(type, "INTEGER"))
    IupSetInt(ctrl, "VALUE", (int)val);
  else
  {
    int prec = IupGetInt(param, "PRECISION");
    iParamSetDoublePrec(ctrl, "VALUE", val, prec);
  }

  if (IupGetInt(ctrl, "SPIN"))
  {
    if (iupAttribGet(ctrl, "_IUPGP_SPINREAL"))
    {
      double min = iupAttribGetDouble(param, "MIN");
      double step = iupAttribGetDouble(ctrl, "_IUPGP_INCSTEP");
      double val = iupAttribGetDouble(param, "VALUE");
      IupSetInt(ctrl, "SPINVALUE", (int)((val - min) / step + 0.5));
    }
    else
    {
      int val = IupGetInt(ctrl, "VALUE");
      IupSetInt(ctrl, "SPINVALUE", val);
    }
  }

  return IUP_DEFAULT;
}

static int iParamListAction_CB(Ihandle *self, char *t, int i, int v)
{
  (void)t;
  if (v == 1)
  {
    Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "PARAM");
    Ihandle* param_box = (Ihandle*)iupAttribGetInherit(self, "PARAMBOX");
    Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");
    int old_i = iupAttribGetInt(param, "VALUE");

    iupAttribSetInt(param, "VALUE", i-1);

    if (cb && !cb(param_box, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(param_box, "USERDATA"))) 
    {
      /* Undo */
      iupAttribSetInt(param, "VALUE", old_i);
      IupSetInt(self, "VALUE", old_i+1);

      /* there is no IUP_IGNORE for IupList */
      return IUP_DEFAULT;
    }
  }

  return IUP_DEFAULT;
}

static int iParamOptionsAction_CB(Ihandle *self, int v)
{
  if (v == 1)
  {
    Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "PARAM");
    Ihandle* param_box = (Ihandle*)iupAttribGetInherit(self, "PARAMBOX");
    Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");
    int old_v = iupAttribGetInt(param, "VALUE");
    int new_v = iupAttribGetInt(self, "OPT");

    iupAttribSetInt(param, "VALUE", new_v);

    if (cb && !cb(param_box, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(param_box, "USERDATA"))) 
    {
      /* Undo */
      iupAttribSetInt(param, "VALUE", old_v);
      IupSetAttribute(IupGetParent(IupGetParent(self)), "VALUE_HANDLE", (char*)IupGetChild(IupGetParent(self), old_v));
    }
  }

  return IUP_DEFAULT;
}

static int iParamFileButton_CB(Ihandle *self)
{
  Ihandle* param   = (Ihandle*)iupAttribGetInherit(self, "PARAM");
  Ihandle* ctrl = (Ihandle*)iupAttribGetInherit(self, "CONTROL");
  Ihandle* param_box = (Ihandle*)iupAttribGetInherit(self, "PARAMBOX");
  Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");

  Ihandle* file_dlg = IupFileDlg();

  IupSetAttributeHandle(file_dlg, "PARENTDIALOG", IupGetDialog(self));
  IupSetStrAttribute(file_dlg, "TITLE", iupAttribGet(param, "TITLE"));
  IupSetAttribute(file_dlg, "VALUE", iupAttribGet(param, "VALUE"));

  IupSetAttribute(file_dlg, "DIALOGTYPE", iupAttribGet(param, "DIALOGTYPE"));
  IupSetAttribute(file_dlg, "FILTER", iupAttribGet(param, "FILTER"));
  IupSetAttribute(file_dlg, "DIRECTORY", iupAttribGet(param, "DIRECTORY"));
  IupSetAttribute(file_dlg, "NOCHANGEDIR", iupAttribGet(param, "NOCHANGEDIR"));
  IupSetAttribute(file_dlg, "NOOVERWRITEPROMPT", iupAttribGet(param, "NOOVERWRITEPROMPT"));

  IupPopup(file_dlg, IUP_CENTER, IUP_CENTER);

  if (IupGetInt(file_dlg, "STATUS") != -1)
  {
    char* value = IupGetAttribute(file_dlg, "VALUE");
    iupAttribSetStr(param, "OLD_VALUE", iupAttribGet(param, "VALUE"));
    iupAttribSetStr(param, "VALUE", value);

    if (cb && !cb(param_box, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(param_box, "USERDATA")))
    {
      /* Undo */
      iupAttribSetStr(param, "VALUE", iupAttribGet(param, "OLD_VALUE"));
    }
    else
      IupSetAttribute(ctrl, "VALUE", iupAttribGet(param, "VALUE"));
  }

  IupDestroy(file_dlg);

  return IUP_DEFAULT;
}

static int iParamColorButton_CB(Ihandle *self)
{
  Ihandle* param   = (Ihandle*)iupAttribGetInherit(self, "PARAM");
  Ihandle* ctrl = (Ihandle*)iupAttribGetInherit(self, "CONTROL");
  Ihandle* param_box = (Ihandle*)iupAttribGetInherit(self, "PARAMBOX");
  Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");

  Ihandle* color_dlg = IupColorDlg();
  IupSetAttributeHandle(color_dlg, "PARENTDIALOG", IupGetDialog(self));
  IupSetStrAttribute(color_dlg, "TITLE", iupAttribGet(param, "TITLE"));
  IupSetAttribute(color_dlg, "VALUE", iupAttribGet(param, "VALUE"));

  IupPopup(color_dlg, IUP_CENTER, IUP_CENTER);

  if (IupGetInt(color_dlg, "STATUS") == 1)
  {
    char* value = IupGetAttribute(color_dlg, "VALUE");
    iupAttribSetStr(param, "OLD_VALUE", iupAttribGet(param, "VALUE"));
    iupAttribSetStr(param, "VALUE", value);

    if (cb && !cb(param_box, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(param_box, "USERDATA")))
    {
      /* Undo */
      iupAttribSetStr(param, "VALUE", iupAttribGet(param, "OLD_VALUE"));
    }
    else
    {
      IupSetAttribute(ctrl, "VALUE", iupAttribGet(param, "VALUE"));
      IupStoreAttribute(self, "BGCOLOR", iupAttribGet(param, "VALUE"));
    }
  }

  IupDestroy(color_dlg);

  return IUP_DEFAULT;
}

static int iParamFontButton_CB(Ihandle *self)
{
  Ihandle* param   = (Ihandle*)iupAttribGetInherit(self, "PARAM");
  Ihandle* ctrl = (Ihandle*)iupAttribGetInherit(self, "CONTROL");
  Ihandle* param_box = (Ihandle*)iupAttribGetInherit(self, "PARAMBOX");
  Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");

  Ihandle* font_dlg = IupFontDlg();
  IupSetAttributeHandle(font_dlg, "PARENTDIALOG", IupGetDialog(self));
  IupSetStrAttribute(font_dlg, "TITLE", iupAttribGet(param, "TITLE"));
  IupSetAttribute(font_dlg, "VALUE", iupAttribGet(param, "VALUE"));

  IupPopup(font_dlg, IUP_CENTER, IUP_CENTER);

  if (IupGetInt(font_dlg, "STATUS") == 1)
  {
    char* value = IupGetAttribute(font_dlg, "VALUE");
    iupAttribSetStr(param, "OLD_VALUE", iupAttribGet(param, "VALUE"));
    iupAttribSetStr(param, "VALUE", value);

    if (cb && !cb(param_box, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(param_box, "USERDATA")))
    {
      /* Undo */
      iupAttribSetStr(param, "VALUE", iupAttribGet(param, "OLD_VALUE"));
    }
    else
      IupSetAttribute(ctrl, "VALUE", iupAttribGet(param, "VALUE"));

    iupAttribSet(param, "OLD_VALUE", NULL);
  }

  IupDestroy(font_dlg);

  return IUP_DEFAULT;
}

static int iParamSpinReal_CB(Ihandle *self, int pos)
{
  Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "PARAM");
  Ihandle* param_box = (Ihandle*)iupAttribGetInherit(self, "PARAMBOX");
  Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");
  Ihandle* ctrl = (Ihandle*)iupAttribGet(param, "CONTROL");
  double min = iupAttribGetDouble(param, "MIN");
  double max = iupAttribGetDouble(param, "MAX");
  double step = iupAttribGetDouble(ctrl, "_IUPGP_INCSTEP");
  double old_value = iupAttribGetDouble(param, "VALUE");

  /* here spin is always [0-spinmax] converted to [min-max] */
  double val = (double)pos*step + min;
  if (val < min)
    val = min;
  if (val > max)
    val = max;

  iupAttribSetDouble(param, "VALUE", val);

  if (cb) 
  {
    int ret;
    iupAttribSet(param_box, "SPINNING", "1");
    ret = cb(param_box, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(param_box, "USERDATA"));
    iupAttribSet(param_box, "SPINNING", NULL);
    if (!ret)
    {
      /* Undo */
      iupAttribSetDouble(param, "VALUE", old_value);
      return IUP_IGNORE;
    }
  }

  {
    int prec = IupGetInt(param, "PRECISION");
    iParamSetDoublePrec(ctrl, "VALUE", val, prec);
  }

  {
    Ihandle* aux = (Ihandle*)iupAttribGet(param, "AUXCONTROL");
    if (aux)
      IupSetDouble(aux, "VALUE", val);
  }

  return IUP_DEFAULT;
}

static int iParamSpinInt_CB(Ihandle *self, int pos)
{
  Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "PARAM");
  Ihandle* param_box = (Ihandle*)iupAttribGetInherit(self, "PARAMBOX");
  Iparamcb cb = (Iparamcb)IupGetCallback(param_box, "PARAM_CB");
  Ihandle* ctrl = (Ihandle*)iupAttribGet(param, "CONTROL");
  int old_value = iupAttribGetInt(param, "VALUE");

  /* here spin is always [min-max] */

  iupAttribSetInt(param, "VALUE", pos);

  if (cb) 
  {
    int ret;
    iupAttribSet(param_box, "SPINNING", "1");
    ret = cb(param_box, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(param_box, "USERDATA"));
    iupAttribSet(param_box, "SPINNING", NULL);
    if (!ret)
    {
      /* Undo */
      iupAttribSetInt(param, "VALUE", old_value);
      return IUP_IGNORE;
    }
  }

  IupSetInt(ctrl, "VALUE", pos);

  {
    Ihandle* aux = (Ihandle*)iupAttribGet(param, "AUXCONTROL");
    if (aux)
      IupSetInt(aux, "VALUE", pos);
  }

  return IUP_DEFAULT;
}

/*******************************************************************************************
                    Creates a Parameter Control Box
*******************************************************************************************/

static int iParamDestroy_CB(Ihandle* self)
{
  Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "PARAM");
  IupDestroy(param);
  iupAttribSet(self, "PARAM", NULL);
  return IUP_DEFAULT;
}

static Ihandle* iParamCreateCtrlBox(Ihandle* param, const char *type)
{
  Ihandle *box, *ctrl = NULL, *label;

  label = IupLabel(iupAttribGet(param, "TITLE"));
  IupSetCallback(label, "DESTROY_CB", iParamDestroy_CB);

  if (iupStrEqual(type, "SEPARATOR"))
  {
    box = IupHbox(label, NULL);
    IupSetAttribute(box,"ALIGNMENT","ACENTER");

    IupSetAttribute(label, "FONTSTYLE", "Bold");
  }
  else
  {
    if (iupStrEqual(type, "STRING") && iupAttribGetInt(param, "MULTILINE"))
    {
      Ihandle* hbox1 = IupHbox(IupSetAttributes(IupFill(), "SIZE=5"), label, NULL);
      IupSetAttribute(hbox1,"ALIGNMENT","ACENTER");

      box = IupVbox(hbox1, NULL);
      IupSetAttribute(box,"ALIGNMENT","ALEFT");
    }
    else
    {
      Ihandle* fill = IupFill();
      int indent = iupAttribGetInt(param, "INDENT");
      IupSetInt(fill, "SIZE", 5 + 8*indent);
      box = IupHbox(fill, label, NULL);
      IupSetAttribute(box,"ALIGNMENT","ACENTER");
    }
  }

  IupSetAttribute(box,"MARGIN","0x0");

  type = iupAttribGet(param, "TYPE");
  if (iupStrEqual(type, "BOOLEAN"))
  {
    int value = iupAttribGetInt(param, "VALUE");
    if (value)
    {
      ctrl = IupToggle(iupAttribGet(param, "TRUE"), NULL);
      IupSetAttribute(ctrl, "VALUE", "ON");
    }
    else
    {
      ctrl = IupToggle(iupAttribGet(param, "FALSE"), NULL);
      IupSetAttribute(ctrl, "VALUE", "OFF");
    }
    IupSetCallback(ctrl, "ACTION", (Icallback)iParamToggleAction_CB);
    IupSetAttribute(ctrl, "EXPAND", "HORIZONTAL");

    IupAppend(box, ctrl);
  }
  else if (iupStrEqual(type, "SEPARATOR"))
  {
    ctrl = IupLabel("");
    IupSetAttribute(ctrl, "SEPARATOR", "HORIZONTAL");
    IupSetAttribute(ctrl, "EXPAND", "HORIZONTAL");

    IupAppend(box, ctrl);
  }
  else if (iupStrEqual(type, "LIST"))
  {
    int i = 0;
    ctrl = IupList(NULL);
    IupSetCallback(ctrl, "ACTION", (Icallback)iParamListAction_CB);
    IupSetAttribute(ctrl, "DROPDOWN", "YES");

    while (*iupAttribGetId(param, "", i) != 0)
    {
      IupStoreAttributeId(ctrl, "", i+1, iupAttribGetId(param, "", i));
      i++;
    }
    IupStoreAttributeId(ctrl, "", i+1, NULL);

    IupSetInt(ctrl, "VALUE", iupAttribGetInt(param, "VALUE") + 1);

    IupAppend(box, ctrl);
  }
  else if (iupStrEqual(type, "OPTIONS"))
  {
    Ihandle* tgl;
    int i = 0;
    ctrl = IupHbox(NULL);
    IupSetAttribute(ctrl, "GAP", "5");

    while (*iupAttribGetId(param, "", i) != 0)
    {
      tgl = IupToggle(iupAttribGetId(param, "", i), NULL);
      IupSetInt(tgl, "OPT", i);
      IupSetCallback(tgl, "ACTION", (Icallback)iParamOptionsAction_CB);

      IupAppend(ctrl, tgl);

      i++;
    }

    IupAppend(box, IupRadio(ctrl));
    tgl = IupGetChild(ctrl, iupAttribGetInt(param, "VALUE"));
    /* RADIO initial value */
    IupSetAttribute(IupGetParent(ctrl), "VALUE_HANDLE", (char*)tgl);
    ctrl = tgl; /* allow TIP to go to the first toggle */
  }
  else if (iupStrEqual(type, "STRING"))
  {
    if (iupAttribGetInt(param, "MULTILINE"))
    {
      Ihandle* hbox;

      ctrl = IupMultiLine(NULL);
      IupSetAttribute(ctrl, "VISIBLECOLUMNS", "8");
      IupSetAttribute(ctrl, "VISIBLELINES", "6");
      IupSetAttribute(ctrl, "EXPAND", "YES");

      hbox = IupHbox(IupSetAttributes(IupFill(), "SIZE=5"), ctrl, NULL);
      IupSetAttribute(hbox,"ALIGNMENT","ACENTER");

      IupAppend(box, hbox);
    }
    else
    {
      char *visiblecolumns;

      ctrl = IupText(NULL);
      IupSetAttribute(ctrl, "VISIBLECOLUMNS", "8");
      IupAppend(box, ctrl);

      visiblecolumns = iupAttribGet(param, "VISIBLECOLUMNS");
      if (visiblecolumns)
        IupSetStrAttribute(ctrl, "VISIBLECOLUMNS", visiblecolumns);
      else
        IupSetAttribute(ctrl, "EXPAND", "HORIZONTAL");
    }
    IupSetCallback(ctrl, "ACTION", (Icallback)iParamTextAction_CB);
    IupStoreAttribute(ctrl, "VALUE", iupAttribGet(param, "VALUE"));

    {
      char* mask = iupAttribGet(param, "MASK");
      if (mask)
        IupStoreAttribute(ctrl, "MASK", mask);
    }

    iupAttribSet(param, "TEXTEXPAND", "1");
  }
  else if (iupStrEqual(type, "FILE"))
  {
      Ihandle* aux;

      ctrl = IupText(NULL);
      IupSetAttribute(ctrl, "VISIBLECOLUMNS", "15");
      IupSetAttribute(ctrl, "EXPAND", "HORIZONTAL");
      IupAppend(box, ctrl);

      IupSetCallback(ctrl, "ACTION", (Icallback)iParamTextAction_CB);
      IupStoreAttribute(ctrl, "VALUE", iupAttribGet(param, "VALUE"));

      iupAttribSet(param, "TEXTEXPAND", "1");
      IupSetAttribute(box,"NORMALIZESIZE","VERTICAL");
      
      aux = IupButton("...", "");
      IupStoreAttribute(aux, "FONT", "Times, Bold 10");
      IupSetAttribute(aux, "SIZE", "16x");
      IupSetCallback(aux, "ACTION", (Icallback)iParamFileButton_CB);
      iupAttribSet(aux, "PARAM", (char*)param);
      iupAttribSet(aux, "CONTROL", (char*)ctrl);
      iupAttribSet(param, "AUXCONTROL", (char*)aux);

      IupAppend(box, aux); 
  }
  else if (iupStrEqual(type, "COLOR"))
  {
      Ihandle* aux;

      ctrl = IupText(NULL);
      IupSetAttribute(ctrl, "VISIBLECOLUMNS", "15");
      IupSetAttribute(ctrl, "EXPAND", "HORIZONTAL");
      IupAppend(box, ctrl);

      IupSetCallback(ctrl, "ACTION", (Icallback)iParamTextAction_CB);
      IupSetAttribute(ctrl, "MASK", "(/d|/d/d|1/d/d|2(0|1|2|3|4)/d|25(0|1|2|3|4|5)) (/d|/d/d|1/d/d|2(0|1|2|3|4)/d|25(0|1|2|3|4|5)) (/d|/d/d|1/d/d|2(0|1|2|3|4)/d|25(0|1|2|3|4|5)) (/d|/d/d|1/d/d|2(0|1|2|3|4)/d|25(0|1|2|3|4|5))");
      IupStoreAttribute(ctrl, "VALUE", iupAttribGet(param, "VALUE"));

      iupAttribSet(param, "TEXTEXPAND", "1");
      IupSetAttribute(box,"NORMALIZESIZE","VERTICAL");

      aux = IupButton(NULL, NULL);
      IupStoreAttribute(aux, "FONT", "Times, Bold Italic 11");
      IupSetAttribute(aux, "SIZE", "16x");
      IupStoreAttribute(aux, "BGCOLOR", iupAttribGet(param, "VALUE"));
      IupSetCallback(aux, "ACTION", (Icallback)iParamColorButton_CB);
      iupAttribSet(aux, "PARAM", (char*)param);
      iupAttribSet(aux, "CONTROL", (char*)ctrl);
      iupAttribSet(param, "AUXCONTROL", (char*)aux);

      IupAppend(box, aux); 
  }
  else if (iupStrEqual(type, "FONT"))
  {
      Ihandle* aux;

      ctrl = IupText(NULL);
      IupSetAttribute(ctrl, "VISIBLECOLUMNS", "15");
      IupSetAttribute(ctrl, "EXPAND", "HORIZONTAL");
      IupAppend(box, ctrl);

      IupSetCallback(ctrl, "ACTION", (Icallback)iParamTextAction_CB);
      IupStoreAttribute(ctrl, "VALUE", iupAttribGet(param, "VALUE"));

      iupAttribSet(param, "TEXTEXPAND", "1");
      IupSetAttribute(box,"NORMALIZESIZE","VERTICAL");
      
      aux = IupButton("F", NULL);
      IupStoreAttribute(aux, "FONT", "Times, Bold Italic 11");
      IupSetAttribute(aux, "SIZE", "16x");

      IupSetCallback(aux, "ACTION", (Icallback)iParamFontButton_CB);
      iupAttribSet(aux, "PARAM", (char*)param);
      iupAttribSet(aux, "CONTROL", (char*)ctrl);
      iupAttribSet(param, "AUXCONTROL", (char*)aux);

      IupAppend(box, aux); 
  }
  else /* INTEGER, REAL */
  {
    ctrl = IupText(NULL);
    IupSetAttribute(ctrl, "VISIBLECOLUMNS", "8");
    IupSetCallback(ctrl, "ACTION", (Icallback)iParamTextAction_CB);

    type = iupAttribGet(param, "TYPE");
    if (iupStrEqual(type, "REAL"))
    {
      double val = iupAttribGetDouble(param, "VALUE");
      int prec = IupGetInt(param, "PRECISION");
      iParamSetDoublePrec(ctrl, "VALUE", val, prec);

      IupSetAttribute(ctrl, "MASKDECIMALSYMBOL", IupGetGlobal("DEFAULTDECIMALSYMBOL"));

      if (iupAttribGetInt(param, "INTERVAL"))
      {
        double min = iupAttribGetDouble(param, "MIN");
        double max = iupAttribGetDouble(param, "MAX");
        double step = iupAttribGetDouble(param, "STEP");
        double val = iupAttribGetDouble(param, "VALUE");
        if (step == 0) step = (max-min)/20.0;
        IupSetfAttribute(ctrl, "MASKFLOAT", IUP_DOUBLE2STR":"IUP_DOUBLE2STR, min, max);
                             
        /* here spin is always [0-spinmax] converted to [min-max] */

        IupSetAttribute(ctrl, "SPIN", "YES");   /* spin only for intervals */
        IupSetAttribute(ctrl, "SPINAUTO", "NO");
        IupAppend(box, ctrl);
        IupSetCallback(ctrl, "SPIN_CB", (Icallback)iParamSpinReal_CB);
        /* SPINMIN=0 and SPININC=1 */
        IupSetInt(ctrl, "SPINMAX", (int)((max-min)/step + 0.5));
        IupSetInt(ctrl, "SPINVALUE", (int)((val-min)/step + 0.5));

        iupAttribSetDouble(ctrl, "_IUPGP_INCSTEP", step);
        iupAttribSet(ctrl, "_IUPGP_SPINREAL", "1");
      }
      else if (iupAttribGetInt(param, "PARTIAL"))
      {
        double min = iupAttribGetDouble(param, "MIN");
        if (min == 0)
          IupSetAttribute(ctrl, "MASKREAL", "UNSIGNED");
        else
          IupSetfAttribute(ctrl, "MASKFLOAT", IUP_DOUBLE2STR":"IUP_DOUBLE2STR, min, 1.0e10);

        IupAppend(box, ctrl);
      }
      else
      {
        IupSetAttribute(ctrl, "MASKREAL", "SIGNED");
        IupAppend(box, ctrl);
      }
    }
    else /* INTEGER*/
    {
      int val = iupAttribGetInt(param, "VALUE");
      IupSetInt(ctrl, "VALUE", val);

      IupSetAttribute(ctrl, "SPIN", "YES");   /* spin always */
      IupSetAttribute(ctrl, "SPINAUTO", "NO");  /* manually update spin so the callback can also updated it */
      IupAppend(box, ctrl);
      IupSetCallback(ctrl, "SPIN_CB", (Icallback)iParamSpinInt_CB);
      iupAttribSet(ctrl, "_IUPGP_INCSTEP", "1");
      IupSetInt(ctrl, "SPINVALUE", val);

      /* here spin is always [min-max] */

      if (iupAttribGetInt(param, "INTERVAL"))
      {
        int min = iupAttribGetInt(param, "MIN");
        int max = iupAttribGetInt(param, "MAX");
        int step = iupAttribGetInt(param, "STEP");
        if (step)
        {
          iupAttribSetInt(ctrl, "_IUPGP_INCSTEP", step);
          IupSetInt(ctrl, "SPININC", step);
        }
        IupSetInt(ctrl, "SPINMAX", max);
        IupSetInt(ctrl, "SPINMIN", min);
        IupSetfAttribute(ctrl, "MASKINT", "%d:%d", min, max);
      }
      else if (iupAttribGetInt(param, "PARTIAL"))
      {
        int min = iupAttribGetInt(param, "MIN");
        if (min == 0)
          IupSetAttribute(ctrl, "MASK", IUP_MASK_UINT);
        else
          IupSetfAttribute(ctrl, "MASKINT", "%d:2147483647", min);
        IupSetInt(ctrl, "SPINMIN", min);
        IupSetAttribute(ctrl, "SPINMAX", "2147483647");
      }
      else                             
      {
        IupSetAttribute(ctrl, "SPINMIN", "-2147483647");
        IupSetAttribute(ctrl, "SPINMAX", "2147483647");
        IupSetAttribute(ctrl, "MASK", IUP_MASK_INT);
      }
    }

    if (iupAttribGetInt(param, "INTERVAL") || iupAttribGetInt(param, "ANGLE"))
    {
      Ihandle* aux;

      if (iupAttribGetInt(param, "ANGLE"))
      {
        aux = IupCreate("dial");  /* Use IupCreate to avoid depending on the IupControls library */
        if (aux)  /* If IupControls library is not available it will fail */
        {
          IupSetDouble(aux, "VALUE", iupAttribGetDouble(param, "VALUE")/RAD2DEG);
          IupSetAttribute(aux, "SIZE", "50x10");
        }
      }
      else
      {
        char* step;

        aux = IupVal("HORIZONTAL");
        IupStoreAttribute(aux, "MIN", iupAttribGet(param, "MIN"));
        IupStoreAttribute(aux, "MAX", iupAttribGet(param, "MAX"));
        IupStoreAttribute(aux, "VALUE", iupAttribGet(param, "VALUE"));
        IupSetAttribute(aux, "EXPAND", "HORIZONTAL");
        iupAttribSet(param, "AUXCONTROL", (char*)aux);
        step = iupAttribGet(param, "STEP");
        if (step)
          IupSetDouble(aux, "STEP", iupAttribGetDouble(param, "STEP")/(iupAttribGetDouble(param, "MAX")-iupAttribGetDouble(param, "MIN")));
        else if (iupStrEqual(type, "INTEGER"))
          IupSetDouble(aux, "STEP", 1.0f/(iupAttribGetDouble(param, "MAX")-iupAttribGetDouble(param, "MIN")));
      }

      if (aux)
      {
        IupSetCallback(aux, "VALUECHANGED_CB", (Icallback)iParamValAction_CB);
        iupAttribSet(aux, "PARAM", (char*)param);
        iupAttribSet(aux, "CONTROL", (char*)ctrl);

        IupAppend(box, aux);
      }
    }
  }

  if (ctrl) IupStoreAttribute(ctrl, "TIP", iupAttribGet(param, "TIP"));
  iupAttribSet(box, "PARAM", (char*)param);
  iupAttribSet(param, "CONTROL", (char*)ctrl);
  iupAttribSet(param, "LABEL", (char*)label);
  return box;
}

/*******************************************************************************************
                    Creates the Dialog and Normalize Sizes
*******************************************************************************************/

static void iParamBoxNormalizeSize(Ihandle** params, int count)
{
  int i, lbl_width;

  /* get the largest label size and set INDEX */
  lbl_width = 0;
  for (i = 0; i < count; i++)
  {
    int w = IupGetInt((Ihandle*)iupAttribGet(params[i], "LABEL"), "NATURALSIZE");
    if (w > lbl_width)
      lbl_width = w;
  }

  for (i = 0; i < count; i++)
  {
    if (!iupStrEqualNoCase(iupAttribGet(params[i], "DATATYPE"), "NONE"))
    {
      char* type = iupAttribGet(params[i], "TYPE");
      if (iupStrEqual(type, "LIST"))
      {
        /* set a minimum size for lists */
        Ihandle* ctrl = (Ihandle*)iupAttribGet(params[i], "CONTROL");
        int charwidth = IupGetInt(ctrl, "CHARSIZE");
        if (IupGetInt(ctrl, "NATURALSIZE") * 4 < 50 * charwidth)
          IupSetInt(ctrl, "RASTERSIZE", (50 * charwidth) / 4);
      }
      else if (iupStrEqual(type, "BOOLEAN"))
      {
        /* reserve enough space for both strings */
        Ihandle* ctrl = (Ihandle*)iupAttribGet(params[i], "CONTROL");
        int wf = iupdrvFontGetStringWidth(ctrl, iupAttribGet(params[i], "FALSE"));
        int wt = iupdrvFontGetStringWidth(ctrl, iupAttribGet(params[i], "TRUE"));
        int w = IupGetInt(ctrl, "NATURALSIZE");
        int v = IupGetInt(ctrl, "VALUE");
        if (v) /* True */
        {
          int box = w - wt;
          wf += box;
          if (wf > w)
            IupSetfAttribute(ctrl, "RASTERSIZE", "%dx", wf + 8);
        }
        else
        {
          int box = w - wf;
          wt += box;
          if (wt > w)
            IupSetfAttribute(ctrl, "RASTERSIZE", "%dx", wt + 8);
        }
      }

      /* set the same size for all labels so they will align the controls column */
      IupSetfAttribute((Ihandle*)iupAttribGet(params[i], "LABEL"), "RASTERSIZE", "%dx", lbl_width);
    }
  }
}

Ihandle* IupParamBox(Ihandle* parent, Ihandle** params, int count)
{
  Ihandle *button_1, *button_2, *button_3=NULL, 
          *param_box, *button_box, *ctrl_box;
  int i, p, text_expand;

  button_1 = IupButton(parent? "_@IUP_APPLY": "_@IUP_OK", NULL);  /* default is OK */
  IupSetAttribute(button_1, "PADDING", IupGetGlobal("DEFAULTBUTTONPADDING"));
  IupSetCallback(button_1, "ACTION", (Icallback)iParamButton1_CB);

  button_2 = IupButton(parent? "_@IUP_RESET": "_@IUP_CANCEL", NULL);  /* default is Cancel */
  IupSetAttribute(button_2, "PADDING", IupGetGlobal("DEFAULTBUTTONPADDING"));
  IupSetCallback(button_2, "ACTION", (Icallback)iParamButton2_CB);
  
  ctrl_box = IupVbox(NULL);

  text_expand = 0;
  for (i = 0; i < count; i++)
  {
    char *type = iupAttribGet(params[i], "TYPE");

    if (iupStrEqual(type, "BUTTONNAMES"))
    {
      char* value = iupAttribGet(params[i], "BUTTON1");
      if (value && *value) IupSetStrAttribute(button_1, "TITLE", value);
      value = iupAttribGet(params[i], "BUTTON2");
      if (value && *value) IupSetStrAttribute(button_2, "TITLE", value);
      value = iupAttribGet(params[i], "BUTTON3");
      if (value && *value) 
      {
        button_3 = IupButton(value, NULL);
        IupSetAttribute(button_3, "PADDING", IupGetGlobal("DEFAULTBUTTONPADDING"));
        IupSetCallback(button_3, "ACTION", (Icallback)iParamButton3_CB);
      }

      IupSetCallback(button_1, "DESTROY_CB", iParamDestroy_CB);
      iupAttribSet(button_1, "PARAM", (char*)params[i]);
    }
    else if (iupStrEqual(type, "HANDLE"))
    {
      Ihandle* ih = (Ihandle*)iupAttribGet(params[i], "VALUE");
      IupSetCallback(ih, "DESTROY_CB", iParamDestroy_CB);
      iupAttribSet(ih, "PARAM", (char*)params[i]);
      IupAppend(ctrl_box, ih);
    }
    else
      IupAppend(ctrl_box, iParamCreateCtrlBox(params[i], type));

    if (IupGetInt(params[i], "TEXTEXPAND"))
      text_expand = 1;
  }

  button_box = IupHbox(
    IupFill(),
    button_1,
    button_2,
    button_3,
    NULL);
  IupSetAttribute(button_box,"MARGIN","0x0");
  IupSetAttribute(button_box, "NORMALIZESIZE", "HORIZONTAL");

  param_box = IupVbox(
    IupFrame(ctrl_box),
    button_box,
    NULL);
  IupSetAttribute(param_box, "MARGIN", "10x10");
  IupSetAttribute(param_box, "GAP", "5");

  if (!parent)
  {
    Ihandle* dlg = IupDialog(param_box);

    if (!text_expand)
    {
      IupSetAttribute(dlg, "DIALOGFRAME", "YES");  /* RESIZE=NO, MINBOX=NO and MAXBOX=NO */
      IupSetAttribute(dlg, "DIALOGHINT", "YES");  /* GTK Only */
    }
    else
    {
      IupSetAttribute(dlg, "MINBOX", "NO");
      IupSetAttribute(dlg, "MAXBOX", "NO");
    }

    IupSetAttributeHandle(dlg, "DEFAULTENTER", button_1);
    IupSetAttributeHandle(dlg, "DEFAULTESC", button_2);

    param_box = dlg;
  }
  else
    IupAppend(parent, param_box);

  /* set INDEX */
  p = 0;
  for (i = 0; i < count; i++)
  {
    if (!iupStrEqualNoCase(iupAttribGet(params[i], "DATATYPE"), "NONE"))
    {
      iupAttribSetId(param_box, "PARAM", p, (char*)params[i]);
      iupAttribSetInt(params[i], "INDEX", p);
      p++;
    }
  }

  iupAttribSetInt(param_box, "PARAMCOUNT", p);

  iupAttribSet(param_box, "BUTTON1", (char*)button_1);
  iupAttribSet(param_box, "BUTTON2", (char*)button_2);
  iupAttribSet(param_box, "BUTTON3", (char*)button_3);

  /* trick to update the label and text sizes */
  iupLayoutCompute(param_box);

  iParamBoxNormalizeSize(params, count);

  /* Force a dialog resize */
  if (!parent)
    IupSetAttribute(param_box, "SIZE", NULL);

  IupSetAttribute(param_box, "PARAMBOX", (char*)param_box);  /* found by inheritance */

  return param_box;
}

/*******************************************************************************************
                    Parameter String Parsing
*******************************************************************************************/

static char* iParamStrGetNextItem(char* line, char sep, int *count)
{
  int i = 0;

  while (line[i] != '\n' && line[i] != 0)
  {
    if (line[i] == sep)
    {
      line[i] = 0;
      *count = i+1;
      return line;
    }

    i++;
  }

  /* last item may not have the separator */
  line[i] = 0;
  *count = i;
  return line;
}

static void iParamStrSetBoolNames(char* extra, Ihandle* param)
{
  char *falsestr = NULL, *truestr = NULL;
  int count;

  if (extra)
  {
    falsestr = iParamStrGetNextItem(extra, ',', &count);  extra += count;
    truestr = iParamStrGetNextItem(extra, ',', &count);
  }

  if (falsestr && truestr)
  {
    iupAttribSetStr(param, "TRUE", truestr);
    iupAttribSetStr(param, "FALSE", falsestr);
  }
  else
  {
    iupAttribSetStr(param, "TRUE", "");
    iupAttribSetStr(param, "FALSE", "");
  }
}

static void iParamStrSetInterval(char* extra, Ihandle* param)
{
  char *min, *max, *step;
  int count;

  if (!extra)
    return;

  min = iParamStrGetNextItem(extra, ',', &count);  extra += count;
  max = iParamStrGetNextItem(extra, ',', &count);  extra += count;
  step = iParamStrGetNextItem(extra, ',', &count);  

  if (max[0])
  {
    iupAttribSet(param, "INTERVAL", "Yes");
    iupAttribSetStr(param, "MIN", min);
    iupAttribSetStr(param, "MAX", max);
    if (step[0])
      iupAttribSetStr(param, "STEP", step);
  }
  else
  {
    iupAttribSet(param, "PARTIAL", "Yes");
    iupAttribSetStr(param, "MIN", min);
  }
}

static void iParamStrSetFileOptions(char* extra, Ihandle* param)
{
  char *type, *filter, *directory, *nochangedir, *nooverwriteprompt;
  int count;

  if (!extra)
    return;

  type = iParamStrGetNextItem(extra, '|', &count);  extra += count;
  filter = iParamStrGetNextItem(extra, '|', &count);  extra += count;
  directory = iParamStrGetNextItem(extra, '|', &count);  extra += count;
  nochangedir = iParamStrGetNextItem(extra, '|', &count);  extra += count;
  nooverwriteprompt = iParamStrGetNextItem(extra, '|', &count);  extra += count;

  iupAttribSetStr(param, "DIALOGTYPE", type);
  iupAttribSetStr(param, "FILTER", filter);
  iupAttribSetStr(param, "DIRECTORY", directory);
  iupAttribSetStr(param, "NOCHANGEDIR", nochangedir);
  iupAttribSetStr(param, "NOOVERWRITEPROMPT", nooverwriteprompt);
}

static void iParamStrSetButtonNames(char* extra, Ihandle* param)
{
  char *button1, *button2, *button3;
  int count;

  if (!extra)
    return;

  button1 = iParamStrGetNextItem(extra, ',', &count);  extra += count;
  button2 = iParamStrGetNextItem(extra, ',', &count);  extra += count;
  button3 = iParamStrGetNextItem(extra, ',', &count);  extra += count;

  iupAttribSetStr(param, "BUTTON1", button1);
  iupAttribSetStr(param, "BUTTON2", button2);
  iupAttribSetStr(param, "BUTTON3", button3);
}

static void iParamStrSetListItems(char* extra, Ihandle* param)
{
  int i = 0, count;
  char *item;

  if (!extra)
    return;

  item = iParamStrGetNextItem(extra, '|', &count);  extra += count;
  while (*item)
  {
    iupAttribSetStrId(param, "", i, item);

    item = iParamStrGetNextItem(extra, '|', &count);  extra += count;
    i++;
  }

  iupAttribSetId(param, "", i, "");
}

static char* iParamStrGetExtra(char* line, char start, char end, int *count)
{
  int i = 0, end_pos = -1;

  if (*line != start)
  {
    *count = 0;
    return NULL;
  }
  line++; /* skip start */

  while (line[i] != '\n' && line[i] != 0)
  {
    if (line[i] == end)
      end_pos = i;

    i++;
  }

  if (end_pos != -1)
  {
    line[end_pos] = 0;
    *count = 1+end_pos+1;
    return line;
  }
  else
  {
    *count = 0;
    return NULL;
  }
}

static int iParamStrLineSize(const char* format)
{
  int i = 0;
  while (format[i] != '\n' && format[i] != 0)
  {
    i++;
    if (i > 4094)   /* to avoid being bigger than the local array */
      break;
  }
  return i + 1;
}

static void iParamStrCopyLine(char* line, const char* format)
{
  int i = 0;
  while (format[i] != '\n' && format[i] != 0)
  {
    line[i] = format[i];
    i++;
    if (i > 4094)   /* to avoid being bigger than the local array */
      break;
  }
  line[i] = '\n';
  line[i+1] = 0;
}

static char* iParamStrGetType(const char* format)
{
  char* type = strchr(format, '%');
  while (type && *(type+1)=='%')
    type = strchr(type+2, '%');
  return type;
}

/* Used in IupLua */
char iupGetParamType(const char* format, int *line_size)
{
  char* type = iParamStrGetType(format);
  char* line_end = strchr(format, '\n');
  if (line_end)
    *line_size = (int)(line_end-format) + 1;  /* include line separator */
  if (type)
    return *(type+1);  /* skip separator */
  else
    return 0;
}

static char* iParamStrGetTitle(char* line_ptr, int count)
{
  int i, n;
  char* title = line_ptr;
  title[count] = 0;
  n = -1;
  for (i=0; i<count; i++)
  {
    if (title[i] != '%')
      n++;
    else /* if (title[i+1] == '%')  if equal to %, next is a %, because we are inside title only */
    {
      n++;
      i++;  /* skip second % */
    }

    if (i != n)  /* copy only if there is a second % */
      title[n] = title[i];
  }
  if (i-1 != n)
    title[n+1] = 0;
  return title;
}

Ihandle* IupParamf(const char* format)
{
  Ihandle* param;
  char line[4096];
  char* line_ptr = &line[0], *title, *type, *tip, *extra, *mask;
  int count;

  iParamStrCopyLine(line, format);

  type = iParamStrGetType(line_ptr);
  if (!type)
    return NULL;

  count = (int)(type-line_ptr);
  title = iParamStrGetTitle(line_ptr, count);
  line_ptr += count;

  type++; /* skip separator */
  line_ptr += 2;

  param = IupUser();
  if (title[0] == '\t')
  {
    int indent = 0;
    while (title[0] == '\t')
    {
      indent++;
      title++;
    }
    iupAttribSetInt(param, "INDENT", indent);
  }
  iupAttribSetStr(param, "TITLE", title);
  
/**********************************************************************************
  REMEMBER: if a new parameter type is added
            then IupLua must be also updated.
 **********************************************************************************/

  switch(*type)
  {
  case 'b':
    iupAttribSet(param, "TYPE", "BOOLEAN");
    iupAttribSet(param, "DATATYPE", "INT");
    extra = iParamStrGetExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamStrSetBoolNames(extra, param);
    break;
  case 'l':
    iupAttribSet(param, "TYPE", "LIST");
    iupAttribSet(param, "DATATYPE", "INT");
    extra = iParamStrGetExtra(line_ptr, '|', '|', &count);  line_ptr += count;
    iParamStrSetListItems(extra, param);
    break;
  case 'o':
    iupAttribSet(param, "TYPE", "OPTIONS");
    iupAttribSet(param, "DATATYPE", "INT");
    extra = iParamStrGetExtra(line_ptr, '|', '|', &count);  line_ptr += count;
    iParamStrSetListItems(extra, param);
    break;
  case 'A':
    iupAttribSet(param, "TYPE", "REAL");
    iupAttribSet(param, "DATATYPE", "DOUBLE");
    iupAttribSet(param, "ANGLE", "Yes");
    extra = iParamStrGetExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamStrSetInterval(extra, param);
    break;
  case 'a':
    iupAttribSet(param, "TYPE", "REAL");
    iupAttribSet(param, "DATATYPE", "FLOAT");
    iupAttribSet(param, "ANGLE", "Yes");
    extra = iParamStrGetExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamStrSetInterval(extra, param);
    break;
  case 'm':
    iupAttribSet(param, "MULTILINE", "Yes");
    /* continue */
  case 's':
    iupAttribSet(param, "TYPE", "STRING");
    iupAttribSet(param, "DATATYPE", "STRING");
    mask = iParamStrGetNextItem(line_ptr, '{', &count);  
    if (*mask) 
      iupAttribSetStr(param, "MASK", mask);
    line_ptr += count-1; /* ignore the fake separator */
    line_ptr[0] = '{';   /* restore possible separator */
    break;
  case 'i':
    iupAttribSet(param, "TYPE", "INTEGER");
    iupAttribSet(param, "DATATYPE", "INT");
    extra = iParamStrGetExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamStrSetInterval(extra, param);
    break;
  case 'R':
    iupAttribSet(param, "TYPE", "REAL");
    iupAttribSet(param, "DATATYPE", "DOUBLE");
    extra = iParamStrGetExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamStrSetInterval(extra, param);
    break;
  case 'r':
    iupAttribSet(param, "TYPE", "REAL");
    iupAttribSet(param, "DATATYPE", "FLOAT");
    extra = iParamStrGetExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamStrSetInterval(extra, param);
    break;
  case 'f':
    iupAttribSet(param, "TYPE", "FILE");
    iupAttribSet(param, "DATATYPE", "STRING");
    extra = iParamStrGetExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamStrSetFileOptions(extra, param);
    break;
  case 'n':
    iupAttribSet(param, "TYPE", "FONT");
    iupAttribSet(param, "DATATYPE", "STRING");
    break;
  case 'c':
    iupAttribSet(param, "TYPE", "COLOR");
    iupAttribSet(param, "DATATYPE", "STRING");
    break;
  case 't':
    iupAttribSet(param, "TYPE", "SEPARATOR");
    iupAttribSet(param, "DATATYPE", "NONE");
    break;
  case 'u':
    iupAttribSet(param, "TYPE", "BUTTONNAMES");
    iupAttribSet(param, "DATATYPE", "NONE");
    extra = iParamStrGetExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamStrSetButtonNames(extra, param);
    break;
  case 'h':
    iupAttribSet(param, "TYPE", "HANDLE");
    iupAttribSet(param, "DATATYPE", "HANDLE");
    break;
  default:
    IupDestroy(param);
    return NULL;
  }

  tip = iParamStrGetExtra(line_ptr, '{', '}', &count);
  if (tip)
    iupAttribSetStr(param, "TIP", tip);

  return param;
}

/* Used in IupLua also */
int iupGetParamCount(const char *format, int *param_extra)
{
  int param_count = 0, extra = 0;
  const char* s = format;

  *param_extra = 0;
  while (*s)
  {
    if (*s == '%' && *(s + 1) == 't')  /* do not count separator lines */
    {
      extra = 1;
      (*param_extra)++;
    }

    if (*s == '%' && *(s + 1) == 'u')  /* do not count BUTTONNAMES lines */
    {
      extra = 1;
      (*param_extra)++;
    }

    if (*s == '\n')
    {
      if (extra)
        extra = 0;
      else
        param_count++;
    }

    s++;
  }

  return param_count;
}


/*******************************************************************************************
                    Dialog Functions
*******************************************************************************************/


int IupGetParamv(const char* title, Iparamcb action, void* user_data, const char* format, int param_count, int param_extra, void** param_data)
{
  Ihandle *dlg, **params;
  int i, line_size, p, count;

  assert(title && format);
  if (!title || !format)
    return 0;

  count = param_count + param_extra;
  params = malloc(count*sizeof(Ihandle*));

  p = 0;
  for (i = 0; i < count; i++)
  {
    char* data_type;

    params[i] = IupParamf(format);
    assert(params[i]);
    if (!params[i])
    {
      free(params);
      return 0;
    }

    data_type = iupAttribGet(params[i], "DATATYPE");
    if (iupStrEqualNoCase(data_type, "FLOAT"))
    {
      float *data_float = (float*)(param_data[p]);
      if (!data_float) { free(params); return 0; }
      iupAttribSetFloat(params[i], "VALUE", *data_float);
      p++;
    }
    else if (iupStrEqualNoCase(data_type, "DOUBLE"))
    {
      double *data_double = (double*)(param_data[p]);
      if (!data_double) { free(params); return 0; }
      iupAttribSetDouble(params[i], "VALUE", *data_double);
      p++;
    }
    else if (iupStrEqualNoCase(data_type, "INT"))
    {
      int *data_int = (int*)(param_data[p]);
      if (!data_int) { free(params); return 0; }
      iupAttribSetInt(params[i], "VALUE", *data_int);
      p++;
    }
    else if (iupStrEqualNoCase(data_type, "STRING"))
    {
      char *data_str = (char*)(param_data[p]);
      if (!data_str) { free(params); return 0; }
      iupAttribSetStr(params[i], "VALUE", data_str);
      p++;
    }
    else if (iupStrEqualNoCase(data_type, "HANDLE"))  /* Input Only */
    {
      Ihandle* data_ih = (Ihandle*)(param_data[p]);
      if (!data_ih) { free(params); return 0; }
      iupAttribSet(params[i], "VALUE", (char*)data_ih);
      p++;
    }

    line_size = iParamStrLineSize(format);
    format += line_size;
  }

  dlg = IupParamBox(NULL, params, count);
  IupSetAttribute(dlg, "PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));
  IupSetAttribute(dlg, "ICON", IupGetGlobal("ICON"));
  IupSetStrAttribute(dlg, "TITLE", (char*)title);
  IupSetCallback(dlg, "CLOSE_CB", (Icallback)iParamDlgClose_CB);
  IupSetCallback(dlg, "PARAM_CB", (Icallback)action);
  iupAttribSet(dlg, "USERDATA", (char*)user_data);

  if (action) 
    action(dlg, IUP_GETPARAM_INIT, user_data);

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (!IupGetInt(dlg, "STATUS"))
  {
    IupDestroy(dlg);
    free(params); 
    return 0;
  }
  else
  {
    p = 0;
    for (i = 0; i < count; i++)
    {
      Ihandle* param = params[i];
      char* data_type = iupAttribGet(param, "DATATYPE");
      if (iupStrEqualNoCase(data_type, "INT"))
      {
        int *data_int = (int*)(param_data[p]);
        *data_int = iupAttribGetInt(param, "VALUE");
        p++;
      }
      else if (iupStrEqualNoCase(data_type, "FLOAT"))
      {
        float *data_float = (float*)(param_data[p]);
        *data_float = iupAttribGetFloat(param, "VALUE");
        p++;
      }
      else if (iupStrEqualNoCase(data_type, "DOUBLE"))
      {
        double *data_double = (double*)(param_data[p]);
        *data_double = iupAttribGetDouble(param, "VALUE");
        p++;
      }
      else if (iupStrEqualNoCase(data_type, "STRING"))
      {
        char *data_str = (char*)(param_data[p]);
        int max_str = iupAttribGetInt(param, "MAXSTR");
        if (!max_str)
        {
          max_str = 512;
          if (iupStrEqual(iupAttribGet(param, "TYPE"), "FILE"))
            max_str = 4096;
          else if (iupStrEqual(iupAttribGet(param, "TYPE"), "STRING") && iupAttribGetInt(param, "MULTILINE"))
            max_str = 10240;
        }
        iupStrCopyN(data_str, max_str, iupAttribGet(param, "VALUE"));
        p++;
      }
    }

    IupDestroy(dlg);
    free(params);
    return 1;
  }
}

int IupGetParam(const char* title, Iparamcb action, void* user_data, const char* format,...)
{
  int param_count, param_extra, i, ret;
  void** param_data;
  va_list arg;

  param_count = iupGetParamCount(format, &param_extra);

  param_data = malloc(param_count*sizeof(void*));

  va_start(arg, format);
  for (i = 0; i < param_count; i++)
  {
    param_data[i] = (void*)(va_arg(arg, void*));
  }
  va_end(arg);

  ret = IupGetParamv(title, action, user_data, format, param_count, param_extra, param_data);

  free(param_data);
  return ret;
}

