#include "HistogramResetter.h"
#include "Gaucho/IGauchoMonitorSvc.h"
HistogramResetter::HistogramResetter(const std::string& name,
    ISvcLocator* pSvcLocator) :
    GaudiHistoAlg(name, pSvcLocator)
{
}

HistogramResetter::~HistogramResetter( )
{
}
StatusCode HistogramResetter::initialize()
{
  return StatusCode::SUCCESS;    ///< Algorithm initialization
}
StatusCode HistogramResetter::finalize() ///< Algorithm initialization
{

  return StatusCode::SUCCESS;    ///< Algorithm initialization
}
StatusCode HistogramResetter::execute()    ///< Algorithm execution
{
  return StatusCode::SUCCESS;    ///< Algorithm initialization
}
StatusCode HistogramResetter::stop() ///<Transition executed upon the Stop command
{
  return StatusCode::SUCCESS;    ///< Algorithm initialization
}
StatusCode HistogramResetter::start() ///<Transition executed upon the Start command
{
  SmartIF<IGauchoMonitorSvc> mon(monitorSvc());
  mon->resetHistos(this);
  return StatusCode::SUCCESS;    ///< Algorithm initialization
}
/// virtuals incident
void HistogramResetter::handle(const Incident&)
{
}
StatusCode HistogramResetter::queryInterface(const InterfaceID& id, void** ppI)
{
  if (0 == ppI)
    return StatusCode::FAILURE;
  // check ID of the interface
  if (IIncidentListener::interfaceID() != id)
    return Algorithm::queryInterface(id, ppI);
  *ppI = static_cast<IIncidentListener*>(this);
  addRef();
  return StatusCode::SUCCESS;
}
DECLARE_COMPONENT(HistogramResetter)
//}
