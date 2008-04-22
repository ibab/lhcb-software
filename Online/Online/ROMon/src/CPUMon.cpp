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
  sprintf(text,"    %5d %5d %4.1f %4.1f %6.0f %6.0f %4d %-16s %s",
	  p.pid,p.ppid,p.cpu,p.mem,p.vsize,p.rss,p.threads,p.owner,p.utgid);
  return os << text;
}

std::ostream& operator<<(std::ostream& os, const Procset& s) {
  char text[132];
  sprintf(text,"          %5s %5s %4s %4s %6s %6s %-4s %-16s %s","PID","PPID","%CPU","%MEM","VSize","RSS","#Thr","Owner","UTGID");
  os << "  Node:"  << s.name << " " << s.processes.size() 
     << " Last update:" << s.time << "." << s.millitm << std::endl
     << text << std::endl;
  for(Procset::Processes::const_iterator c=s.processes.begin(); c!=s.processes.end(); c=s.processes.next(c))
    os << "   -> " << *c << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const ProcFarm& f) {
  os << "Farm:"  << f.name << " " << f.nodes.size() << " nodes. Last update:" << f.time << std::endl;
  for(ProcFarm::Nodes::const_iterator n=f.nodes.begin(); n!=f.nodes.end(); n=f.nodes.next(n))
    os << *n << std::endl;
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
  return _firstUpdate(nodes);
}

/// Retrieve timestamp of most recent updated node
CPUfarm::TimeStamp CPUfarm::lastUpdate() const {
  return _lastUpdate(nodes);
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

/// Reset object structure
ProcFarm* ProcFarm::reset() {
  ::memset(this,0,sizeof(ProcFarm));  
  type = TYPE;
  return this;
}

/// Fix-up object sizes
void ProcFarm::fixup() {
  type = TYPE;
  totalSize = length();
}

/// Retrieve timestamp of earliest updated node
ProcFarm::TimeStamp ProcFarm::firstUpdate() const {
  return _firstUpdate(nodes);
}

/// Retrieve timestamp of most recent updated node
ProcFarm::TimeStamp ProcFarm::lastUpdate() const {
  return _lastUpdate(nodes);
}

