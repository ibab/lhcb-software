// $Id: DeliveryHandler.cpp,v 1.2 2009-11-27 16:39:45 frankb Exp $
//====================================================================
//  Comet
//--------------------------------------------------------------------
//
//  Package    : Stomp
//
//  Description: DIM enabled Stomp
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Stomp/src/DeliveryHandler.cpp,v 1.2 2009-11-27 16:39:45 frankb Exp $
#include "Stomp/DeliveryHandler.h"
#include "Stomp/StompSensor.h"
#include "Stomp/Connector.h"
#include "Stomp/Commands.h"
#include "Stomp/Log.h"
#include "CPP/Event.h"

using namespace std;
using namespace Stomp;

void DeliveryHandler::handle(const Event& ev)  {
  if ( ev.eventtype == IocEvent )  {
    switch(ev.type)  {
      case CMD_DATA:  {
        char text[132];
        Connector* c = ev.iocPtr<Connector>();
        const Data& d = c->data();
        ::sprintf(text,"%d#",d.time);
        string value = text;
        value += d.tag;
        value += '#';
        value += d.value;
        m_sensor->send(d.tag,value.c_str(),value.length());
        // log() << "Publish data tag " << d.tag << ": " << value << endl;
        break;
      }
      default:
        break;
    }
  }
}
