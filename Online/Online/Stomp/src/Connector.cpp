// $Id: Connector.cpp,v 1.1 2009-07-03 18:10:07 frankb Exp $
//====================================================================
//  Stomp
//--------------------------------------------------------------------
//
//  Package    : Stomp
//
//  Description: DIM enabled Stomp plugin for Apache
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Stomp/src/Connector.cpp,v 1.1 2009-07-03 18:10:07 frankb Exp $

#include "Stomp/Commands.h"
#include "Stomp/Connector.h"
#include "Stomp/Log.h"
#include "CPP/IocSensor.h"

extern "C" {
#include "dic.h"
#include "dis.h"
}
#include <ctime>

using namespace Stomp;
using namespace std;
#define DEAD_TAG "DEAD"

/// Standard constructor
Connector::Connector(const std::string& tag, bool may_remove, Interactor* target) 
: m_type(STRING_T), m_data(tag,DEAD_TAG,time(0)), 
  m_target(target), m_haveData(false), 
  m_mayRemove(may_remove)
{
  string t = tag;
  for(size_t i=0; i<t.length(); ++i)
    if ( t[i] == '.' ) t[i] = '/';
  m_id = ::dic_info_service((char*)t.c_str(),MONITORED,0,0,0,infoHandler,(long)this,DEAD_TAG,5);
  log() << "Mapping data tag " << tag << " to DIM service " << t << endl;
}

/// Standard destructor
Connector::~Connector()  {
  if ( m_id ) {
    ::dic_release_service(m_id);
    m_id = 0;
  }
}

/// Set data item
void Connector::set(int typ, const char* ptr, size_t siz)   {
  char text[256];
  m_data.time = ::time(0);
  switch (typ)  {
    case FLOAT_T:
      ::sprintf(text,"%f",*(float*)ptr);
      m_data.value = text;
      break;
    case DOUBLE_T:
      ::sprintf(text,"%f",*(double*)ptr);
      m_data.value = text;
      break;
    case SHORT_T:
      ::sprintf(text,"%d",*(short*)ptr);
      m_data.value = text;
      break;
    case INT_T:
      ::sprintf(text,"%d",*(int*)ptr);
      m_data.value = text;
      break;
    case LONG_T:
      ::sprintf(text,"%ld",*(long*)ptr);
      m_data.value = text;
      break;
    case STRING_T:
      m_data.value = ptr;
      break;
    case UNKNOWN_T:
    default:
      m_data.value = "";
      return;
  }
  publish();
}

/// Publish data item
void Connector::publish()    {
  //if ( m_haveData )  {
    IocSensor::instance().send(m_target,CMD_DATA,this);
  //}
}

/// DimInfo overload to process messages
void Connector::infoHandler(void* tag, void* address, int* size)  {
  Connector* h = *(Connector**)tag;
  if ( address && size && *size>0 ) {
    char *msg = (char*)address;
    if ( *(int*)msg == *(int*)DEAD_TAG )  {
      h->set(STRING_T,DEAD_TAG,5);
    }
    else  {
      if ( !h->m_haveData )  {
        const char* fmt = ::dic_get_format(h->m_id);
        h->m_haveData = true;
        switch ( ::toupper(fmt[0]) )  {
          case 'F': h->m_type = FLOAT_T;    break;
          case 'D': h->m_type = DOUBLE_T;   break;
          case 'S': h->m_type = SHORT_T;    break;
          case 'I': h->m_type = INT_T;      break;
          case 'L': h->m_type = LONG_T;     break;
          case 'C': h->m_type = STRING_T;   break;
          default:                          return;
        }
      }
      if ( h->m_type == STRING_T ) msg[*size]=0;
      h->set(h->m_type, msg, *size);
    }
  }
  else  {
    h->set(STRING_T,DEAD_TAG,5);
  }
}
