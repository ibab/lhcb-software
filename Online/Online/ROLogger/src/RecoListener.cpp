// $Id: RecoListener.cpp,v 1.2 2009-03-26 16:44:21 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/RecoListener.cpp,v 1.2 2009-03-26 16:44:21 frankb Exp $

// Framework include files
#include "ROLogger/RecoListener.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "UPI/upidef.h"
#include "ROLoggerDefs.h"

#include <cstring>
#include <vector>
#include <memory>

extern "C" {
#include "dic.h"
}

using namespace ROLogger;
using namespace std;
typedef vector<string> _SV;

/// Standard constructor with object setup through parameters
RecoListener::RecoListener(Interactor* parent,const string& n, const string& fac) 
  : m_parent(parent), m_name(n), m_facility(fac)
{
  char txt[64];
  auto_ptr<_SV> f(new _SV());
  //string nam = "RunInfo/" + m_name + "/HLTsubFarms";
  //m_subFarmDP = ::dic_info_service((char*)nam.c_str(),MONITORED,0,0,0,subFarmHandler,(long)this,0,0);
  //::upic_write_message2("Subfarm content for %s_RunInfo from:%s",m_name.c_str(),nam.c_str());
  for(int i=0; i<5; ++i) {
    for(int j=0; j<10; ++j) {
      sprintf(txt,"HLT%c%02d",char(i+'A'),int(j<4 ? j+1 : j+2));
      f->push_back(fmcLogger(txt,m_facility));
    }
  }
  f->push_back(fmcLogger("STOREIO01",m_facility));
  f->push_back(fmcLogger("STORECTL01",m_facility));
  IocSensor::instance().send(m_parent,CMD_UPDATE_FARMS,f.release());
}

/// Standard destructor
RecoListener::~RecoListener() {
  //::dic_release_service(m_subFarmDP);
}

/// DIM command service callback
void RecoListener::subFarmHandler(void* tag, void* address, int* size) {
  auto_ptr<_SV> f(new _SV());
  RecoListener* h = *(RecoListener**)tag;
  for(const char* data = (char*)address, *end=data+*size;data<end;data += strlen(data)+1)
    f->push_back(fmcLogger(data,h->m_facility));
  f->push_back(fmcLogger("STOREIO01",h->m_facility));
  f->push_back(fmcLogger("STORECTL01",h->m_facility));
  f->push_back(fmcLogger("MONA08",h->m_facility));
  f->push_back(fmcLogger("MONA09",h->m_facility));
  f->push_back(fmcLogger("MONA07",h->m_facility));
  if ( h->name() == "LHCb" ) f->push_back(fmcLogger("CALD07",h->m_facility));
  IocSensor::instance().send(h->m_parent,CMD_UPDATE_FARMS,f.release());
}
