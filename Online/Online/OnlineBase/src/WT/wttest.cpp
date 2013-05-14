//----------------------------------------------------------------------
//    WAIT FACILITY TEST PROGRAM
//----------------------------------------------------------------------
#include "WT/wtdef.h"
#include "RTL/rtl.h"
#include "SCR/scr.h"
#include <cstdlib>
#include <cstdarg>

static int INTERVAL_NORMAL = 1;
static int INTERVAL1 = 6*INTERVAL_NORMAL;
static int INTERVAL2 = 60*INTERVAL_NORMAL;
static int INTERVAL3 = 600*INTERVAL_NORMAL;
static int INTERVAL4 = 6000*INTERVAL_NORMAL;

static unsigned long alarm1, alarm2, alarm3, alarm4;
static int ast1_count = 0;
static int ast2_count = 0;
static int ast3_count = 0;
static int ast4_count = 0;
static int rearm4_count = -1;
static SCR::Pasteboard* pasteboard = 0;
static SCR::Display*    display = 0;

static void __print_at_file(int, int, const char* fmt, ...)  {
  va_list args;
  char buff[256];
  va_start( args, fmt );
  ::vsprintf( buff, fmt, args);
  va_end(args);
  printf("%s\n",buff);
}

static void __print_at_screen(int x, int y, const char* fmt, ...)  {
  va_list args;
  char buff[256];
  va_start( args, fmt );
  ::vsprintf( buff, fmt, args);
  va_end(args);
  ::scrc_begin_pasteboard_update(pasteboard);
  ::scrc_put_chars(display,buff,SCR::NORMAL,y,x,0);
  ::scrc_end_pasteboard_update(pasteboard);
}

static void (*print_at)(int x, int y, const char* fmt, ...) = 0;

static int ast1(void* par)  {
  ast1_count++;
  return wtc_insert(WT_FACILITY_TIMER1, par);
}

static int rearm1(unsigned int /* fac */, void* /* par */)  {
  static int count = 0;
  if ( (++count%50)==0 ) print_at(40, 4, "%5d",count);
  int sc = lib_rtl_set_timer(INTERVAL1, ast1, 0, &alarm1);
  return lib_rtl_is_success(sc) ? WT_SUCCESS : WT_ERROR;
}

static int action1(unsigned int /* fac */, void* /* par */) {
  static int count = 0;
  if ( (++count%50)==0 ) {
    print_at(50, 4, "%5d",ast1_count);
    print_at(30, 4, "%5d",count);
  }
  return WT_SUCCESS;
}

static int ast2(void* par) {
  ast2_count++;
  return wtc_insert(WT_FACILITY_TIMER2,par);
}

static int rearm2(unsigned int /* fac */, void* /* par */)  {
  static int count = 0;
  print_at(40, 5, "%5d",++count);
  return lib_rtl_set_timer(INTERVAL2, ast2, 0, &alarm2);
}

static int action2(unsigned int /* fac */, void* /* par */) {
  static int count = 0;
  print_at(50, 5, "%5d",ast2_count);
  print_at(30, 5, "%5d",++count);
  return WT_SUCCESS;
}

static int ast3(void* par)  {
  ast3_count++;
  return wtc_insert(WT_FACILITY_TIMER3, par);
}

static int rearm3(unsigned int /* fac */, void* /* par */)  {
  static int count = 0;
  print_at(40, 6, "%5d",++count);
  return lib_rtl_set_timer(INTERVAL3, ast3, 0, &alarm3);
}

static int action3(unsigned int /* fac */, void* /* par */) {
  static int count = 0;
  print_at(50, 6, "%5d",ast3_count);
  print_at(30, 6, "%5d",++count);
  return WT_SUCCESS;
}

static int ast4(void* par)  {
  ast4_count++;
  return wtc_insert(WT_FACILITY_TIMER4, par);
}

static int rearm4(unsigned int /* fac */, void* /* par */)  {
  static int count = 0;
  print_at(40, 7, "%5d",++count);
  if ( rearm4_count > 0 ) --rearm4_count;
  if ( rearm4_count == 0 ) exit(0);
  return lib_rtl_set_timer(INTERVAL4, ast4, 0, &alarm4);
}

