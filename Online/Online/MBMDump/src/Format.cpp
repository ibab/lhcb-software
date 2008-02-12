// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/Format.cpp,v 1.3 2008-02-12 17:15:24 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: Format.cpp,v 1.3 2008-02-12 17:15:24 frankb Exp $
//
// C++ include files
#include "MBMDump/MBMDump.h"
using namespace MBMDump;
#define s_FMT_HEX08    " %08X "
#define s_FMT_DEC10      "%10d"

Format::Format()  {
  words_per_line  = 4;
  column_one_flag = 1;
  ascii_flag      = true;
  // default all words to hex....in case of dud string
  for(int i=0;i<MAX_WORDS_PER_LINE;i++)
    fmt[i] = s_FMT_HEX08;
}

const char* Format::FMT_HEX08()  {  
  return  s_FMT_HEX08;
}
const char* Format::FMT_DEC10()  {  
  return  s_FMT_DEC10;
}
