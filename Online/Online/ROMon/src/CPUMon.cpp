#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "ROMonDefs.h"
#include "ROMon/CPUMon.h"
#include "ROMon/ROMonGblBuffer.h"

using namespace ROMon;
using namespace std;

ostream& ROMon::operator<<(ostream& os, const ROMon::Memory& m) {
  char text[132];
  ::snprintf(text,sizeof(text),"Memory:%d Free:%d Cache:%5d Active:%d Inactive:%d",
             m.memTotal,m.memFree,m.cached,m.active,m.inactive);
  return os << text;
}

ostream& ROMon::operator<<(ostream& os, const ROMon::CPU::Stat& s) {
  char text[132];
  ::snprintf(text,sizeof(text),"User:%4.1f%% Sys:%4.1f%% IO:%4.1f%% irq:%4.1f%% sirq:%4.1f%%",
             s.user,s.system,s.iowait,s.IRQ,s.softIRQ);
  return os << text;
}
ostream& ROMon::operator<<(ostream& os, const ROMon::CPU& c) {
  char text[132];
  ::snprintf(text,sizeof(text),"    Clock:%5.0f Cache:%5d kB Bogo:%4.2f ",c.clock,c.cache,c.bogomips);
  return os << text << c.stats;
}

ostream& ROMon::operator<<(ostream& os, const ROMon::CPUset& s) {
  os << "  Node:"  << s.name << " Family:" << s.family << " ContextRate:" << s.ctxtRate 
     << " Last update:" << s.time << "." << s.millitm << endl
     << "          Memory:" << s.memory << " kB Free:" << s.memfree << " kB Usage:" << s.averages << endl;
  for(CPUset::Cores::const_iterator c=s.cores.begin(); c!=s.cores.end(); c=s.cores.next(c))
    os << "   -> " << *c << endl;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const ROMon::CPUfarm& f) {
  os << "Farm:"  << f.name << " " << f.nodes.size() << " nodes. Last update:" << f.time << endl;
  for(CPUfarm::Nodes::const_iterator n=f.nodes.begin(); n!=f.nodes.end(); n=f.nodes.next(n))
    os << *n << endl;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const ROMon::Process& p) {
  char text[132];
  bool noutgid = ::strncmp(p.utgid,"N/A",3)==0;
  const char *opt = noutgid ? p.cmd : "", *utgid = noutgid ? "N/A: " : p.utgid;
  if ( p.cpu > 99.9 )
    ::snprintf(text,sizeof(text),"    %5d %5d %4.0f %4.1f %6.0f %6.0f %4d %-16s %s%s",
               p.pid,p.ppid,p.cpu,p.mem,p.vsize,p.rss,p.threads,p.owner,utgid,opt);
  else
    ::snprintf(text,sizeof(text),"    %5d %5d %4.1f %4.1f %6.0f %6.0f %4d %-16s %s%s",
               p.pid,p.ppid,p.cpu,p.mem,p.vsize,p.rss,p.threads,p.owner,utgid,opt);
  return os << text;
}

ostream& ROMon::operator<<(ostream& os, const ROMon::Procset& s) {
  char text[132];
  ::snprintf(text,sizeof(text),"          %5s %5s %4s %4s %6s %6s %-4s %-16s %s","PID","PPID","%CPU","%MEM","VSize","RSS","#Thr","Owner","UTGID");
  os << "  Node:"  << s.name << " " << s.processes.size() 
     << " Last update:" << s.time << "." << s.millitm << endl
     << text << endl;
  for(Procset::Processes::const_iterator c=s.processes.begin(); c!=s.processes.end(); c=s.processes.next(c))
    os << "   -> " << *c << endl;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const ROMon::ProcFarm& f) {
  os << "Farm:"  << f.name << " " << f.nodes.size() << " nodes. Last update:" << f.time << endl;
  for(ProcFarm::Nodes::const_iterator n=f.nodes.begin(); n!=f.nodes.end(); n=f.nodes.next(n))
    os << *n << endl;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const ROMon::NodeStats& n) {
  os << *n.cpu() << endl << "  " << n.memory << endl << *n.procs();
  return os;
}

ostream& ROMon::operator<<(ostream& os, const ROMon::Diskspace& n) {
  os << "Local disk:" << n.blockSize << " " << n.numBlocks << "/" << n.freeBlocks << endl;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const ROMon::DeferredHLTStats& n) {
  os << n.name << endl << "  " << n.localdisk << endl;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const ROMon::DeferredHLTSubfarmStats& n) {
  os << n.name << endl << "  " << endl;
  return os;
}

