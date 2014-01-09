// $Id: Win32Start.cpp,v 1.6 2008-10-21 13:53:52 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/Win32Start.cpp,v 1.6 2008-10-21 13:53:52 frankb Exp $

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <cstring>
#include <string>
#include "RTL/rtl.h"
using namespace std;

#define FOREGROUND_BLACK 0
#define FOREGROUND_WHITE 15
#define BACKGROUND_WHITE (15<<4)
#define BACKGROUND_BLACK 0

#ifndef _WIN32
#define STARTF_USESIZE          (1<<0)
#define STARTF_USEPOSITION      (1<<1)
#define STARTF_USEFILLATTRIBUTE (1<<2)
#define STARTF_USECOUNTCHARS    (1<<3)

struct STARTUPINFO {
  const char* lpTitle;
  int         dwX;
  int         dwY;
  int         dwXSize;
  int         dwYSize;
  int         dwXCountChars;
  int         dwYCountChars;
  char        dwFillAttribute;
  int         dwFlags;
  size_t      cb;
};

string getColor(int code) {
  switch(code) {
  case 0:  return "black";
  case 1:  return "blue";
  case 2:  return "green";
  case 3:  return "cyan";
  case 4:  return "red";
  case 5:  return "magenta";
  case 6:  return "brown";
  case 7:  return "lightgrey";
  case 8:  return "darkgrey";
  case 9:  return "lightblue";
  case 10: return "lightgreen";
  case 11: return "lightcyan";
  case 12: return "lightred";
  case 13: return "lightmagenta";
  case 14: return "yellow";
  case 15: return "white";
  }
  return "white";
}

#endif

static void startTerminal(const string& utgid, const string& cmd, STARTUPINFO& si)  {
#ifdef _WIN32
  PROCESS_INFORMATION pi;
  ::memset( &pi, 0, sizeof(pi) );
  if ( 0 == ::SetEnvironmentVariable("UTGID",utgid.c_str()) ) {
    cout << "Failed to set environment: UTGID=" << utgid << endl;
  }
  if( !::CreateProcess(NULL,    // No module name (use command line). 
    (char*)cmd.c_str(),         // Command line. 
    NULL,                       // Process handle not inheritable. 
    NULL,                       // Thread handle not inheritable. 
    FALSE,                      // Set handle inheritance to FALSE. 
    CREATE_NEW_CONSOLE,         // No creation flags. 
    NULL,                       // Use parent's environment block. 
    NULL,                       // Use parent's starting directory. 
    &si,                        // Pointer to STARTUPINFO structure.
    &pi )                       // Pointer to PROCESS_INFORMATION structure.
    ) 
  {
    cout << "CreateProcess [" << utgid << "] " << cmd << " failed."  << endl;
    ::exit(0);
  }
  // Close process and thread handles. 
  CloseHandle( pi.hProcess );
  CloseHandle( pi.hThread );
#else
  char txt[128];
  int font_height=13;
  int font_width=6;
  string env, command = "xterm -ls -132 ";
  sprintf(txt,"-fn %dx%d -fb %dx%db ",font_width,font_height,font_width,font_height);
  command += txt;
  if ( si.dwFlags&STARTF_USESIZE && si.dwFlags&STARTF_USEPOSITION )
    ::sprintf(txt,"-geometry %dx%d+%d+%d ",int(si.dwXSize/font_width),int(si.dwYSize/font_height),si.dwX,si.dwY);
  else if ( si.dwFlags&STARTF_USESIZE )
    ::sprintf(txt,"-geometry %dx%d ",int(si.dwXSize/font_width),int(si.dwYSize/font_height));
  else if ( si.dwFlags&STARTF_USEPOSITION )
    ::sprintf(txt,"-geometry +%d+%d ",si.dwX,si.dwY);
  else
    memset(txt,0,sizeof(txt));
  command += txt;
  if ( si.dwFlags&STARTF_USEFILLATTRIBUTE ) {
    int fg = 0xF&si.dwFillAttribute;
    int bg = si.dwFillAttribute>>4;
    command += "-fg " + getColor(fg) + " -bg " + getColor(bg) + " ";
  }
  if ( si.dwFlags&STARTF_USECOUNTCHARS ) {
    if ( si.dwYCountChars > 0 )
      ::sprintf(txt,"-sl %d ",si.dwYCountChars);
    else
      ::sprintf(txt,"+sb ");
    command += txt;
  }
  if ( si.lpTitle ) {
    command += " -title \"";
    command += si.lpTitle;
    command += "\" ";
  }
  if ( !cmd.empty() ) {
    command += " -e \"";
    if ( utgid.empty() )
      command += "exec "+cmd;
    else
      command += "export UTGID="+utgid+"; exec -a \\${UTGID} "+cmd;
    command += "\"";
  }
  command += "&";
  // cout << command << endl;
  env = "UTGID="+utgid;
  if ( 0 != ::putenv((char*)env.c_str()) ) {
    cout << "Failed to set environment: UTGID=" << utgid << endl;
  }
  ::system(command.c_str());
#endif
}

