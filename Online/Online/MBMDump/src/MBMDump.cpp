// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/MBMDump.cpp,v 1.10 2010-08-31 16:02:34 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: MBMDump.cpp,v 1.10 2010-08-31 16:02:34 frankb Exp $
//
// C++ include files
#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"
#include "RTL/rtl.h"
#include <cctype>
#include <string.h>

const char* MBMDump::procName()  {  
  return RTL::processName().c_str();    
}

void MBMDump::rconv(char c[], int word)   {
  for(int i=0;i<4;i++){
    //c[3-i] = (word>>(i*8) & 0XFF);
    //if(!isalnum(c[3-i]))c[3-i] = '.';
    c[i] = char((word>>(i*8) & 0XFF));
    if(!isalnum(c[i]))  {
      if ( isspace(c[i]) ) c[i] = ' ';
      else if ( ispunct(c[i]) );
      else c[i] = '.';
    }
  }
}

static void help()  {
    ::lib_rtl_output(LIB_RTL_ALWAYS,"mbmdump -opt [-opt]");
    ::lib_rtl_output(LIB_RTL_ALWAYS,"    -d(ebug)               Invoke debugger.");
}

static size_t print_msg(void* /* env */, int /* lvl */, const char* fmt, va_list args)  {
  char buff[1024];
  size_t result = vsprintf(buff,fmt,args);
  char* p = ::strchr(buff,'\n');
  if ( p && p < buff+sizeof(buff) ) *p = 0;
  //::upic_write_message(buff,"");
  return result;
}

extern "C" int mbmdump(int argc,char *argv[])   {
  using namespace MBMDump;
  RTL::CLI cli(argc, argv, help);
  bool debug = cli.getopt("debug",1) != 0;
  bool files = cli.getopt("files",1) != 0;
  if ( debug ) lib_rtl_start_debugger();
  int status = ::upic_attach_terminal();
  if(status != UPI_NORMAL)
    exit(EXIT_FAILURE);

  ::lib_rtl_install_printer(print_msg,0);
  ::upic_change_pasteboard(132,0);
  if ( files )  {
    FileMainMenu m;
    return m.run();
  }
  MBMMainMenu m;
  return m.run();
}

extern "C" int mbm_dump(int argc,char *argv[])   {
  return mbmdump(argc,argv);
}
