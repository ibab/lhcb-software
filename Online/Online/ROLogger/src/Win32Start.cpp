// $Id: Win32Start.cpp,v 1.1 2008-05-23 21:56:11 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: Start windows error logger from DOS command line
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/Win32Start.cpp,v 1.1 2008-05-23 21:56:11 frankb Exp $
#ifdef _WIN32

#include <windows.h>
#include <iostream>
#include <string>
#include "RTL/rtl.h"

#define FOREGROUND_BLACK 0
#define FOREGROUND_WHITE 15
#define BACKGROUND_WHITE (15<<4)
#define BACKGROUND_BLACK 0

void startProcess(const std::string& utgid, const std::string& cmd, STARTUPINFO& si)  {
  PROCESS_INFORMATION pi;
  ::memset( &pi, 0, sizeof(pi) );
  ::SetEnvironmentVariable("UTGID",utgid.c_str());
  if( !::CreateProcess(NULL, // No module name (use command line). 
    (char*)cmd.c_str(),           // Command line. 
    NULL,                  // Process handle not inheritable. 
    NULL,                  // Thread handle not inheritable. 
    FALSE,                 // Set handle inheritance to FALSE. 
    CREATE_NEW_CONSOLE,    // No creation flags. 
    NULL,                  // Use parent's environment block. 
    NULL,                  // Use parent's starting directory. 
    &si,                   // Pointer to STARTUPINFO structure.
    &pi )                  // Pointer to PROCESS_INFORMATION structure.
    ) 
  {
    std::cout << "CreateProcess [" << utgid << "] " << cmd << " failed."  << std::endl;
    ::exit(0);
  }
  // Close process and thread handles. 
  CloseHandle( pi.hProcess );
  CloseHandle( pi.hThread );
}

static void help()  {
  std::cout <<    
"   start_rologger_win32 <partition> -option [-option]                    \n"
"                                                                         \n"
"   bu[ffer]=<number>         History buffer size in lines.               \n"
"   ms[croll]=<number>        Message window scroll size in lines         \n"
"   hs[croll]=<number>        History window scroll size in lines         \n"
"   hi[storyheight]=<number>  Height of history window in pixels          \n"
"   co[ntrolswidth]=<number>  Width of controls window in pixels          \n"
"   le[ftmargin]=<number>     Margin to left of desktop in pixels         \n"
"   ri[ghtmargin]=<number>    Margin to right of desktop in pixels        \n"
"   to[pmargin]=<number>      Margin to top of desktop in pixels          \n"
"   bo[ttommargin]=<number>   Margin to bottom of desktop in pixels       \n"
"                                                                         \n"
"                     D E S K T O P      M A R G I N S                    \n"
"   +--------------------------------------------------------------------+\n"
"   |         top margin [-to]                                           |\n"
"   | left    +-----------------------------------------------+  right   |\n"
"   | margin  |                                               |  margin  |\n"
"   | [-le]   |                                               |  [-ri]   |\n"
"   |         |                                               |          |\n"
"   |         |       M E S S A G E    W I N D O W            |          |\n"
"   |         |                                               |          |\n"
"   |         |                                               |          |\n"
"   |         +--------------------------+--------------------+          |\n"
"   |         |  H I S T O R Y   ^       |  C O N T R O L S   |          |\n"
"   |         |  W I N D O W   history   |  W I N D O W       |          |\n"
"   |         |                height    |                    |          |\n"
"   |         |                  v       | <-controls width-> |          |\n"
"   |         +--------------------------+--------------------+          |\n"
"   |         bottom margin  [-bo]                                       |\n"
"   +--------------------------------------------------------------------+\n"
 << std::endl;
}