static int action4(unsigned int /* fac */, void* /* par */) {
  static int count = 0;
  print_at(50, 7, "%5d",ast4_count);
  print_at(30, 7, "%5d",++count);
  return WT_SUCCESS;
}

extern "C" int wtc_test(int /* argc */, char** /* argv */)   {  
  int width, height;
  print_at = __print_at_screen;

  int status = wtc_init();
  if( status != WT_SUCCESS ) exit(status);

  ::scrc_create_pasteboard (&pasteboard, 0, &height, &width);
  ::scrc_create_display (&display, height-2, width-2, SCR::NORMAL, SCR::ON, "WT test program");
  ::scrc_paste_display  (display, pasteboard, 2, 2);
  ::scrc_end_pasteboard_update(pasteboard);
  ::scrc_fflush(pasteboard);
  ::scrc_set_cursor(display, 2, 10);
  ::scrc_cursor_off(pasteboard);

  print_at(4,2,"Object name             Actions    Rearms      Asts");
  print_at(4,4,"Timer 1 (%5d msecs):",INTERVAL1);
  print_at(4,5,"Timer 2 (%5d msecs):",INTERVAL2);
  print_at(4,6,"Timer 3 (%5d msecs):",INTERVAL3);
  print_at(4,7,"Timer 4 (%5d msecs):",INTERVAL4);

  //---TIMER 1----
  status = wtc_subscribe( WT_FACILITY_TIMER1, rearm1, action1);
  if( status != WT_SUCCESS ) exit(status);
  //---TIMER 2----
  status = wtc_subscribe( WT_FACILITY_TIMER2, rearm2, action2);
  if( status != WT_SUCCESS ) exit(status);
  //---TIMER 3----
  status = wtc_subscribe( WT_FACILITY_TIMER3, rearm3, action3);
  if( status != WT_SUCCESS ) exit(status);
  //---TIMER 4----
  status = wtc_subscribe( WT_FACILITY_TIMER4, rearm4, action4);
  if( status != WT_SUCCESS ) exit(status);
  for(;;)  {
    unsigned int facility;
    int sub_status;
    void* userpar;
    status = wtc_wait(&facility, &userpar, &sub_status);
    lib_rtl_output(LIB_RTL_ALWAYS,"Exited WAIT>>>> Facility = %d Status=%d Sub-Status = %d\n", 
      facility, status, sub_status);
  }
}

extern "C" int wtc_test_qmtest(int /* argc */, char** /* argv */)   {  
  print_at = __print_at_file;
  rearm4_count = 4;
  INTERVAL2 = 200*INTERVAL_NORMAL;
  int status = wtc_init();
  if( status != WT_SUCCESS ) exit(status);
  print_at(4,2,"Object name             Actions    Rearms      Asts");
  print_at(4,5,"Timer 2 (%5d msecs):",INTERVAL2);
  print_at(4,6,"Timer 3 (%5d msecs):",INTERVAL3);
  print_at(4,7,"Timer 4 (%5d msecs):",INTERVAL4);

  //---TIMER 2----
  status = wtc_subscribe( WT_FACILITY_TIMER2, rearm2, action2);
  if( status != WT_SUCCESS ) exit(status);
  //---TIMER 3----
  status = wtc_subscribe( WT_FACILITY_TIMER3, rearm3, action3);
  if( status != WT_SUCCESS ) exit(status);
  //---TIMER 4----
  status = wtc_subscribe( WT_FACILITY_TIMER4, rearm4, action4);
  if( status != WT_SUCCESS ) exit(status);
  for(;;)  {
    unsigned int facility;
    int sub_status;
    void* userpar;
    status = wtc_wait(&facility, &userpar, &sub_status);
    lib_rtl_output(LIB_RTL_ALWAYS,"Exited WAIT>>>> Facility = %d Status=%d Sub-Status = %d\n", 
		   facility, status, sub_status);
  }
}
