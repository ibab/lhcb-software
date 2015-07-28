// $Id: HelpDisplay.cpp,v 1.1 2010-09-20 19:00:10 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/HelpDisplay.cpp,v 1.1 2010-09-20 19:00:10 frankb Exp $

// Framework include files
#include "SCR/scr.h"
#include "ROMon/HelpDisplay.h"
#include "ROMonDefs.h"

// C++ include files
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace ROMon;
using namespace SCR;
using namespace std;

// Initializing constructor with default file
HelpDisplay::HelpDisplay(InternalDisplay* parent, const string& title, const string& tag) 
  : InternalDisplay(parent,title)
{
  string input = ::getenv("ROMONROOT") != 0 ? ::getenv("ROMONROOT") : "..";
  string fin = input+"/doc/farmMon.hlp";
  _init(tag,fin);
}

// Initializing constructor with file name
HelpDisplay::HelpDisplay(InternalDisplay* parent, const string& title, const string& tag, const std::string& fin) 
  : InternalDisplay(parent,title)
{
  _init(tag,fin);
}

// Initialize help display with data content
void HelpDisplay::_init(const std::string& tag, const std::string& fin)
{
  bool use = false, isHeader=false;
  string s, start="<"+tag+">", stop = "</"+tag+">";
  string head = m_title + ": " + fin;
  ifstream in(fin.c_str());

  ::scrc_create_display(&m_display,60,132,NORMAL,ON,head.c_str());
  ::scrc_put_chars(m_display,"Hit CTRL-H to hide the display",BOLD,2,2,1);
  for(int line=3; in.good(); ) {
    getline(in,s);
    if ( !use && (s.find(start) != string::npos || s.find("<common>") != string::npos) ) {
      isHeader = true;
      use = true;
      continue;
    }
    if ( use && (s.find(stop) != string::npos || s.find("</common>") != string::npos) ) {
      use = false;
    }
    if ( use ) {
      ::scrc_put_chars(m_display,s.c_str(),isHeader ? BOLD: NORMAL,++line,2,1);
      isHeader = false;
    }
  }
  ::scrc_set_border(m_display,head.c_str(),INVERSE|BLUE);
}
