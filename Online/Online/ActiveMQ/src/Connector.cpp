// $Id: Connector.cpp,v 1.1 2010/11/01 17:20:22 frankb Exp $
//====================================================================
//  ActiveMQ
//--------------------------------------------------------------------
//
//  Package    : ActiveMQ
//
//  Description: DIM enabled ActiveMQ plugin for Apache
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /local/reps/lhcb/Online/ActiveMQ/src/Connector.cpp,v 1.1 2010/11/01 17:20:22 frankb Exp $

#include "ActiveMQ/Commands.h"
#include "ActiveMQ/Connector.h"
#include "CPP/IocSensor.h"
#include "RTL/rtl.h"

extern "C" {
#include "dic.h"
#include "dis.h"
}
#include <ctime>

using namespace ActiveMQ;
using namespace std;
#define DEAD_TAG "DEAD"

/// Standard constructor
Connector::Connector(const std::string& tag, bool may_remove, Interactor* target) 
  : m_type(STRING_T), m_data(tag,DEAD_TAG,time(0)), 
  m_target(target), m_haveData(false), 
  m_mayRemove(may_remove)
{
  string t = tag;
  //for(size_t i=0; i<t.length(); ++i)
  //  if ( t[i] == '.' ) t[i] = '/';
  m_id = ::dic_info_service((char*)t.c_str(),MONITORED,0,0,0,infoHandler,(long)this,(void*)DEAD_TAG,5);
  m_id2 = 0;//::dic_info_service((char*)t.c_str(),MONITORED,0,0,0,infoHandler,(long)this,(void*)DEAD_TAG,5);
  ::lib_rtl_output(LIB_RTL_INFO,"Mapping data tag %s to DIM service %s",tag.c_str(),t.c_str());
}

/// Standard destructor
Connector::~Connector()  {
  if ( m_id ) {
    ::dic_release_service(m_id);
    m_id = 0;
  }
  if ( m_id2 ) {
    ::dic_release_service(m_id2);
    m_id2 = 0;
  }
}

/// Set data item
void Connector::set(int typ, const char* ptr, size_t /* siz */)   {
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
  //::printf("Dim data ready:%s  -> %s\n",m_data.tag.c_str(),m_data.value.c_str());
  publish();
}

/// Publish data item
void Connector::publish()    {
  //if ( m_haveData )  {
  // IOC must match ActiveMQHandler::handle(const Event& ev)!
  //IocSensor::instance().send(m_target,CMD_DATA,this);
  IocSensor::instance().send(m_target,CMD_DATA,new Data(m_data));
  //}
}

/// DimInfo overload to process messages
void Connector::infoHandler(void* tag, void* address, int* size)  {
  Connector* h = *(Connector**)tag;
  if ( address && size && *size>0 ) {
    char *msg = (char*)address;
    if ( *(int*)msg == *(int*)DEAD_TAG )  {
      //::printf("Address[DEAD]:%s->%p\n",h->m_data.tag.c_str(),address);
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
      if ( *size && h->m_type == STRING_T ) msg[*size]=0;
      //::printf("Dim data ready:%s  -> %s %p\n",h->m_data.tag.c_str(),address,size);
      h->set(h->m_type, msg, *size);
    }
  }
  else  {
    //::printf("Address[INVALID]:%s->%p %p %d\n",h->m_data.tag.c_str(),address,size,size?*size:-1);
    h->set(STRING_T,DEAD_TAG,5);
  }
}
