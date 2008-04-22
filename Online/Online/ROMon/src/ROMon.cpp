#include <cstdio>
#include <cstdlib>
#define MBM_IMPLEMENTATION
#include "ROMonDefs.h"
#include "ROMon/ROMon.h"
#include "MBM/bmstruct.h"

using namespace ROMon;

MBMBuffer* MBMBuffer::reset() {
  ::memset(this,0,sizeof(MBMBuffer));  
  return this;
}

MBMClient* MBMClient::reset() {
  ::memset(this,0,sizeof(MBMClient));  
  return this;
}

FSMTask* FSMTask::reset() {
  ::memset(this,0,sizeof(FSMTask));  
  return this;
}

Node* Node::reset() {
  ::memset(this,0,sizeof(Node)+sizeof(Node::Buffers)+sizeof(Node::Tasks));
  type = TYPE;
  return this;
}

Node::Buffers* Node::buffers()  const {
  return (Buffers*)(name + sizeof(name));
}

Node::Tasks* Node::tasks()  const {
  Buffers* b = buffers();
  return (Tasks*)(((char*)b) + b->length());
}

void Node::fixup() {
  type = TYPE;
  bmSize = buffers()->length();
  taskSize = tasks()->length();
  totalSize = bmSize + taskSize + sizeof(Node);
}

Nodeset* Nodeset::reset() {
  ::memset(this,0,sizeof(Nodeset));
  type = TYPE;
  return this;
}

/// Retrieve timestamp of earliest updated node
Nodeset::TimeStamp Nodeset::firstUpdate() const {
  return _firstUpdate(nodes);
}

/// Retrieve timestamp of most recent updated node
Nodeset::TimeStamp Nodeset::lastUpdate() const {
  return _lastUpdate(nodes);
}

/// Default constructor
FSMTask::FSMTask() : processID(-1) {
  name[0] = 0;
}

/// Copt constructor
FSMTask::FSMTask(const FSMTask& copy) {
  *this = copy;
}

FSMTask& FSMTask::operator=(const FSMTask& copy) {
  strncpy(name,copy.name,sizeof(name));
  name[sizeof(name)-1]=0;
  processID   = copy.processID;
  state       = copy.state;
  targetState = copy.targetState;
  metaState   = copy.metaState;
  lastCmd     = copy.lastCmd;
  doneCmd     = copy.doneCmd;
  return *this;
}
//==============================================================================

std::ostream& ROMon::log() {
  return std::cout << ::lib_rtl_timestr() << " ";
}

void ROMon::print_startup(const char* msg) {
  log() << ::lib_rtl_timestr() << "> Readout monitor " << msg << " started on " 
	<< RTL::nodeName() << " as " << RTL::processName() << std::endl;
}

