#include <stdlib.h>
#include <stdio.h>
#include <iup.h>
#include <iupkey.h>

//#define SCROLLBAR
//#define SCROLLBOX

static Ihandle *generate_workarea(void)
{
	Ihandle	*lb_main, *vb_main, *lst_main, *lb_size, *vb_size, *lst_size, *hbox;
	char	*p;
	int	i;

	lb_main = IupLabel("Files");
	//IupSetAttribute(lb_main, "RASTERSIZE", "200");
	lst_main = IupList(NULL);
	IupSetAttribute(lst_main, "EXPAND", "YES");
	IupSetAttribute(lst_main, "MULTIPLE", "YES");
#ifdef	SCROLLBAR
	IupSetAttribute(lst_main, "SCROLLBAR", "YES");
#else
	IupSetAttribute(lst_main, "SCROLLBAR", "VERTICAL");
#endif
	//vb_main = IupVbox(lb_main, lst_main, NULL);
	vb_main = IupVbox(IupSbox(lst_main), NULL);

	lb_size = IupLabel("Size");
	lst_size = IupList(NULL);
	IupSetAttribute(lst_size, "EXPAND", "VERTICAL");
	IupSetAttribute(lst_size, "SCROLLBAR", "NO");
	//vb_size = IupVbox(lb_size, lst_size, NULL);
	vb_size = IupVbox(IupSbox(lst_size), NULL);

#ifdef	SCROLLBOX
	for (i = 1; i <= 100; i++) {
#else
	for (i = 1; i <= 30; i++) {
#endif
		p = malloc(32);
		sprintf(p, "Mytestfile%03d.txt", i);
		IupSetAttributeId(lst_main, "", i, p);
		IupSetAttributeId(lst_size, "", i, "10.997GB");
	}

	hbox = IupHbox(vb_main, vb_size, NULL);
	return hbox;
}

int main(int argc, char* argv[])
{
	Ihandle	*mbox, *dlg;

	IupOpen(&argc, &argv);

	//mbox = IupButton("Remove", NULL);
#ifdef	SCROLLBOX
	mbox = IupScrollBox(generate_workarea());
	IupSetAttribute(mbox, "RASTERSIZE", "400x400");
#else
	mbox = generate_workarea();
#endif
	
	dlg = IupDialog(mbox);
	IupSetAttribute(dlg, "TITLE", "IupList Test");
	IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

	IupMainLoop();
	IupClose();
	return 0;
}
