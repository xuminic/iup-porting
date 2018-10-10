/*   Automatically generated by Iup 3 LED Compiler to C.   */

#include <stdlib.h>
#include <stdarg.h>
#include <iup.h>

static Ihandle* named[      8 ];

#include <iupcontrols.h>

void dial_led_load (void)
{
  named[0] = IupSetAtt( "lbl_h", IupLabel( "0" ), 
    "SIZE", "50x", NULL );
  named[1] = IupSetAtt( "lbl_v", IupLabel( "0" ), 
    "SIZE", "50x", NULL );
  named[2] = IupSetAtt( "lbl_c", IupLabel( "0" ), 
    "SIZE", "50x", NULL );
  named[3] = IupSetAtt( "dial_v", IupDial( "VERTICAL" ), 
    "UNIT", "DEGREES", 
    "MOUSEMOVE_CB", "dial_mousemove", 
    "BUTTON_PRESS_CB", "dial_buttonpress", 
    "BUTTON_RELEASE_CB", "dial_buttonrelease", NULL );
  named[4] = IupSetAtt( "dial_h", IupDial( "HORIZONTAL" ), 
    "MOUSEMOVE_CB", "dial_mousemove", 
    "BUTTON_PRESS_CB", "dial_buttonpress", 
    "BUTTON_RELEASE_CB", "dial_buttonrelease", NULL );
  named[5] = IupSetAtt( "dial_c", IupDial( "CIRCULAR" ), 
    "UNIT", "DEGREES", 
    "MOUSEMOVE_CB", "dial_mousemove", 
    "BUTTON_PRESS_CB", "dial_buttonpress", 
    "BUTTON_RELEASE_CB", "dial_buttonrelease", NULL );
  named[6] = IupSetAtt( "dlg", IupDialog(
    IupSetAtt( NULL, IupVbox(
      IupSetAtt( NULL, IupHbox(
        named[3] /* dial_v */,
        named[1] /* lbl_v */,
      NULL), 
        "ALIGNMENT", "ACENTER", NULL ),
      IupSetAtt( NULL, IupHbox(
        named[4] /* dial_h */,
        named[0] /* lbl_h */,
      NULL), 
        "ALIGNMENT", "ACENTER", NULL ),
      IupSetAtt( NULL, IupHbox(
        named[5] /* dial_c */,
        named[2] /* lbl_c */,
      NULL), 
        "ALIGNMENT", "ACENTER", NULL ),
    NULL), 
      "MARGIN", "10x10", 
      "GAP", "10", NULL )
  ), 
    "TITLE", "IupDial Test", NULL );
}