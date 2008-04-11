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

CPU::CPU() {
  reset();
}

CPU* CPU::reset() {
  ::memset(this,0,sizeof(CPU));
  return this;
}

CPUset::CPUset() {
  reset();
}

CPUset* CPUset::reset() {
  ::memset(this,0,sizeof(CPUset));
  return this;
}

CPUfarm* CPUfarm::reset() {
  ::memset(this,0,sizeof(CPUfarm));  
  type = TYPE;
  return this;
}

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

