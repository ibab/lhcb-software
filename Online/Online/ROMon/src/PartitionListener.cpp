#include "ROMon/PartitionListener.h"
#include "ROMon/Constants.h"
#include "CPP/IocSensor.h"
#include <cstring>
#include <vector>
#include <memory>
extern "C" {
#include "dic.h"
}
using namespace std;
using namespace ROMon;

/// Standard constructor with object setup through parameters
PartitionListener::PartitionListener(Interactor* parent, const string& nam) : m_parent(parent), m_name(nam)
{
  string name = "RunInfo/" + m_name + "/HLTsubFarms";
  m_subFarmDP = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,subFarmHandler,(long)this,0,0);
  name = "RunInfo/" + m_name + "/RunStatus";
  m_runStateDP = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,runStateHandler,(long)this,0,0);
  name = "RunInfo/" + m_name + "/partId";
  m_partIdDP = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,partIdHandler,(long)this,0,0);
}

/// Standard destructor
PartitionListener::~PartitionListener() {
  if ( m_partIdDP   ) ::dic_release_service(m_partIdDP);
  if ( m_runStateDP ) ::dic_release_service(m_runStateDP);
  if ( m_subFarmDP  ) ::dic_release_service(m_subFarmDP);
}

/// DIM command service callback
void PartitionListener::subFarmHandler(void* tag, void* address, int* size) {
  typedef vector<string> StringV;
  string svc;
  auto_ptr<StringV > f(new StringV());
  PartitionListener* h = *(PartitionListener**)tag;
  for(const char* data = (char*)address, *end=data+*size;data<end;data += strlen(data)+1)
    f->push_back(data);
  if ( h->m_name == "FEST" || h->m_name == "LHCb" )   {
    f->push_back("CALD07");
    f->push_back("MONA08");
    f->push_back("MONA09");
    f->push_back("STORECTL01");
  }
  for(StringV::iterator i=f->begin(); i != f->end(); ++i) {
    string& s = *i;
    for(size_t j=0; j<s.length(); ++j) {
      s[j] = ::tolower(s[j]);
    }
  }
  IocSensor::instance().send(h->m_parent,CMD_CONNECT,f.release());
}

/// DIM command service callback
void PartitionListener::runStateHandler(void* tag, void* address, int* size) {
  if ( address && tag && size && *size>0 ) {
    PartitionListener* h = *(PartitionListener**)tag;
    int state = *(int*)address;
    IocSensor::instance().send(h->m_parent,CMD_RUNSTATE,state);
  }
}

/// DIM command service callback
void PartitionListener::partIdHandler(void* tag, void* address, int* size) {
  if ( address && tag && size && *size>0 ) {
    PartitionListener* h = *(PartitionListener**)tag;
    int pid = *(int*)address;
    IocSensor::instance().send(h->m_parent,CMD_PARTITIONID,pid);
  }
}
