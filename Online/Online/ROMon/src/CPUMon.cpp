#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "ROMon/CPUMon.h"
#include "ROMon/ROMonGblBuffer.h"

using namespace ROMon;

std::ostream& operator<<(std::ostream& os, const CPU& c) {
  char text[132];
  sprintf(text,"    Clock:%5.0f Cache:%5d kB Bogo:%4.2f User:%4.1f%% Sys:%4.1f%% IO:%4.1f%% irq:%4.1f%% sirq:%4.1f%%",
	  c.clock,c.cache,c.bogomips,c.stats.user,c.stats.system,c.stats.iowait,c.stats.IRQ,c.stats.softIRQ);
  return os << text;
}

std::ostream& operator<<(std::ostream& os, const CPUset& s) {
  os << "  Node:"  << s.name << " Family:" << s.family << " ContextRate:" << s.ctxtRate 
     << " Last update:" << s.time << "." << s.millitm << std::endl;
  for(CPUset::Cores::const_iterator c=s.cores.begin(); c!=s.cores.end(); c=s.cores.next(c))
    os << "   -> " << *c << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const CPUfarm& f) {
  os << "Farm:"  << f.name << " " << f.nodes.size() << " nodes. Last update:" << f.time << std::endl;
  for(CPUfarm::Nodes::const_iterator n=f.nodes.begin(); n!=f.nodes.end(); n=f.nodes.next(n))
    os << *n << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const Process& p) {
  char text[132];
  sprintf(text,"    PID:%5d %5d %3.0f %3.0f %1d %-16s %s",
	  p.pid,p.ppid,p.cpu,p.mem,p.threads,p.owner,p.utgid);
  return os << text;
}

std::ostream& operator<<(std::ostream& os, const Procset& s) {
  os << "  Node:"  << s.name << " " << s.processes.size() 
     << " Last update:" << s.time << "." << s.millitm << std::endl;
  for(Procset::Processes::const_iterator c=s.processes.begin(); c!=s.processes.end(); c=s.processes.next(c))
    os << "   -> " << *c << std::endl;
  return os;
}

/// Empty constructor
CPU::CPU() {
  reset();
}

/// Reset data content
CPU* CPU::reset() {
  ::memset(this,0,sizeof(CPU));
  return this;
}

/// Default constructor
CPUset::CPUset() {
  reset();
}

/// Reset object structure
CPUset* CPUset::reset() {
  ::memset(this,0,sizeof(CPUset));
  return this;
}

/// Reset object structure
CPUfarm* CPUfarm::reset() {
  ::memset(this,0,sizeof(CPUfarm));  
  type = TYPE;
  return this;
}

/// Fix-up object sizes
void CPUfarm::fixup() {
  type = TYPE;
  totalSize = length();
}

/// Retrieve timestamp of earliest updated node
CPUfarm::TimeStamp CPUfarm::firstUpdate() const {
  bool has_nodes = false;
  TimeStamp t(INT_MAX,999);
  for(Nodes::const_iterator n=nodes.begin(); n!=nodes.end(); n=nodes.next(n))  {
    has_nodes = true;
    if ( (*n).time < t.first ) {
      t.first = (*n).time;
      t.second = (*n).millitm;
    }
    else if ( (*n).time == t.first && (*n).millitm < t.second ) {
      t.second = (*n).millitm;
    }
  }
  return has_nodes ? t : TimeStamp(0,0);
}

/// Retrieve timestamp of most recent updated node
CPUfarm::TimeStamp CPUfarm::lastUpdate() const {
  bool has_nodes = false;
  TimeStamp t(0,0);
  for(Nodes::const_iterator n=nodes.begin(); n!=nodes.end(); n=nodes.next(n))  {
    has_nodes = true;
    if ( (*n).time > t.first ) {
      t.first = (*n).time;
      t.second = (*n).millitm;
    }
    else if ( (*n).time == t.first && (*n).millitm > t.second ) {
      t.second = (*n).millitm;
    }
  }
  return has_nodes ? t : TimeStamp(0,0);
}

/// Empty constructor
Process::Process() {
  reset();
}

/// Reset data content
Process* Process::reset() {
  ::memset(this,0,sizeof(Process));
  return this;
}

/// Default constructor
Procset::Procset() {
  reset();
}

/// Reset object structure
Procset* Procset::reset() {
  ::memset(this,0,sizeof(Procset));
  return this;
}

