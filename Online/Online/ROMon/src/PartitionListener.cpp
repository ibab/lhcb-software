#include "ROMon/PartitionListener.h"
#include "ROMon/Constants.h"
#include "ROMonDefs.h"
#include "CPP/IocSensor.h"
#include "RTL/readdir.h"
#include "RTL/strdef.h"
#include "RTL/rtl.h"
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
PartitionListener::PartitionListener(CPP::Interactor* par, const string& nam, const string& match)
  : m_parent(par), m_name(nam), m_match(match), m_subFarmDP(0), m_partIdDP(0), m_runStateDP(0)
{
  for(size_t j=0; j<m_match.length(); ++j)
    m_match[j] = char(::tolower(m_match[j]));
  if ( m_name == "ALL") {
    m_subFarmDP = ::dic_info_service((char*)"DIS_DNS/SERVER_LIST",MONITORED,0,0,0,dnsDataHandler,(long)this,0,0);
  }
  else {
    string name = "RunInfo/" + m_name + "/HLTsubFarms";
    m_subFarmDP = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,subFarmHandler,(long)this,0,0);
    name = "RunInfo/" + m_name + "/RunStatus";
    m_runStateDP = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,runStateHandler,(long)this,0,0);
    name = "RunInfo/" + m_name + "/partId";
    m_partIdDP = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,partIdHandler,(long)this,0,0);
  }
}

/// Standard constructor with object setup through parameters
PartitionListener::PartitionListener(CPP::Interactor* par, const string& nam, const string& match, bool)
  : m_parent(par), m_name(nam), m_match(match), m_subFarmDP(0), m_partIdDP(0), m_runStateDP(0)
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
    f->push_back(name());
    while ( (e=::readdir(dir)) != 0 ) {
      sf_nam = e->d_name;
      idx = sf_nam.find(".xml");
      if ( idx+4 == sf_nam.length() ) {
        if ( idx != string::npos && sf_nam.find("TaskInventory") == string::npos ) {
          sf_nam = sf_nam.substr(0,idx);
          for(size_t j=0; j<sf_nam.length(); ++j)
            sf_nam[j] = char(::tolower(sf_nam[j]));
          if ( m_match.empty() || m_match=="*" ) {
            //::lib_rtl_output(LIB_RTL_FATAL,"Use File: %s\n",sf_nam.c_str());
            f->push_back(sf_nam);
          }
          else if ( ::strcase_match_wild(sf_nam.c_str(),m_match.c_str()) ) {
            //::lib_rtl_output(LIB_RTL_FATAL,"Use File: %s\n",sf_nam.c_str());
            f->push_back(sf_nam);
          }
          else {
            //::lib_rtl_output(LIB_RTL_FATAL,"Skip File: %s\n",sf_nam.c_str());
          }
        }
      }
    }
    ::closedir(dir);
    IocSensor::instance().send(parent(),CMD_CONNECT,f.release());
    return;
  }
  ::lib_rtl_output(LIB_RTL_FATAL,"Error reading XML directory: %s\n",dir_name.c_str());
  ::exit(1);
}

/// Standard destructor
PartitionListener::~PartitionListener() {
  if ( m_partIdDP   ) ::dic_release_service(m_partIdDP);
  if ( m_runStateDP ) ::dic_release_service(m_runStateDP);
  if ( m_subFarmDP  ) ::dic_release_service(m_subFarmDP);
}

/// DIM command service callback
void PartitionListener::dnsDataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    PartitionListener* listener = *(PartitionListener**)tag;
    char *msg = (char*)address;
    string svc, node;
    size_t idx, idq;
    switch(msg[0]) {
    case '+':
      getServiceNode(++msg,svc,node);
      idx = svc.find("/ROpublish");
      idq = svc.find("/hlt");
      if ( idq == string::npos ) idq = svc.find("/mona");
      if ( idq == string::npos ) idq = svc.find("/store");
      if ( idx != string::npos && idq == 0 ) {
        string f = svc.substr(1,idx-1);
        if ( ::strcase_match_wild(f.c_str(),listener->m_match.c_str()) ) {
          IocSensor::instance().send(listener->parent(),CMD_ADD,new string(f));
        }
      }
      break;
    case '-':
      break;
    case '!':
      //getServiceNode(++msg,svc,node);
      //log() << "Service " << msg << " in ERROR." << endl;
      break;
    default:
      if ( *(int*)msg != *(int*)"DEAD" )  {
        char *at, *p = msg, *last = msg;
        auto_ptr<vector<string> > farms(new vector<string>());
        farms->push_back(listener->name());
        while ( last != 0 && (at=strchr(p,'@')) != 0 )  {
          last = strchr(at,'|');
          if ( last ) *last = 0;
          getServiceNode(p,svc,node);
          idx = svc.find("/ROpublish");
          idq = svc.find("/hlt");
          if ( idq == string::npos ) idq = svc.find("/mona");
          if ( idq == string::npos ) idq = svc.find("/store");
          if ( idx != string::npos && idq == 0 ) {
            string f = svc.substr(1,idx-1);
            if ( ::strcase_match_wild(f.c_str(),listener->m_match.c_str()) ) {
              farms->push_back(f);
            }
          }
          p = last+1;
        }
        if ( !farms->empty() )
          IocSensor::instance().send(listener->parent(),CMD_CONNECT,farms.release());
      }
      break;
    }
  }
}

/// DIM command service callback
void PartitionListener::subFarmHandler(void* tag, void* address, int* size) {
  string svc, host;
  auto_ptr<StringV > f(new StringV());
  PartitionListener* h = *(PartitionListener**)tag;
  f->push_back(h->name());
  for(const char* data = (char*)address, *end=data+*size;data<end;data += strlen(data)+1) {
    string s = data;
    for(size_t j=0; j<s.length(); ++j)
      s[j] = char(::tolower(s[j]));

    if ( h->m_match.empty() || h->m_match == "*" )
      f->push_back(s);
    else if ( ::strcase_match_wild(s.c_str(),h->m_match.c_str()) ) 
      f->push_back(s);
  }
  host = h->name();
  if ( host.substr(0,4) == "LHCb" )   {
    f->push_back("cald07");
    f->push_back("mona09");
  }
  else if ( h->name() == "FEST" )   {
    f->push_back("mona09");
  }
  f->push_back("mona08");
  f->push_back("storectl01");
  IocSensor::instance().send(h->parent(),CMD_CONNECT,f.release());
}

/// DIM command service callback
void PartitionListener::runStateHandler(void* tag, void* address, int* size) {
  if ( address && tag && size && *size>0 ) {
    PartitionListener* h = *(PartitionListener**)tag;
    int state = *(int*)address;
    IocSensor::instance().send(h->parent(),CMD_RUNSTATE,state);
  }
}

/// DIM command service callback
void PartitionListener::partIdHandler(void* tag, void* address, int* size) {
  if ( address && tag && size && *size>0 ) {
    PartitionListener* h = *(PartitionListener**)tag;
    int pid = *(int*)address;
    IocSensor::instance().send(h->parent(),CMD_PARTITIONID,pid);
  }
}
