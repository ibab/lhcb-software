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

static void _the_printer(void*, const char* text)  {
  ::fprintf(stdout,text);
  ::fflush(stdout);
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
  //print_lvl = level;
}

/// Calls the display action
Status TypedObject::display(int severity, const char* fmt...)  {
  if ( severity > print_lvl ) {                  // receives:
    va_list args;                                // - the log level
    va_start( args, fmt);                        // - a standard C formatted 
    char str[4096];                              //   string (like printf)
    size_t il  = 0;//sprintf(str,"%s::",RTL::processName().c_str());
    size_t len = vsprintf(str+il,fmt,args);
    va_end (args);
    str[il+len]   = '\n';
    str[il+len+1] = '\0';
    print_func(print_arg,str);
  }
  return FSM::SUCCESS;
}