/// Empty constructor
Diskspace::Diskspace() {
  reset();
}

/// Reset data content
Diskspace* Diskspace::reset() {
  ::memset(this,0,sizeof(Diskspace));
  return this;
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
TimeStamp CPUfarm::firstUpdate() const {
  return _firstUpdate(nodes);
}

/// Retrieve timestamp of most recent updated node
TimeStamp CPUfarm::lastUpdate() const {
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
TimeStamp ProcFarm::firstUpdate() const {
  return _firstUpdate(nodes);
}

/// Retrieve timestamp of most recent updated node
TimeStamp ProcFarm::lastUpdate() const {
  return _lastUpdate(nodes);
}

/// Reset node structure to allow re-filling
NodeStats* NodeStats::reset() {
  ::memset(this,0,sizeof(NodeStats)+sizeof(CPUset)+sizeof(Procset));
  type = TYPE;
  return this;
}

/// Access to the CPU part of the node structure
CPUset* NodeStats::cpu()  const {
  return (CPUset*)(name + sizeof(name));
}

/// Access to the process part of the node structure
Procset* NodeStats::procs()  const {
  CPUset* c = cpu();
  return (Procset*)(((char*)c) + c->length());
}

/// Fix the lengths before sending. This is the last statement after filling
void NodeStats::fixup() {
  type = TYPE;
  cpuSize = cpu()->length();
  procSize = procs()->length();
  totalSize = cpuSize + procSize + sizeof(NodeStats);
}

/// Standard constructor
Connection::Connection() {
  reset();
}

/// Reset data structure content
Connection* Connection::reset() {
  ::memset(node,0,sizeof(node));
  last = 0;
  status = -1;
  return this;
}

/// Standard constructor
Connectionset::Connectionset() {
  reset();
}

/// Reset object structure
Connectionset* Connectionset::reset() {
  ::memset(this,0,sizeof(Connectionset));
  type = TYPE;
  return this;
}

/// Standard constructor
NodeSummary::NodeSummary(const std::string& n) {
  reset();
  ::strncpy(name,n.c_str(),sizeof(name));
  name[sizeof(name)-1] = 0;
}

/// Reset data structure content
NodeSummary* NodeSummary::reset() {
  ::memset(this,0,sizeof(Procset));
  state = DEAD;
  status = BAD;
  return this;
}

/// Default constructor
SubfarmSummary::SubfarmSummary(const std::string& n) {
  reset();
  ::strncpy(name,n.c_str(),sizeof(name));
  name[sizeof(name)-1] = 0;
}

/// Reset object structure
SubfarmSummary* SubfarmSummary::reset() {
  ::memset(this,0,sizeof(SubfarmSummary));
  type = TYPE;
  return this;
}

/// Default constructor
DeferredHLTStats::DeferredHLTStats(const std::string& n) {
  reset();
  ::strncpy(name,n.c_str(),sizeof(name));
  name[sizeof(name)-1] = 0;
}

/// Reset object structure
DeferredHLTStats* DeferredHLTStats::reset() {
  ::memset(this,0,sizeof(DeferredHLTStats));
  type = TYPE;
  return this;
}

/// Access to the buffer part of the node structure
DeferredHLTSubfarmStats::Nodes* DeferredHLTSubfarmStats::nodes()  const {
  return (Nodes*)(((char*)&runs) + runs.length());
}

/// Fix the lengths before sending. This is the last statement after filling
void DeferredHLTSubfarmStats::fixup() {
  type = TYPE;
  runsSize  = runs.length();
  totalSize = runs.data_length() + nodes()->length() + sizeof(DeferredHLTSubfarmStats);
}

/// Reset node structure to allow re-filling
DeferredHLTSubfarmStats* DeferredHLTSubfarmStats::reset() {
  ::memset(this,0,sizeof(DeferredHLTSubfarmStats)+sizeof(Runs)+sizeof(Nodes));
  type = TYPE;
  return this;
}

/// Retrieve timestamp of earliest updated node
TimeStamp DeferredHLTSubfarmStats::firstUpdate() const {
  return _firstUpdate(*nodes());
}

/// Retrieve timestamp of most recent updated node
TimeStamp DeferredHLTSubfarmStats::lastUpdate() const {
  return _lastUpdate(*nodes());
}

