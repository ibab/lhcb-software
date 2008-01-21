#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <ctime>


#include <iostream>
#include <iomanip>

#include "ROMon.h"


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
  bmSize = buffers()->length();
  taskSize = tasks()->length();
  totalSize = bmSize + taskSize + sizeof(Node);
}

Nodeset* Nodeset::reset() {
  ::memset(this,0,sizeof(Nodeset));
  return this;
}

namespace {

template<typename T> std::ostream& operator<<(std::ostream& os, const FixItems<T>& items) {
  os << "Start address:" << (void*)&items
     << " Num Elements:" << std::setw(6)  << std::right << items.size()
     << " - "            << std::setw(6)  << std::left << (items.end()-items.begin())
     << " Length:"       << std::setw(4)  << std::right << items.length()
     << " DataLen:"      << std::setw(4)  << std::right << items.data_length()
     << " Item size: "   << std::setw(4)  << std::right << sizeof(T)
     << " Begin: "       << (void*)items.begin()
     << " End: "         << (void*)items.end();
  if ( items.begin() != items.end() ) os << std::endl;
  for (T* p=items.begin(); p!=items.end(); ++p) os << *p;
  return os;
}

template<typename T> std::ostream& operator<<(std::ostream& os, const VarItems<T>& items) {
  os << "Start address:" << (void*)&items
     << " Num Elements:" << std::setw(6)  << std::right << items.size()
     << " - "            << std::setw(6)  << std::left << (items.end()-items.begin())
     << " Length:"       << std::setw(4)  << std::right << items.length()
     << " DataLen:"      << std::setw(4)  << std::right << items.data_length()
     << " Item size: "   << std::setw(4)  << std::right << sizeof(T)
     << " Begin: "       << (void*)items.begin()
     << " End: "         << (void*)items.end();
  if ( items.begin() != items.end() ) os << std::endl;
  for (T* p=items.begin(); p!=items.end(); p=items.next(p)) os << *p;
  return os;
}

std::ostream& operator<<(std::ostream& os, const CONTROL& c) {
  os << "Size:" << sizeof(c) << " " << sizeof(CONTROL);
  return os;
}

std::ostream& operator<<(std::ostream& os, const MBMClient& c) {
  os << " MBMClient@"  << (void*)&c
     << " Name:"      << c.name 
     << " PID:"       << std::setw(6) << c.processID
     << " Partition:" << std::setw(6) << std::hex << c.partitionID << std::dec
     << " State:"     << c.state
     << " Type:"      << c.type
     << " Events:"    << std::setw(6) << c.events
     << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const MBMBuffer& b) {
  os << "MBM@"     << (void*)&b
     << " Name:"   << b.name
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
  Node::Tasks*   t = n.tasks();
  Node::Buffers* b = n.buffers();
  os << "Node@"       << (void*)&n
     << " Name:"      << n.name
     << std::endl;
  os << "Buffers:" << *b << std::endl;
  os << "Tasks:"   << *t << std::endl;
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

}


static FSMTask& fill(FSMTask* t, const MBMClient& c) {
  ::strcpy(t->name,c.name);
  t->processID   = c.processID;
  t->state       = 'I';
  t->targetState = 'R';
  t->metaState   = 'E';
  t->lastCmd  = 0;
  t->doneCmd = 1;
  return *t;
}

static MBMBuffer& fill(MBMBuffer* mbm, int partitionID, const char* name) {
  static int task_no = 0;
  const char types[] = {'C','P','U'};
  ::memset(mbm,0,sizeof(MBMBuffer));
  MBMBuffer::Clients::iterator it = mbm->clients.reset();
  ::strcpy(mbm->name,name);
  for(int i=0; i<10; ++i, it=mbm->clients.add(it)) {
    ::sprintf((*it).name,"Task_%s_%02d",name,++task_no);
    (*it).partitionID = partitionID;
    (*it).processID   = 1024+10*i+task_no;
    (*it).events      = 1000*i;
    (*it).type        = types[i%3];
    (*it).state       = 'a'+i;
  }
  return *mbm;
}

static Node& fill(Node* n, const char* name) {
  char txt[64];
  static int buff_no = 0;
  n->reset();
  ::strcpy(n->name,name);
  Node::Buffers* b = n->buffers();
  Node::Buffers::iterator ib = b->reset();
  for(int i=0; i<10; ++i, ib = b->add(ib) ) {
    ::sprintf(txt,"Events_%02d",++buff_no);
    fill(ib, 0x111*i, txt);
  }
  Node::Tasks* t = n->tasks();
  Node::Tasks::iterator it = t->reset();
  for(ib=b->begin(); ib != b->end(); ib = b->next(ib) ) {
    MBMBuffer::Clients::iterator ic=(*ib).clients.begin();
    for(; ic != (*ib).clients.end(); ++ic, it=t->add(it))
      fill(it,*ic);
  }
  n->fixup();
  return *n;
}

extern "C" int ro_mon_test(int, char**) {
  size_t len = 1024*64;
  char* buff = new char[len];
  typedef FixItems<int> IntItems;
  IntItems* i_items = (IntItems*)buff;
  i_items->reset();
  std::cout << *i_items << std::endl;
  for(int i=0, *i_it=i_items->begin(); i<10; ++i) {
    *i_it = i;
    i_it=i_items->add(i_it);
  }
  std::cout << *i_items << std::endl;
  std::cout << fill((MBMBuffer*)buff, 0x14d, "Events") << std::endl;
  std::cout << fill((Node*)buff, "storectl01") << std::endl;
  delete [] buff;
  return 1;
}
