
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "iup.h"


Ihandle	*dlg_main;
Ihandle	*tune_label_total;
Ihandle	*tune_label_fail;
Ihandle	*tune_progress;
Ihandle *timer = NULL;

static	int 	evt_update;

/* old IUP doesn't have IupSetInt() */
void Iup_set_int(Ihandle* ih, const char* name, int num)
{
  char value[20];  /* +4,294,967,296 */
  sprintf(value, "%d", num);
  IupStoreAttribute(ih, name, value);
}


/* gcc -Wall -g -I./external/iup/include `pkg-config gtk+-2.0 --cflags` -L./external/iup/lib/Linux311_64  -o iup_thread_sync_timer iup_thread_sync_timer.c -lpthread -liup `pkg-config gtk+-2.0 --libs` -lX11
 */

static int tuner_idle(void)
{
	static	int	last_update;
	static	char	buf[64], tune_fail[64];

	if (evt_update != last_update) {
		last_update = evt_update;

		Iup_set_int(tune_progress, "VALUE", evt_update);

		sprintf(buf, "Frequency Tuned: %d", evt_update);
		IupSetAttribute(tune_label_total, "VALUE", buf);

		sprintf(tune_fail, "Failure of Tuning: %d", evt_update + 100);
		IupSetAttribute(tune_label_fail, "TITLE", tune_fail);

	}
	return 1;	/* must return TRUE */
}

static int time_cb(void)
{
	tuner_idle();
	return IUP_DEFAULT;
}

static void catch_alarm (int sig)
{
	printf("Alarm noted. [%d]\n", sig);
	alarm(5);
}

static void *task_command(void *arg)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	for (evt_update = 0; evt_update < 100; evt_update++) {
		sleep(1);
	}
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t	task_cmd;
	Ihandle		*vbox_tune;

	signal (SIGALRM, catch_alarm);
	alarm(5);

	IupOpen(&argc, &argv);
	IupSetGlobal("SINGLEINSTANCE", "IQTuner");

	tune_label_total = IupText(NULL);
	IupSetAttribute(tune_label_total, "READONLY", "YES");
	IupSetAttribute(tune_label_total, "BORDER", "NO");
	IupSetAttribute(tune_label_total, "BGCOLOR", "DLGBGCOLOR");
	IupSetAttribute(tune_label_total, "CANFOCUS", "NO");
	IupSetAttribute(tune_label_total, "SIZE", "120");

	tune_label_fail  = IupLabel(NULL);
	IupSetAttribute(tune_label_fail, "TITLE", "Failure of Tuning: 0");

	tune_progress = IupProgressBar();
	IupSetAttribute(tune_progress, "EXPAND", "HORIZONTAL");
	IupSetAttribute(tune_progress, "SIZE", "x10");
	IupSetAttribute(tune_progress, "DASHED", "YES");

	vbox_tune = IupVbox(tune_label_total, tune_label_fail, tune_progress, NULL);
	IupSetAttribute(vbox_tune, "NGAP", "8");
	IupSetAttribute(vbox_tune, "NMARGIN", "8x8");

	dlg_main = IupDialog(vbox_tune);
	IupSetAttribute(dlg_main, "TITLE", "IQTuner");
	IupSetAttribute(dlg_main, "RASTERSIZE", "500x400");

	IupShow(dlg_main);

	Iup_set_int(tune_progress, "MIN", 0);
	Iup_set_int(tune_progress, "MAX", 99);

	timer = IupTimer();
	IupSetCallback(timer, "ACTION_CB", (Icallback)time_cb);
	IupSetAttribute(timer, "TIME", "100");
	IupSetAttribute(timer, "RUN", "YES");

	pthread_create(&task_cmd, NULL, task_command, NULL);
	IupMainLoop();
	pthread_cancel(task_cmd);
	pthread_join(task_cmd, NULL);

	IupDestroy(dlg_main);
	IupClose();
	return 0;
}