static pair<int,int> getDesktopGeometry(const string& geometry) {
  if ( geometry.empty() ) {
#ifdef _WIN32
    HWND h = ::GetDesktopWindow();
    if ( 0 != h )  {
      RECT rc;
      if ( 0 != ::GetWindowRect(h, &rc) )   {
        return make_pair(rc.right,rc.bottom);
      }
    }
#endif
  }
  else {
    pair<int,int> rc;
    if ( 2 == ::sscanf(geometry.c_str(),"%dx%d",&rc.first,&rc.second) ) {
      return rc;
    }
  }
  return make_pair(0,0);
}

static void help()  {
  cout <<    
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
    << endl;
}

extern "C" int start_rologger_win32(int argc, char** argv)  {
  RTL::CLI cli(argc, argv, help);
  string part_name = "LHCb";  
  string geometry;
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
  cli.getopt("partition",     2,  part_name);
  cli.getopt("geometry",      2,  geometry);
#ifndef _WIN32
  const char* logger = "`which gentest.exe` libROLogger.so";
  if ( geometry.empty() ) geometry  = "1280x1024";
#else
  const char* logger = "gentest.exe ROLogger";
#endif
  pair<int,int> dt = getDesktopGeometry(geometry);
  if ( dt.first && dt.second ) {
    const char* partition = part_name.c_str();
    int pid = ::lib_rtl_pid();
    int dt_width  = dt.first- right_margin;
    int dt_height = dt.second - bottom_margin;
    char utgid[128], comm[255], title[255], node[128];
    STARTUPINFO si;
    ::memset( &si, 0, sizeof(si) );
    si.cb = sizeof(si);

    ::sprintf(node,"%s",RTL::nodeNameShort().c_str());

    ::sprintf(title,"%s: Error Logger display",partition);
    ::sprintf(utgid,"%s_ErrLog_%d_display",node,pid);
    ::sprintf(comm, "%s  romon_logger -service=%s -col -buff=0 -disp",logger,utgid);
    si.lpTitle         = title;
    si.dwX             = left_margin;
    si.dwY             = top_margin;
    si.dwXSize         = dt_width-left_margin;
    si.dwYSize         = dt_height-top_margin-history_height;
    si.dwXCountChars   = 0;          // Scroll size horizontal
    si.dwYCountChars   = msg_scroll; // Scroll size vertical
    si.dwFillAttribute = (char)(FOREGROUND_WHITE+BACKGROUND_BLACK);
    si.dwFlags         = (int)(STARTF_USESIZE+STARTF_USEPOSITION+STARTF_USEFILLATTRIBUTE+STARTF_USECOUNTCHARS);
    startTerminal(utgid,comm,si);

    ::sprintf(title,"%s: Error Logger history",partition);
    ::sprintf(utgid,"%s_ErrLog_%d_history",node,pid);
    ::sprintf(comm, "%s romon_logger -service=%s -col -buff=%d",logger,utgid,hist_buff);
    si.lpTitle         = title;
    si.dwX             = left_margin;
    si.dwY             = dt_height-history_height+title_size;
    si.dwXSize         = dt_width-left_margin-ctrl_width;
    si.dwYSize         = history_height-2*title_size;
    si.dwYCountChars   = his_scroll; // Scroll size vertical
    si.dwFillAttribute = (char)(FOREGROUND_WHITE+BACKGROUND_BLACK);
    si.dwFlags         = (int)(STARTF_USESIZE+STARTF_USEPOSITION+STARTF_USEFILLATTRIBUTE+STARTF_USECOUNTCHARS);
    startTerminal(utgid,comm,si);

    ::lib_rtl_sleep(2000);
    ::sprintf(title,"%s: Error Logger control",partition);
    ::sprintf(utgid,"%s_ErrLog_%d",node,pid);
    ::sprintf(comm, "%s romon_display_server -service=%s",logger,partition);
    si.lpTitle         = title;
    si.dwX             = dt_width-ctrl_width+title_size;
    si.dwY             = dt_height-history_height+title_size;
    si.dwXSize         = dt_width-si.dwX+title_size;
    si.dwYSize         = history_height-2*title_size;
    si.dwYCountChars   = 0; // Scroll size vertical
    si.dwFillAttribute = (char)(FOREGROUND_BLACK|BACKGROUND_WHITE);
    si.dwFlags         = (int)(STARTF_USESIZE+STARTF_USEPOSITION+STARTF_USEFILLATTRIBUTE+STARTF_USECOUNTCHARS);
    startTerminal(utgid,comm,si);
    return 1;
  }
  cout << "Failed to determine desktop geometry." << endl;
  return 1;
}
