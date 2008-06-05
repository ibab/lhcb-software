// $Id: ROMonOstream.cpp,v 1.2 2008-06-05 18:40:00 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonOstream.cpp,v 1.2 2008-06-05 18:40:00 frankb Exp $

#include <iomanip>
#include <sys/timeb.h>
#define MBM_IMPLEMENTATION
#include "ROMon/ROMonOstream.h"
using namespace ROMon;

std::ostream& operator<<(std::ostream& os, const CONTROL& c) {
  os << "Events: Prod:" << std::setw(8) << c.tot_produced
     << " Pend:" << std::setw(3) << c.i_events
     << " Max:"  << std::setw(4) << c.p_emax
     << " Space[kB]: Tot:" << std::setw(5) << (c.bm_size*c.bytes_p_Bit)/1024
     << " Free:" << (c.i_space*c.bytes_p_Bit)/1024
     << " Users:Tot:" << std::setw(3) << c.i_users
     << " Max:" << c.p_umax
     << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const MBMClient& c) {
  os << " MBMClient@"  << (void*)&c
     << " Name:"      << std::setw(24) << std::left << c.name 
     << " PID:"       << std::setw(6)  << c.processID
     << " Partition:" << std::setw(6)  << std::hex << c.partitionID << std::dec
     << " State:"     << std::setw(2)  << std::right << (int)c.state
     << " Type:"      << c.type
     << " Events:"    << std::setw(6) << c.events;
  if ( c.type == 'C' ) os << " Reqs:" << c.reqs[0] << c.reqs[1] << c.reqs[2] << c.reqs[3];   
  os << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const MBMBuffer& b) {
  os << "MBM@"     << (void*)&b
     << " Name:"   << std::setw(16) << std::left << b.name
     << " Len:"    << b.length() << std::endl;
  os << "Control:" << b.ctrl     << std::endl;
  os << "Clients:" << b.clients  << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const FSMTask& t) {
  os << "Task@"    << (void*)&t
     << " UTGID:"  << std::setw(32) << std::left << t.name
     << " PID:"    << std::setw(6)  << t.processID
     << " State:"  << t.state << " -> " << t.targetState << " [" << t.metaState << "]"
     << " Stamps:" << t.lastCmd << " " << t.doneCmd
     << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const Node& n) {
  char buff[64];
  time_t tim = n.time;
  ::strftime(buff,sizeof(buff),"%Y-%m-%d %H:%M:%S",::localtime(&tim));
  Node::Tasks*   t = n.tasks();
  Node::Buffers* b = n.buffers();
  os << "Node@"       << (void*)&n
     << " Name:"      << n.name
     << " Time:"      << buff << "." << n.millitm
     << std::endl;
  os << "Buffers:"    << *b << std::endl;
  os << "Tasks:  "    << *t << std::endl;
  os << "Node@"       << (void*)&n
     << " BuffLen:"   << b->length()
     << " "           << b->data_length()
     << " bmSize:"    << n.bmSize
     << " TaskLen:"   << t->length()
     << " "           << t->data_length()
     << " FSMTasks:"  << t->size()*sizeof(FSMTask)
     << " Clients:"   << t->size()*sizeof(MBMClient)
     << " taskSize:"  << n.taskSize
     << " Total:"     << n.totalSize
     << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const Nodeset& n) {
  os << "Nodeset@"       << (void*)&n
     << " Name:"      << n.name
     << std::endl;
  os << "Nodes:"    << n.nodes << std::endl;
  return os;
}
