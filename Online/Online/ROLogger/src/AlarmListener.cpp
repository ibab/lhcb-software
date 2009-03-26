// $Id: AlarmListener.cpp,v 1.6 2009-03-26 16:44:21 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/AlarmListener.cpp,v 1.6 2009-03-26 16:44:21 frankb Exp $

// Framework include files
#include "ROLogger/AlarmListener.h"
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
AlarmListener::AlarmListener(Interactor* parent,const string& n, const string& fac) 
  : m_parent(parent), m_name(n), m_facility(fac)
{
  auto_ptr<_SV> f(new _SV());
  string nam = "RunInfo/" + m_name + "/HLTsubFarms";
  m_subFarmDP = ::dic_info_service((char*)nam.c_str(),MONITORED,0,0,0,subFarmHandler,(long)this,0,0);
  ::upic_write_message2("Subfarm content for %s_RunInfo from:%s",m_name.c_str(),nam.c_str());
  f->push_back(fmcLogger("STORECTL01",m_facility));
  f->push_back(fmcLogger("MONA08",m_facility));
  f->push_back(fmcLogger("MONA09",m_facility));
  f->push_back(fmcLogger("MONA07",m_facility));
  if ( name() == "LHCb" ) f->push_back(fmcLogger("CALD07",m_facility));
  IocSensor::instance().send(m_parent,CMD_UPDATE_FARMS,f.release());
}

/// Standard destructor
AlarmListener::~AlarmListener() {
  ::dic_release_service(m_subFarmDP);
}

/// DIM command service callback
void AlarmListener::subFarmHandler(void* tag, void* address, int* size) {
  auto_ptr<_SV> f(new _SV());
  AlarmListener* h = *(AlarmListener**)tag;
  for(const char* data = (char*)address, *end=data+*size;data<end;data += strlen(data)+1)
    f->push_back(fmcLogger(data,h->m_facility));
  f->push_back(fmcLogger("STORECTL01",h->m_facility));
  f->push_back(fmcLogger("MONA08",h->m_facility));
  f->push_back(fmcLogger("MONA09",h->m_facility));
  f->push_back(fmcLogger("MONA07",h->m_facility));
  if ( h->name() == "LHCb" ) f->push_back(fmcLogger("CALD07",h->m_facility));
  IocSensor::instance().send(h->m_parent,CMD_UPDATE_FARMS,f.release());
}
