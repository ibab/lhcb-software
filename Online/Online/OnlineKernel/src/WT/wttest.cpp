/*----------------------------------------------------------------------*/
/*    ALEPH WAIT FACILITY TEST PROGRAM                                  */
/*----------------------------------------------------------------------*/
#include "WT/wtdef.h"
#include "RTL/rtl.h"
#include "RTL/screen.h"
#include <cstdlib>
#include <cstdarg>

#define INTERVAL_NORMAL 1
#define INTERVAL1 6*INTERVAL_NORMAL
#define INTERVAL2 60*INTERVAL_NORMAL
#define INTERVAL3 600*INTERVAL_NORMAL
#define INTERVAL4 6000*INTERVAL_NORMAL

static unsigned long alarm1, alarm2, alarm3, alarm4;
static int ast1_count = 0;
static int ast2_count = 0;
static int ast3_count = 0;
static int ast4_count = 0;

void print_at(int x, int y, const char* fmt, ...)  {
  va_list args;
  char buff[256];
  va_start( args, fmt );
  ::vsprintf( buff, fmt, args);
  printxy(x,y,buff);
  //lib_rtl_output(LIB_RTL_ALWAYS,buff);
  //lib_rtl_output(LIB_RTL_ALWAYS,"\n");
  refresh();
}

int ast1(void* par)  {
  ast1_count++;
  return wtc_insert(WT_FACILITY_TIMER1, par);
}

int rearm1(unsigned int /* fac */, void* /* par */)  {
  static int count = 0;
  if ( (++count%50)==0 ) print_at(40, 4, "%5d",count);
  int sc = lib_rtl_set_timer(INTERVAL1, ast1, 0, &alarm1);
  return lib_rtl_is_success(sc) ? WT_SUCCESS : WT_ERROR;
}

int action1(unsigned int /* fac */, void* /* par */) {
  static int count = 0;
  if ( (++count%50)==0 ) {
    print_at(50, 4, "%5d",ast1_count);
    print_at(30, 4, "%5d",count);
  }
  return WT_SUCCESS;
}

int ast2(void* par) {
  ast2_count++;
  return wtc_insert(WT_FACILITY_TIMER2,par);
}

int rearm2(unsigned int /* fac */, void* /* par */)  {
  static int count = 0;
  print_at(40, 5, "%5d",++count);
  return lib_rtl_set_timer(INTERVAL2, ast2, 0, &alarm2);
}

int action2(unsigned int /* fac */, void* /* par */) {
  static int count = 0;
  print_at(50, 5, "%5d",ast2_count);
  print_at(30, 5, "%5d",++count);
  return WT_SUCCESS;
}

int ast3(void* par)  {
  ast3_count++;
  return wtc_insert(WT_FACILITY_TIMER3, par);
}

int rearm3(unsigned int /* fac */, void* /* par */)  {
  static int count = 0;
  print_at(40, 6, "%5d",++count);
  return lib_rtl_set_timer(INTERVAL3, ast3, 0, &alarm3);
}

int action3(unsigned int /* fac */, void* /* par */) {
  static int count = 0;
  print_at(50, 6, "%5d",ast3_count);
  print_at(30, 6, "%5d",++count);
  return WT_SUCCESS;
}

int ast4(void* par)  {
  ast4_count++;
  return wtc_insert(WT_FACILITY_TIMER4, par);
}

int rearm4(unsigned int /* fac */, void* /* par */)  {
  static int count = 0;
  print_at(40, 7, "%5d",++count);
  return lib_rtl_set_timer(INTERVAL4, ast4, 0, &alarm4);
}

int action4(unsigned int /* fac */, void* /* par */) {
  static int count = 0;
  print_at(50, 7, "%5d",ast4_count);
  print_at(30, 7, "%5d",++count);
  return WT_SUCCESS;
}

extern "C" int wtc_test(int /* argc */, char** /* argv */)   {  
  int status = wtc_init();
  if( status != WT_SUCCESS ) exit(status);

  initscreen();      // clear + initialize the screen
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
