// $Header:
// Include files 
#include "GaudiKernel/SvcFactory.h"
#include <string>


// local
#include "OnlineFileSelector/TestFilePollerSvc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestFilePollerSvc
//
// 2014-02-06 : Ioannis Chalkiadakis
//-----------------------------------------------------------------------------

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,TestFilePollerSvc)

using namespace std;
using namespace LHCb;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestFilePollerSvc::TestFilePollerSvc(const string& nam,ISvcLocator* svc)
                                    :OnlineService::OnlineService(nam,svc) {

  

}
//=============================================================================
// Destructor
//=============================================================================
TestFilePollerSvc::~TestFilePollerSvc() {} 

//=============================================================================


StatusCode TestFilePollerSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if(IAlertSvc::interfaceID().versionMatch(riid))
    *ppvIF = (IAlertSvc*)this;
  else
    return OnlineService::queryInterface(riid, ppvIF);
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode TestFilePollerSvc::initialize()  {
  StatusCode sc = LHCb::OnlineService::initialize();
  setSvcName(OnlineService::name());
  if ( !sc.isSuccess() )  {
     error("Cannot initialize service base class.");
     return sc;
  }
  sc = service("Poller",m_filePoller,true);
  if ( !sc.isSuccess() )  {
     error("Cannot access poller service.");
     return sc;
  }
  m_filePoller->addListener(this);
  
  return m_filePoller == 0 ? StatusCode::FAILURE : StatusCode::SUCCESS;
}


StatusCode TestFilePollerSvc::finalize()  {

  if ( m_filePoller )  {

    m_filePoller->remListener(this);
    releaseInterface(m_filePoller);

    }
  return LHCb::OnlineService::finalize();
}



StatusCode TestFilePollerSvc::alertSvc(const string& file_name) 
{
  //Wake listener up,take care of any lock/synchronisation issues.
  cout << "Ready to read events from " + file_name << endl;
  
  return StatusCode::SUCCESS;
}


void TestFilePollerSvc::setSvcName(const string& name)
{
  m_listener_name = name;
}


string TestFilePollerSvc::getSvcName()
{
  return m_listener_name;
  
}
