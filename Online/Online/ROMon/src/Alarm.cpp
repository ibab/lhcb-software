// $Id: Alarm.cpp,v 1.5 2009-06-03 10:07:13 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/Alarm.cpp,v 1.5 2009-06-03 10:07:13 frankb Exp $

// Framework include files
#include "ROMon/Alarm.h"
#include "SCR/scr.h"

// C++ include files
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iomanip>

using namespace ROMon;
using namespace SCR;
using namespace std;

Alarm::Alarm(const string& txt) : AlarmTag(0,"","") {
  this->fromString(txt);
}

unsigned int Alarm::hash32(const char* key) {
  unsigned int hash;
  const char* k;
  for (hash = 0, k = key; *k; k++) {
    hash += *k; hash += (hash << 10); hash ^= (hash >> 6); 
  }
  hash += (hash << 3); hash ^= (hash >> 11); hash += (hash << 15);
  return hash;
}

string Alarm::toString(const string& t)  const {
  stringstream s;
  tag = t;
  char txt[32];
  ::sprintf(txt,"#%08lx#%08x#",when,code);
  s << t << txt << subfarm << "#" << node << "#" << description << "#" << optional;
  return s.str();
}

void Alarm::fromString(const string& str) {
  string copy(str);
  char *p1, *p2, *p3, *p4, *p5, *p6, *s = (char*)copy.c_str();
  if ( (p1=::strchr(s,'#')) ) {
    *p1 = 0;
    if ( (p2=::strchr(p1+1,'#')) ) {
      *p2 = 0;
      if ( (p3=::strchr(p2+1,'#')) ) {
        *p3 = 0;
        if ( (p4=::strchr(p3+1,'#')) ) {
          *p4 = 0;
          if ( (p5=::strchr(p4+1,'#')) ) {
            *p5 = 0;
            if ( (p6=::strchr(p5+1,'#')) ) {
              unsigned long tmp;
              *p6 = 0;
              tag = s;
              ::sscanf(p1+1,"%08lx",&tmp);
              when = time_t(tmp);
              ::sscanf(p2+1,"%08lx",&tmp);
              code = tmp;
              subfarm = p3+1;
              node = p4+1;
              description = p5+1;
              optional = p6+1;
            }
          }
        }
      }
    }
  }
}

int Alarm::level() const {
  static int mask = ERR_LVL_MONITOR|ERR_LVL_WARNING|ERR_LVL_ERROR|ERR_LVL_ALARM;
  return code&mask;
}

int Alarm::color() const {
  int color = NORMAL;
  if ( code&ERR_LVL_MONITOR ) color = NORMAL;
  if ( code&ERR_LVL_WARNING ) color = YELLOW;
  if ( code&ERR_LVL_ERROR   ) color = RED|BOLD;
  if ( code&ERR_LVL_ALARM   ) color = RED|INVERSE|BOLD;
  return color;
}

const char* Alarm::message() const {
  static char gtxt[256];
  switch(code) {
  case ERR_NO_ERROR:              return "No obvious Errors detected... ";
  case ERR_NO_UPDATES:            return "No update information available ";
  case ERR_NOT_USED:              return "Node not used.... ";
  case ERR_SLOTS_LIMIT:           return "MBM buffers: SLOTS at limit ";
  case ERR_SPACE_LIMIT:           return "MBM buffers: SPACE at limit ";
  case ERR_NODE_STUCK:            return "MBM buffers at limit ";
  case ERR_MEPRX_MISSING:         return "Event builder task MEPRX dead ";
  case ERR_MEPRX_STUCK:           return "Event builder task MEPRx stuck ";
  case ERR_RECEIVER_MISSING:      return "Event receiver dead ";
  case ERR_RECEIVER_STUCK:        return "Event receiver stuck ";
  case ERR_MONITOR_MISSING:       return "Event monitor dead ";
  case ERR_MONITOR_STUCK:         return "Event monitor stuck ";
  case ERR_SENDER_MISSING:        return "Sender dead ";
  case ERR_SENDER_STUCK:          return "Sender stuck ";
  case ERR_MOORE_MISSING:         return "Event filter task MOORE dead ";
  case ERR_MOORE_STUCK:           return "Event filter task MOORE stuck ";
  case ERR_NODAQ_ACTIVITY:        return "No DAQ activity visible ";
  case ERR_NOHLT_ACTIVITY:        return "No HLT activity visible ";
  case ERR_NOSTORAGE_ACTIVITY:    return "No STORAGE activity visible ";
  case ERR_NOMONITORING_ACTIVITY: return "No MONITORING activity visible ";
  case ERR_REMOVED:               return "Alarm value became normal. Alarm removed";
  case ERR_REMOVEDALL:            return "All alarms cleared";
  case ERR_NODE_DEAD:             return "The controls infrastructure cannot connect to this node ";
  case ERR_NO_PVSS_PROJECT:       return "The following PVSS project is not running ";
  case ERR_NO_CONNECTIONS:        return "This node cannot reach the required network hosts ";
  case ERR_CONNECTION_MISSING:    return "A network connection from this node is missing ";
  case ERR_NO_TASKS:              return "None of the required tasks were found on this node ";
  case ERR_TASK_MISSING:          return "Task is missing ";
  default:
    ::sprintf(gtxt,"Unknown Alarm - Never heard of this error code [%08X] ",int(code));
    return gtxt;
  }
}

string Alarm::time() const {
  char txt[32];
  ::strftime(txt,sizeof(txt),"%H:%M:%S",::localtime(&when));
  return txt;
}

ostream& operator<<(ostream& os, const Alarm& a) {
  os << setw(13) << left << a.node << left << setw(12) << a.time() << " " << a.message() << a.description;
  return os;
}
