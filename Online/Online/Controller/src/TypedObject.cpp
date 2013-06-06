/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
#include "FiniteStateMachine/TypedObject.h"
#include <cstdarg>
#include <cstdio>

using namespace FiniteStateMachine;
using namespace std;

/// Class Constructor
TypedObject::TypedObject(const Type *typ,const string& nam)
  : m_name(nam), m_type(typ)
{
}

/// Standatrd destructor  
TypedObject::~TypedObject()    {
}

static size_t _the_printer(void*, int, const char* src, const char* text)  {
  size_t len = ::fputs(src,stdout);
  len += fputs(": ",stdout);
  len += fputs(text,stdout);
  ::fflush(stdout);
  return len;
}

static TypedObject::dump_function_t print_func = _the_printer;
static void* print_arg = 0;
static int   print_lvl = 0;

/// Set new print level. Returns the old print level
int TypedObject::setPrintLevel(int new_level)    {
  int old_level = print_lvl;
  print_lvl = new_level;
  return old_level;
}

void TypedObject::setPrinter(void* arg, dump_function_t fcn)  {
  print_func = fcn ? fcn : _the_printer;
  print_arg = arg;
}

/// Calls the display action
Status TypedObject::display(int severity, const char* src, const char* fmt...)  {
  if ( severity >= print_lvl ) {                  // receives:
    va_list args;                                // - the log level
    va_start( args, fmt);                        // - a standard C formatted 
    char str[4096];                              //   string (like printf)
    size_t len = vsnprintf(str,sizeof(str)-2,fmt,args);
    va_end (args);
    str[len]   = '\n';
    str[len+1] = '\0';
    print_func(print_arg,severity,src,str);
  }
  return FSM::SUCCESS;
}
