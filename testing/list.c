#include <stdio.h>
#include <iup.h>

//#define CFG_RESIZE_EVENT
//#define CFG_INITIAL_SIZE	"800x600"


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

static	Ihandle	*dlgmain, *ih_list;

#ifdef	CFG_SCROLLBOX
static	Ihandle	*sbox;
#endif

#ifdef	CFG_RESIZE_EVENT
static int event_resize(Ihandle *ih, int width, int height)
{
	printf("event_resize: %dx%d\n", width, height);
	return IUP_DEFAULT;
}
#endif

static void ListTest(void)
{
	int	i;

	ih_list = IupList(NULL);
	IupSetAttribute(ih_list, "EXPAND", "YES");
	IupSetAttribute(ih_list, "MULTIPLE", "YES");
	IupSetAttribute(ih_list, "SCROLLBAR", "YES");
	IupSetAttribute(ih_list, "ALIGNMENT", "ARIGHT");
	IupSetAttribute(ih_list, "CANFOCUS", "YES");
	IupSetAttribute(ih_list, "AUTOHIDE", "NO");
	for (i = 0; setlist[i]; i++) {
		IupSetStrAttributeId(ih_list, "", i+1, setlist[i]);
	}

	dlgmain = IupDialog(ih_list);
	IupSetAttribute(dlgmain, "TITLE", "IupList Example");
#ifdef	CFG_INITIAL_SIZE
	IupSetAttribute(dlgmain, "RASTERSIZE", CFG_INITIAL_SIZE);
#endif
#ifdef	CFG_RESIZE_EVENT
	IupSetCallback(dlgmain, "RESIZE_CB", (Icallback) event_resize);
#endif

	IupShow(dlgmain);
}

int main(int argc, char **argv)
{
	IupOpen(&argc, &argv);
	ListTest();
	IupMainLoop();
	IupClose();
	return 0;
}
