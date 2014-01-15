// $Id: ROMonOstream.cpp,v 1.3 2008-11-11 15:09:26 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonOstream.cpp,v 1.3 2008-11-11 15:09:26 frankb Exp $

#include <iomanip>
#include <sys/timeb.h>
#define MBM_IMPLEMENTATION
#include "ROMon/ROMonOstream.h"
using namespace std;
using namespace ROMon;
using namespace Online;

ostream& operator<<(ostream& os, const CONTROL& c) {
  os << "Events: Prod:" << setw(8) << c.tot_produced
     << " Pend:" << setw(3) << c.i_events
     << " Max:"  << setw(4) << c.p_emax
     << " Space[kB]: Tot:" << setw(5) << (c.bm_size*c.bytes_p_Bit)/1024
     << " Free:" << (c.i_space*c.bytes_p_Bit)/1024
     << " Users:Tot:" << setw(3) << c.i_users
     << " Max:" << c.p_umax
     << endl;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const MBMClient& c) {
  os << " MBMClient@"  << (void*)&c
     << " Name:"      << setw(24) << left << c.name 
     << " PID:"       << setw(6)  << c.processID
     << " Partition:" << setw(6)  << hex << c.partitionID << dec
     << " State:"     << setw(2)  << right << (int)c.state
     << " Type:"      << c.type
     << " Events:"    << setw(6) << c.events;
  if ( c.type == 'C' ) os << " Reqs:" << c.reqs[0] << c.reqs[1] << c.reqs[2] << c.reqs[3];   
  os << endl;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const MBMBuffer& b) {
  os << "MBM@"     << (void*)&b
     << " Name:"   << setw(16) << left << b.name
     << " Len:"    << b.length() << endl;
  os << "Control:" << b.ctrl     << endl;
  os << "Clients:" << b.clients  << endl << right;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const FSMTask& t) {
  os << "Task@"    << (void*)&t
     << " UTGID:"  << setw(32) << left << t.name
     << " PID:"    << setw(6)  << t.processID
     << " State:"  << t.state << " -> " << t.targetState << " [" << t.metaState << "]"
     << " Stamps:" << t.lastCmd << " " << t.doneCmd << right
     << endl;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const Node& n) {
  char buff[64];
  time_t tim = n.time;
  ::strftime(buff,sizeof(buff),"%Y-%m-%d %H:%M:%S",::localtime(&tim));
  Node::Tasks*   t = n.tasks();
  Node::Buffers* b = n.buffers();
  os << "Node@"       << (void*)&n
     << " Name:"      << n.name
     << " Time:"      << buff << "." << n.millitm
     << endl;
  os << "Buffers:"    << *b << endl;
  os << "Tasks:  "    << *t << endl;
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
     << endl;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const Nodeset& n) {
  os << "Nodeset@"       << (void*)&n
     << " Name:"      << n.name
     << endl;
  os << "Nodes:"    << n.nodes << endl;
  return os;
}

