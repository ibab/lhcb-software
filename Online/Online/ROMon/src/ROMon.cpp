#include <cstdio>
#include <cstdlib>
#define MBM_IMPLEMENTATION
#include "ROMonDefs.h"
#include "ROMon/ROMon.h"
#include "MBM/bmstruct.h"
#include "RTL/time.h"

using namespace ROMon;

void ROMon::ro_get_node_name(char* name, size_t len) {
  ::strncpy(name,RTL::nodeNameShort().c_str(),len);
  name[len-1] = 0;
  char* p = ::strchr(name,'.');
  if ( p ) *p = 0;
}

void ROMon::ro_gettime(int* time, unsigned int* millitm) {
  timeval tv={0,0};
  ::gettimeofday(&tv,0);
  *time = tv.tv_sec;
  *millitm = tv.tv_usec/1000;
}

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

/// Reset node structure to allow re-filling
Node* Node::reset() {
  ::memset(this,0,sizeof(Node)+sizeof(Node::Buffers)+sizeof(Node::Tasks));
  type = TYPE;
  return this;
}

/// Access to the buffer part of the node structure
Node::Buffers* Node::buffers()  const {
  return (Buffers*)(name + sizeof(name));
}

/// Access to the tasks part of the node structure
Node::Tasks* Node::tasks()  const {
  Buffers* b = buffers();
  return (Tasks*)(((char*)b) + b->length());
}

/// Fix the lengths before sending. This is the last statement after filling
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
	<< RTL::nodeNameShort() << " as " << RTL::processName() << std::endl;
}

