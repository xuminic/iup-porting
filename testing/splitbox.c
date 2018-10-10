#include <stdio.h>
#include <iup.h>

static char	*setlist[] = {
	"IUP is a multi-platform toolkit for building graphical user interfaces.",
	"It offers a simple API in three basic languages: C, Lua and LED.",
	"IUP's purpose is to allow a program source code to be compiled",
	"in different systems",
	"without any modification.",
	"Its main advantages are:",
	"high performance,",
	"due to the fact that it uses native interface elements.",
	"fast learning by the user, due to the simplicity of its API.",
	"This work was developed at Tecgraf/PUC-Rio by means of the partnership with PETROBRAS/CENPES.",
	NULL
};

void SplitTest(void)
{
  Ihandle *dlg, *bt, *split, *ml, *vbox;
  int	i;

#if 0
  bt = IupButton("Button", NULL);
  IupSetAttribute(bt, "EXPAND", "YES");
#else
  bt = IupList(NULL);
  IupSetAttribute(bt, "EXPAND", "YES");
  IupSetAttribute(bt, "MULTIPLE", "YES");
  IupSetAttribute(bt, "SCROLLBAR", "YES");
  IupSetAttribute(bt, "ALIGNMENT", "ARIGHT");
  IupSetAttribute(bt, "CANFOCUS", "YES");
  for (i = 0; setlist[i]; i++) {
    IupSetStrAttributeId(bt, "", i+1, setlist[i]);
  }
#endif

  ml = IupMultiLine(NULL);
  IupSetAttribute(ml, "EXPAND", "YES");
  IupSetAttribute(ml, "VISIBLELINES", "5");
  IupSetAttribute(ml, "VISIBLECOLUMNS", "10");
  
  split = IupSplit(bt, ml);
//  IupSetAttribute(split, "ORIENTATION", "VERTICAL");
  IupSetAttribute(split, "ORIENTATION", "HORIZONTAL");
  IupSetAttribute(split, "COLOR", "127 127 255");

  vbox = IupVbox(split, NULL);
  IupSetAttribute(vbox, "MARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "10");

  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "IupSplit Example");
  IupSetAttribute(dlg, "RASTERSIZE", "800");

  IupShow(dlg);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  SplitTest();

  IupMainLoop();

  IupClose();

  return 0;
}
#endif