extern "C" start_rologger_win32(int argc, char** argv)  {
  RTL::CLI cli(argc, argv, help);
  int hist_buff     = 500000;
  int history_height= 400;
  int ctrl_width    = 450;
  int title_size    = 20;
  int right_margin  = 60;
  int bottom_margin = 30;
  int top_margin    = 0;
  int left_margin   = 0;
  int his_scroll    = 1500;
  int msg_scroll    = 5000;
  cli.getopt("historyheight", 2,  history_height);
  cli.getopt("controlswidth", 2,  ctrl_width);
  cli.getopt("rightmargin",   2,  right_margin);
  cli.getopt("bottommargin",  2,  bottom_margin);
  cli.getopt("topmargin",     2,  top_margin);
  cli.getopt("leftmargin",    2,  left_margin);
  cli.getopt("buffer",        2,  hist_buff);
  cli.getopt("hscroll",       2,  his_scroll);
  cli.getopt("mscroll",       2,  msg_scroll);

  HWND h = ::GetDesktopWindow();
  if ( 0 != h )  {
    RECT rc;
    if ( 0 != ::GetWindowRect(h, &rc) )   {
      //    std::cout << rc.left << " " << rc.top << " " << rc.right << " " << rc.bottom << std::endl;
      const char* partition = "LHCb";
      int pid = ::lib_rtl_pid();
      char utgid[128], comm[255], title[255], node[128];
      const char* logger = "gentest.exe ROLogger";
      STARTUPINFO si;

      rc.right  -= right_margin;
      rc.bottom -= bottom_margin;

      ::memset( &si, 0, sizeof(si) );
      si.cb = sizeof(si);

      ::sprintf(node,"%s",RTL::nodeNameShort().c_str());

      ::sprintf(title,"%s: Error Logger display",partition);
      ::sprintf(utgid,"%s_ErrorLog_%d_display",node,pid);
      ::sprintf(comm, "%s  romon_logger -service=%s -col -buff=0 -disp",logger,utgid);
      si.lpTitle        = title;
      si.dwX            = left_margin;
      si.dwY            = top_margin;
      si.dwXSize        = rc.right-left_margin;
      si.dwYSize        = rc.bottom-top_margin-history_height;
      si.dwXCountChars  = 0;          // Scroll size horizontal
      si.dwYCountChars  = msg_scroll; // Scroll size vertical
      si.dwFillAttribute=FOREGROUND_WHITE+BACKGROUND_BLACK;
      si.dwFlags        =STARTF_USESIZE+STARTF_USEPOSITION+STARTF_USEFILLATTRIBUTE+STARTF_USECOUNTCHARS;
      startProcess(utgid,comm,si);

      ::sprintf(title,"%s: Error Logger history",partition);
      ::sprintf(utgid,"%s_ErrorLog_%d_history",node,pid);
      ::sprintf(comm, "%s romon_logger -service=%s -col -buff=%d -disp",logger,utgid,hist_buff);
      si.lpTitle        = title;
      si.dwX            = left_margin;
      si.dwY            = rc.bottom-history_height+title_size;
      si.dwXSize        = rc.right-left_margin-ctrl_width;
      si.dwYSize        = history_height-2*title_size;
      si.dwYCountChars  = his_scroll; // Scroll size vertical
      si.dwFillAttribute=FOREGROUND_WHITE+BACKGROUND_BLACK;
      si.dwFlags        =STARTF_USESIZE+STARTF_USEPOSITION+STARTF_USEFILLATTRIBUTE+STARTF_USECOUNTCHARS;
      startProcess(utgid,comm,si);

      ::Sleep(2000);
      ::sprintf(title,"%s: Error Logger control",partition);
      ::sprintf(utgid,"%s_ErrorLog_%d",node,pid);
      ::sprintf(comm, "gentest.exe ROLogger romon_display_server -service=%s",partition);
      si.lpTitle        = title;
      si.dwX            = rc.right-ctrl_width+title_size;
      si.dwY            = rc.bottom-history_height+title_size;
      si.dwXSize        = rc.right-si.dwX+title_size;
      si.dwYSize        = history_height-2*title_size;
      si.dwYCountChars  = 0; // Scroll size vertical
      si.dwFillAttribute=FOREGROUND_BLACK|BACKGROUND_WHITE;
      si.dwFlags        =STARTF_USESIZE+STARTF_USEPOSITION+STARTF_USEFILLATTRIBUTE+STARTF_USECOUNTCHARS;
      startProcess(utgid,comm,si);
    }
  }
  return 1;
}
#endif
