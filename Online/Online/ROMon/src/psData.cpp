// $Id: psData.cpp,v 1.2 2008-10-21 13:53:52 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/psData.cpp,v 1.2 2008-10-21 13:53:52 frankb Exp $

// C++ include files
#include <cstring>
#include <iostream>

// Framework includes
#include "ROMon/CPUMon.h"

using namespace std;
#include "ROMonDefs.h"
using namespace ROMon;

#define defineItem(x) { const char* x = ptr; ptr = strchr(x,'\0')+1; res.push_back(x); }
vector<string> ROMon::psItems(const char*& ptr) {
  vector<string> res;
  res.reserve(40);
  defineItem(utgid);
  defineItem(tid);
  defineItem(tgid);
  defineItem(ppid);
  defineItem(pgid);
  defineItem(nlwp);
  defineItem(usr);
  defineItem(grp);
  defineItem(tty);
  defineItem(cmd);
  defineItem(sch);
  defineItem(prio);
  defineItem(rtprio);
  defineItem(nice);
  defineItem(psr);
  defineItem(stat);
  defineItem(cpu);
  defineItem(mem);
  defineItem(vsize);
  defineItem(lock);
  defineItem(rss);
  defineItem(data);
  defineItem(stack);
  defineItem(exe);
  defineItem(lib);
  defineItem(share);
  defineItem(ign);
  defineItem(pend);
  defineItem(catched);
  defineItem(block);
  defineItem(start);
  defineItem(elaps);
  defineItem(cputim);
  defineItem(cmdline);
  return res;
}
#undef defineItem

#define defineItem(x) res.push_back(#x);
std::vector<std::string> ROMon::psLabels() {
  vector<string> res;
  res.reserve(40);
  defineItem(utgid);
  defineItem(tid);
  defineItem(tgid);
  defineItem(ppid);
  defineItem(pgid);
  defineItem(nlwp);
  defineItem(usr);
  defineItem(grp);
  defineItem(tty);
  defineItem(cmd);
  defineItem(sch);
  defineItem(prio);
  defineItem(rtprio);
  defineItem(nice);
  defineItem(psr);
  defineItem(stat);
  defineItem(cpu);
  defineItem(mem);
  defineItem(vsize);
  defineItem(lock);
  defineItem(rss);
  defineItem(data);
  defineItem(stack);
  defineItem(exe);
  defineItem(lib);
  defineItem(share);
  defineItem(ign);
  defineItem(pend);
  defineItem(catched);
  defineItem(block);
  defineItem(start);
  defineItem(elaps);
  defineItem(cputim);
  defineItem(cmdline);
  return res;
}
#undef defineItem
