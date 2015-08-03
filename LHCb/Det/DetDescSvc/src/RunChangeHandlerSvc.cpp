// Include files
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"

#include "DetDesc/ValidDataObject.h"
#include "DetDesc/RunChangeIncident.h"

#include <boost/format.hpp>

// local
#include "RunChangeHandlerSvc.h"

DECLARE_SERVICE_FACTORY( RunChangeHandlerSvc )

#define ON_DEBUG if (msgLevel(MSG::DEBUG))
#define DEBUG_MSG ON_DEBUG debug()


//-----------------------------------------------------------------------------
// Implementation file for class : RunChangeHandlerSvc
//
// 2008-07-24 : Marco Clemencic
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RunChangeHandlerSvc::RunChangeHandlerSvc(const std::string &name, ISvcLocator *svcLoc):
  base_class(name,svcLoc),
  m_currentRun(0)
{
  declareProperty("Conditions", m_condDesc,
   "Map defining what to use to replace the location of the source XML files.");
}

//=============================================================================
// Destructor
//=============================================================================
RunChangeHandlerSvc::~RunChangeHandlerSvc() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode RunChangeHandlerSvc::initialize(){
  // base class initialization
  StatusCode sc = Service::initialize();
  if (!sc.isSuccess()) return sc;

  // local initialization
  DEBUG_MSG << "--- initialize ---" << endmsg;

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
  DEBUG_MSG << "--- finalize ---" << endmsg;

  if (m_incSvc)
    incidentSvc()->removeListener(this, IncidentType::RunChange);

  // release acquired interfaces
  m_evtSvc.reset();
  m_detSvc.reset();
  m_incSvc.reset();
  m_ums.reset();
  m_evtProc.reset();

  // base class finalization
  return Service::finalize();
}

//=========================================================================
// Handle RunChange incident
//=========================================================================
void RunChangeHandlerSvc::handle(const Incident &inc) {
  DEBUG_MSG << inc.type() << " incident received" << endmsg;

  const RunChangeIncident* rci = dynamic_cast<const RunChangeIncident*>(&inc);
  if (!rci) {
    error() << "Cannot dynamic_cast the incident to RunChangeIncident, "
               "run change ignored" << endmsg;
    return;
  }

  if (m_currentRun != rci->runNumber()) {
    ON_DEBUG {
      DEBUG_MSG << "Change of run number detected " << m_currentRun;
      DEBUG_MSG << "->" << rci->runNumber() << endmsg;
    }
  } else {
    DEBUG_MSG << "Update requested without change of run (flushing XML cache)." << endmsg;
    xmlParser()->clearCache();
  }

  m_currentRun = rci->runNumber();
  // update objects
  update();
}

//=========================================================================
// Flag for update all conditions
//=========================================================================
void RunChangeHandlerSvc::update(){
  std::for_each(m_conditions.begin(), m_conditions.end(),
                [this](CondData &cond) {this->update(cond);});
}

//=========================================================================
// Flag for update one condition
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
	if( cond.pathTemplate.find("%") != std::string::npos )
	  par[0] = (boost::format(cond.pathTemplate) % m_currentRun).str();
	else
	  par[0] = cond.pathTemplate ;

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
