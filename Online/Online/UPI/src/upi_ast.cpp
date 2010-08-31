/*
        upic_ast.c
         Created           : 23-OCT-1989 by Christian Arnault
*/
#include "UPI/upidef.h"
#include "SCR/scr.h"

#ifdef SCREEN
static int s_upic_done = 0;
#endif
extern System Sys;

//---------------------------------------------------------------------------
void upic_ast() {
#ifdef SCREEN
  s_upic_done = 1;
#else
#endif
}

//---------------------------------------------------------------------------
void upic_enable_ast()    {
#ifdef SCREEN
  scrc_enable_unsolicited_input (Sys.pb, 0);
#else
#endif
}

//---------------------------------------------------------------------------
void upic_enable_input()    {
#ifdef SCREEN
  s_upic_done = 0;
#else
#endif
}
//---------------------------------------------------------------------------
int upic_done()    {
#ifdef SCREEN
  return s_upic_done;
#else
  return 0;
#endif
}

//---------------------------------------------------------------------------
void upic_disable_input()   {
#ifdef SCREEN
  s_upic_done = 0;
#ifdef VAX
#elif _OSK
  _ss_rel (0);
#endif
#else
#endif
}

//---------------------------------------------------------------------------
int upic_test_input()    {
#ifdef SCREEN
  return (scrc_test_input());
#else
  return UPI_SS_NORMAL;
#endif
}

//---------------------------------------------------------------------------
void upic_disable_ast()   {
#ifdef SCREEN
  scrc_disable_unsolicited_input (Sys.pb);
#else
#endif
}
