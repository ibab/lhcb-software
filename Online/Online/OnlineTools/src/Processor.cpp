//====================================================================
//  OnlineTools
//--------------------------------------------------------------------
//
//  Package    : OnlineTools
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineTools/src/Processor.cpp,v 1.2 2008-09-26 16:05:41 frankb Exp $

// Framework include files
#include "OnlineTools/Processor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "Defs.h"
#include <sstream>
#include <iomanip>

using namespace std;
using namespace OnlineTools;

int ProcessorGroup::add(NodeManipulator* item,
			const string& a1, 
			const string& a2, 
			const string& a3, 
			const string& a4)
{
  if ( !a1.empty() ) {
    SubGroups::iterator i=m_groups.find(a1);
    if ( i==m_groups.end() ) m_groups[a1]=new ProcessorGroup(a1,m_exec);
    m_groups[a1]->add(item,a2,a3,a4,"");
    return 1;
  }
  m_groups[item->name()] = item;
  return 1;
}

NodeManipulator::Children ProcessorGroup::children() {
  NodeManipulator::Children r;
  for(SubGroups::iterator i=m_groups.begin();i!=m_groups.end();++i)
    r.push_back((*i).second);
  return r;
}

/// Standard constructor
Processor::Processor(const string& n,CPP::Interactor* i)  : NodeManipulator(n,i)
{
}

/// Standard destructor
Processor::~Processor()  {
}

int Processor::_send(const string& m, const string& d) {
  stringstream s, q;
  s << "echo " << left << setw(12) << (name()+":  `") << d << "`" << endl;
  q << left << setw(12) << (name()+":  ") << left << setw(12) << m << "   -->  " << d << endl;
  IocSensor::instance().send(m_exec,CMD_EXECUTE,new string(s.str()));
  ::upic_write_message(q.str().c_str(),"");
  return 1;
}

int Processor::doPing() 
{  return _send("PING","ping -c 1 -W 1 "+name()+" | grep packets");   }

int Processor::doReset() 
{  return _send("RESET","power_reset "+name());                       }

int Processor::doStatus() 
{  return _send("STATUS","power_status "+name());                     }

int Processor::doSwitchON() 
{  return _send("Switch ON","power_on "+name());                      }

int Processor::doSwitchOFF() 
{  return _send("Switch OFF","power_off "+name());                    }
