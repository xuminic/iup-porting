
#include <stdio.h>
#include <iup.h>

//#define CFG_RESIZE_EVENT
#define CFG_INITIAL_SIZE	"800x600"
#define CFG_SCROLLBOX


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
	"Creates an interface element that displays a list of items.",
	"The list can be visible or can be dropped down. It also can have an edit box for text input.",
	"So it is a 4 in 1 element.",
	"In native systems the dropped down case is called Combo Box.",
	"The values can be any text. Items before \"1\" are ignored.",
	"Before map the first item with a NULL is considered the end of the list and items can be set in any order.",
	"After map, there are a few rules:",
	"if \"1\" is set to NULL, all items are removed.",
	"if \"id\" is set to NULL, all items after id are removed.",
	"if \"id\" is between the first and the last item, the current idth item is replaced.",
	"The effect is the same as removing the old item and inserting a new one at the old position.",
	"if \"id+1\" is set then it is appended after the last item.",
	"Items after \"id+1\" are ignored. (since 3.0)",
	NULL
};

static	Ihandle	*dlgmain, *list_upper, *list_lower, *splitbox;

#ifdef	CFG_SCROLLBOX
static	Ihandle	*sbox_upper, *sbox_lower, *sbox_hori;
#endif

#ifdef	CFG_RESIZE_EVENT
static int event_resize(Ihandle *ih, int width, int height)
{
	//printf("event_resize: %dx%d\n", width, height);
	IupSetAttribute(ih, "RASTERSIZE", IupGetAttribute(ih, "RASTERSIZE"));
	return IUP_DEFAULT;
}
#endif

void SplitTest(void)
{
	int	i;

	list_upper = IupList(NULL);
	IupSetAttribute(list_upper, "EXPAND", "YES");
	IupSetAttribute(list_upper, "MULTIPLE", "YES");
	IupSetAttribute(list_upper, "ALIGNMENT", "ARIGHT");
	IupSetAttribute(list_upper, "CANFOCUS", "YES");
#ifndef CFG_SCROLLBOX
	IupSetAttribute(list_upper, "SCROLLBAR", "YES");
	IupSetAttribute(list_upper, "AUTOHIDE", "NO");
#endif
	for (i = 0; setlist[i]; i++) {
		IupSetStrAttributeId(list_upper, "", i+1, setlist[i]);
	}

	list_lower = IupList(NULL);
	IupSetAttribute(list_lower, "EXPAND", "YES");
	IupSetAttribute(list_lower, "MULTIPLE", "YES");
	IupSetAttribute(list_lower, "ALIGNMENT", "ARIGHT");
	IupSetAttribute(list_lower, "CANFOCUS", "YES");
#ifndef CFG_SCROLLBOX
	IupSetAttribute(list_lower, "SCROLLBAR", "YES");
	IupSetAttribute(list_lower, "AUTOHIDE", "NO");
#endif
	for (i = 0; setlist[i]; i++) {
		IupSetStrAttributeId(list_lower, "", i+1, setlist[i]);
	}

#ifndef	CFG_SCROLLBOX
	splitbox = IupSplit(list_upper, list_lower);
#else
	sbox_upper = IupScrollBox(list_upper);
	//IupSetAttribute(sbox_upper, "SCROLLBAR", "VERTICAL");
	sbox_lower = IupScrollBox(list_lower);
	//IupSetAttribute(sbox_lower, "SCROLLBAR", "VERTICAL");
	splitbox = IupSplit(sbox_upper, sbox_lower);
#endif
	IupSetAttribute(splitbox, "ORIENTATION", "HORIZONTAL");	// "VERTICAL"
	IupSetAttribute(splitbox, "COLOR", "127 127 255");
	IupSetAttribute(splitbox, "VALUE", "500");
	
//#ifndef	CFG_SCROLLBOX
	dlgmain = IupDialog(splitbox);
//#else
//	sbox_hori = IupScrollBox(splitbox);
//	IupSetAttribute(sbox_hori, "SCROLLBAR", "HORIZONTAL");
//	dlgmain = IupDialog(sbox_hori);
//#endif
	IupSetAttribute(dlgmain, "TITLE", "IupSplit Example");
#ifdef	CFG_INITIAL_SIZE
	IupSetAttribute(dlgmain, "RASTERSIZE", CFG_INITIAL_SIZE);
#endif
#ifdef	CFG_RESIZE_EVENT
	IupSetCallback(dlgmain, "RESIZE_CB", (Icallback) event_resize);
#endif

	IupShow(dlgmain);
}

int main(int argc, char* argv[])
{
	IupOpen(&argc, &argv);
	SplitTest();
	IupMainLoop();
	IupClose();
	return 0;
}

