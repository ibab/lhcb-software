#include "ROMon/PartitionListener.h"
#include "ROMon/Constants.h"
#include "CPP/IocSensor.h"
#include "RTL/readdir.h"
#include "RTL/strdef.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <memory>
extern "C" {
#include "dic.h"
}
using namespace std;
using namespace ROMon;
typedef vector<string> StringV;

/// Standard constructor with object setup through parameters
PartitionListener::PartitionListener(Interactor* parent, const string& nam, const std::string& match)
  : m_parent(parent), m_name(nam), m_match(match)
{
  for(size_t j=0; j<m_match.length(); ++j)
    m_match[j] = char(::tolower(m_match[j]));
  string name = "RunInfo/" + m_name + "/HLTsubFarms";
  m_subFarmDP = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,subFarmHandler,(long)this,0,0);
  name = "RunInfo/" + m_name + "/RunStatus";
  m_runStateDP = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,runStateHandler,(long)this,0,0);
  name = "RunInfo/" + m_name + "/partId";
  m_partIdDP = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,partIdHandler,(long)this,0,0);
}

/// Standard constructor with object setup through parameters
PartitionListener::PartitionListener(Interactor* parent, const string& nam, const string& match, bool)
  : m_parent(parent), m_name(nam), m_match(match), m_subFarmDP(0), m_partIdDP(0), m_runStateDP(0)
{
  const char* c = ::getenv("ROMONDATA");
  string sf_nam, dir_name = c ? c : "../xml";
  for(size_t j=0; j<m_match.length(); ++j)
    m_match[j] = char(::tolower(m_match[j]));

  DIR* dir = ::opendir(dir_name.c_str());
  if ( dir ) {
    auto_ptr<StringV > f(new StringV());
    StringV::iterator i;
    size_t idx = 0;
    dirent* e = 0;
    while ( (e=::readdir(dir)) != 0 ) {
      sf_nam = e->d_name;
      idx = sf_nam.find(".xml");
      if ( idx+4 == sf_nam.length() ) {
        if ( idx != string::npos && sf_nam.find("TaskInventory") == string::npos ) {
	  sf_nam = sf_nam.substr(0,idx);
	  for(size_t j=0; j<sf_nam.length(); ++j)
	    sf_nam[j] = char(::tolower(sf_nam[j]));
	  if ( m_match.empty() || m_match=="*" )
	    f->push_back(sf_nam);
	  else if ( ::strcase_match_wild(sf_nam.c_str(),m_match.c_str()) ) 
	    f->push_back(sf_nam);
        }
      }
    }
    IocSensor::instance().send(m_parent,CMD_CONNECT,f.release());
    return;
  }
  cout << "Error reading XML directory:" << dir_name << endl;
  ::exit(1);
}

/// Standard destructor
PartitionListener::~PartitionListener() {
  if ( m_partIdDP   ) ::dic_release_service(m_partIdDP);
  if ( m_runStateDP ) ::dic_release_service(m_runStateDP);
  if ( m_subFarmDP  ) ::dic_release_service(m_subFarmDP);
}

/// DIM command service callback
void PartitionListener::subFarmHandler(void* tag, void* address, int* size) {
  string svc;
  auto_ptr<StringV > f(new StringV());
  PartitionListener* h = *(PartitionListener**)tag;
  for(const char* data = (char*)address, *end=data+*size;data<end;data += strlen(data)+1) {
    string s = data;
    for(size_t j=0; j<s.length(); ++j)
      s[j] = char(::tolower(s[j]));

    if ( h->m_match.empty() || h->m_match == "*" )
      f->push_back(s);
    else if ( ::strcase_match_wild(s.c_str(),h->m_match.c_str()) ) 
      f->push_back(s);
  }
  if ( h->m_name == "FEST" || h->m_name == "LHCb" )   {
    f->push_back("cald07");
    f->push_back("mona08");
    f->push_back("mona09");
    f->push_back("storectl01");
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
