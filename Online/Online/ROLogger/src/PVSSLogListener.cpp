// $Id: PVSSLogListener.cpp,v 1.4 2008-10-21 13:53:52 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/PVSSLogListener.cpp,v 1.4 2008-10-21 13:53:52 frankb Exp $

// Framework include files
#include "ROLogger/PVSSLogListener.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "UPI/upidef.h"
#include "ROLoggerDefs.h"

#include <vector>
#include <memory>
#include <cstring>

extern "C" {
#include "dic.h"
}
#ifdef _WIN32
#include <winsock2.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif

using namespace ROLogger;
using namespace std;
typedef vector<string> _SV;

/// Standard constructor with object setup through parameters
PVSSLogListener::PVSSLogListener(Interactor* parent,const string& n) : m_parent(parent), m_name(n)
{
  auto_ptr<_SV> f(new _SV());
  string nam = "RunInfo/" + m_name + "/HLTsubFarms";
  m_subFarmDP = ::dic_info_service((char*)nam.c_str(),MONITORED,0,0,0,subFarmHandler,(long)this,0,0);
  ::upic_write_message2("Subfarm content for %s_RunInfo from:%s",m_name.c_str(),nam.c_str());
  f->push_back("/HLT01/logger/pvss/log");
  f->push_back("/STORECTL01/logger/pvss/log");
  f->push_back("/MONA08/logger/pvss/log");
  if ( name() == "LHCb" ) f->push_back("/CALD07/logger/pvss/log");
  IocSensor::instance().send(m_parent,CMD_UPDATE_FARMS,f.release());
}

/// Standard destructor
PVSSLogListener::~PVSSLogListener() {
  ::dic_release_service(m_subFarmDP);
}

/// DIM command service callback
void PVSSLogListener::subFarmHandler(void* tag, void* address, int* size) {
  string svc;
  auto_ptr<_SV> f(new _SV());
  PVSSLogListener* h = *(PVSSLogListener**)tag;
  for(const char* data = (char*)address, *end=data+*size;data<end;data += strlen(data)+1) {
    svc = "/";
    svc += data;
    svc += "/logger/pvss/log";
    f->push_back(svc);
  }
  f->push_back("/HLT01/logger/pvss/log");
  f->push_back("/STORECTL01/logger/pvss/log");
  f->push_back("/MONA08/logger/pvss/log");
  if ( h->name() == "LHCb" ) f->push_back("/CALD07/logger/pvss/log");
  IocSensor::instance().send(h->m_parent,CMD_UPDATE_FARMS,f.release());
}
