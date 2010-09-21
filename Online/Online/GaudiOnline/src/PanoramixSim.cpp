// $Id: PanoramixSim.cpp,v 1.2 2010-09-21 14:28:26 frankb Exp $
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiOnline/PanoramixSim.h"

// Instantiation of a static factory class used by clients to create instances of this service

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,PanoramixSim)

using namespace LHCb;
using namespace std;

// Standard Constructor
PanoramixSim::PanoramixSim(const string& nam, ISvcLocator* svcLoc)   
: EventRunable(nam, svcLoc)
{
}

// Standard Destructor
PanoramixSim::~PanoramixSim()   
{
}

StatusCode PanoramixSim::reinit() {
  IService* evtLoopMgr=0;
  SmartIF<IService> am(serviceLocator());
  cout << "Starting EventLoopMgr to reinitialize....." << endl;
  service("EventLoopMgr",evtLoopMgr,true).ignore();
  am->stop();
  StatusCode sc = evtLoopMgr->reinitialize();
  evtLoopMgr->release();
  am->start();
  cout << "EventLoopMgr reinitialized....." << sc.getCode() << endl;
  return sc;
}

int PanoramixSim::prompt() {
  char c = 'N';
  cout << "Issue new command:" << endl;
  cout << "Exit the event loop:             E" << endl;
  cout << "Reinitialize the event selector: R" << endl;
  cout << "Execut next event:               N" << endl;
  cin >> c;
  c = char(::toupper(c));
  switch(c) {
  case 'N':
    return 1;
  case 'R':
    return reinit().isSuccess() ? 1 : 0;
  case 'E':
  default:
    return 0;
  }
}

/// IRunable implementation : Run the class implementation
StatusCode PanoramixSim::run()   {
  SmartIF<IAppMgrUI> ui(serviceLocator());
  if ( ui )    {
    m_receiveEvts = true;
    while ( prompt() > 0 )   {
      // loop over the events
      DataObject* pObj = 0;
      StatusCode sc = ui->nextEvent(1);
      if ( sc.isSuccess() )  {
        if ( !m_dataSvc->findObject("/Event",pObj).isSuccess() )  {
          info("End of event input reached. Event server dead ?");
        }
        continue;
      }
      error("Failed to process event.");
    }
    error("Exiting event loop with SUCCESS.");
    return StatusCode::SUCCESS;
  }
  return error("Failed to access Application manager UI.");
}
