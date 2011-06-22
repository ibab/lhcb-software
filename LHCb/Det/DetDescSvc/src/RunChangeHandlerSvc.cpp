// Include files
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"

#include "DetDesc/ValidDataObject.h"
#include "DetDesc/RunChangeIncident.h"

#include <boost/format.hpp>

// local
#include "RunChangeHandlerSvc.h"

DECLARE_SERVICE_FACTORY( RunChangeHandlerSvc )

//-----------------------------------------------------------------------------
// Implementation file for class : RunChangeHandlerSvc
//
// 2008-07-24 : Marco Clemencic
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RunChangeHandlerSvc::RunChangeHandlerSvc(const std::string &name, ISvcLocator *svcLoc):
  Service(name,svcLoc),
  m_currentRun(0),
  m_evtSvc(0),m_detSvc(0),m_incSvc(0),m_ums(0),m_evtProc(0)
{
  declareProperty("Conditions", m_condDesc,
   "Map defining what to use to replace the location of the source XML files.");
}

//=============================================================================
// Destructor
//=============================================================================
RunChangeHandlerSvc::~RunChangeHandlerSvc() {}

//=============================================================================
// IInterface implementation
//=============================================================================
StatusCode RunChangeHandlerSvc::queryInterface(const InterfaceID& riid, void** ppvUnknown){
  if ( IIncidentListener::interfaceID().versionMatch(riid) ) {
    *ppvUnknown = (IIncidentListener*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode RunChangeHandlerSvc::initialize(){
  // base class initialization
  StatusCode sc = Service::initialize();
  if (!sc.isSuccess()) return sc;
  // local initialization
  MsgStream log(msgSvc(),name());
  if( log.level() <= MSG::DEBUG )
    log << MSG::DEBUG << "--- initialize ---" << endmsg;

  incidentSvc()->addListener(this, IncidentType::RunChange);
  // ensure that we can call evtProc() and updMgrSvc() while in handle
  evtProc();
  updMgrSvc();

  // Prepare the list of conditions
  CondDescMap::iterator condDesc;
  for (condDesc = m_condDesc.begin(); condDesc != m_condDesc.end(); ++condDesc) {
    m_conditions.push_back(CondData(detectorSvc(),condDesc->first,condDesc->second));
    updMgrSvc()->registerCondition(this,condDesc->first);
  }
  // FIXME: (MCl) This is a hack to be sure that the UMS knows about all the
  // objects we have to modify before we get to the first event.
  return updMgrSvc()->update(this);
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode RunChangeHandlerSvc::finalize(){
  // local finalization
  MsgStream log(msgSvc(),name());
  if( log.level() <= MSG::DEBUG )
    log << MSG::DEBUG << "--- finalize ---" << endmsg;

  if (m_incSvc)
    incidentSvc()->removeListener(this, IncidentType::RunChange);

  // release acquired interfaces
  release(m_evtSvc);
  release(m_detSvc);
  release(m_incSvc);
  release(m_ums);
  release(m_evtProc);

  // base class finalization
  return Service::finalize();
}

//=========================================================================
// Handle RunChange incident
//=========================================================================
void RunChangeHandlerSvc::handle(const Incident &inc) {
  MsgStream log( msgSvc(), name() );
  if( log.level() <= MSG::DEBUG )
    log << MSG::DEBUG << inc.type() << " incident received" << endmsg;

  const RunChangeIncident* rci = dynamic_cast<const RunChangeIncident*>(&inc);
  if (!rci) {
    log << MSG::ERROR << "Cannot dynamic_cast the incident to RunChangeIncident, "
                         " run change ignored" << endmsg;
    return;
  }
  
  if (m_currentRun != rci->runNumber()) {
    if( log.level() <= MSG::DEBUG )
      log << MSG::DEBUG << "Change of run number detected " << m_currentRun;
    m_currentRun = rci->runNumber();
    if( log.level() <= MSG::DEBUG )
      log << "->" << m_currentRun << endmsg;

    // loop over the object to update
    Conditions::iterator cond;
    for (cond = m_conditions.begin(); cond != m_conditions.end(); ++cond) {
      update(*cond);
    }
  }
}

//=========================================================================
// Handle RunChange incident
//=========================================================================
void RunChangeHandlerSvc::update(CondData &cond){
  // get the object and its registry
  if (cond.object) {
    IRegistry *reg = cond.object->registry();
    if (reg) {
      // get the opaque address
      IOpaqueAddress *addr = reg->address();
      if (addr) {
        // This is a bit of a hack, but it is the only way of replacing the
        // URL to use for an object.
        std::string* par = const_cast<std::string*>(addr->par());
        par[0] = (boost::format(cond.pathTemplate) % m_currentRun).str();
        // notify the UMS and the object that they have to be updated.
        cond.object->forceUpdateMode();
        updMgrSvc()->invalidate(cond.object.ptr());
        // exit to avoid the trap at the end of the function
        return;
      }
    }
  }
  // If we get here, it means that we cannot manipulate the address
  evtProc()->stopRun(); // schedule a stop
  throw GaudiException("Cannot modify address for object at " + cond.object.path(), name(), StatusCode::FAILURE);
}
