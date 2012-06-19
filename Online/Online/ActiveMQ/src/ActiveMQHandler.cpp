// $Id: ActiveMQHandler.cpp,v 1.1 2010/11/01 17:20:22 frankb Exp $
//====================================================================
//  Comet
//--------------------------------------------------------------------
//
//  Package    : ActiveMQ
//
//  Description: DIM enabled ActiveMQ
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /local/reps/lhcb/Online/ActiveMQ/src/ActiveMQHandler.cpp,v 1.1 2010/11/01 17:20:22 frankb Exp $
#include "ActiveMQ/ActiveMQHandler.h"
#include "ActiveMQ/ActiveMQSensor.h"
#include "ActiveMQ/Connector.h"
#include "ActiveMQ/Commands.h"
#include "RTL/Base64.h"
#include "CPP/Event.h"

// C++ include files
#include <memory>

using namespace std;
using namespace ActiveMQ;

void ActiveMQHandler::handle(const Event& ev)  {
  if ( ev.eventtype == IocEvent )  {
    switch(ev.type)  {
    case CMD_DATA:  {
      //Connector* c = ev.iocPtr<Connector>();
      //const Data& d = c->data();
      auto_ptr<Data> data(ev.iocPtr<Data>());
      const Data& d = *data.get();
      if ( m_protocol == PROTO_XML ) {
	size_t len_max=1024+d.value.length()+d.tag.length();
	std::auto_ptr<char> text(new char[len_max]);
	::sprintf(text.get(),"<message time=\"%d\" tag=\"%s\"><![CDATA[%s]]></message>",
		  d.time,d.tag.c_str(),d.value.c_str());
	m_sensor->send(d.tag,text.get());
	delete [] text.release();
      }
      else if ( m_protocol == PROTO_SIMPLE ) {
        char text[132];
        ::sprintf(text,"%d#",d.time);
        string value = text;
        value += d.tag;
        value += '#';
        value += d.value;
        m_sensor->send(d.tag,value);
      }
      //size_t len_out=0, idx = ::sprintf(text.get(),"<message time=\"%d\" tag=\"%s\"><![CDATA[",
      //		     d.time,d.tag.c_str());
      //if (!::lib_rtl_encode64(d.value.c_str(),d.value.length(),text.get()+idx,len_max-idx-1,&len_out,9999999)) {
      //  ::lib_rtl_output(LIB_RTL_ERROR,"AMQHandler> Failed to Base64 encode data value for:%s",d.tag.c_str());
      //}
      //*(text.get()+idx+len_out) = 0;
      //string value = text.get();
      //value += d.value;
      //value += "\"/>";
      //m_sensor->send(d.tag,value.c_str(),value.length());
      //delete [] text.release();
      break;
    }
    default:
      break;
    }
  }
}
