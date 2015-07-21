#include "TestHistogramAlg.h"
#include "AIDA/IHistogram1D.h"
TestHistogramAlg::TestHistogramAlg(const std::string& name,
    ISvcLocator* pSvcLocator) :
    GaudiHistoAlg(name, pSvcLocator), m_hist(0)
{
}

TestHistogramAlg::~TestHistogramAlg( )
{
}
StatusCode TestHistogramAlg::initialize()
{
  m_hist = book1D("GauchoTestHistogram", 0.0, 5.0, 6);
  return StatusCode::SUCCESS;    ///< Algorithm initialization
}
StatusCode TestHistogramAlg::finalize() ///< Algorithm initialization
{

  return StatusCode::SUCCESS;    ///< Algorithm initialization
}
StatusCode TestHistogramAlg::execute()    ///< Algorithm execution
{
  m_hist->fill(double(0.5));
  m_hist->fill(double(1.5), double(0.5));
  m_hist->fill(double(2.5), double(0.25));
  m_hist->fill(double(3.5), double(0.125));
  return StatusCode::SUCCESS;    ///< Algorithm initialization
}
StatusCode TestHistogramAlg::stop() ///<Transition executed upon the Stop command
{
  return StatusCode::SUCCESS;    ///< Algorithm initialization
}
StatusCode TestHistogramAlg::start() ///<Transition executed upon the Start command
{
  return StatusCode::SUCCESS;    ///< Algorithm initialization
}
/// virtuals incident
void TestHistogramAlg::handle(const Incident&)
{
}
StatusCode TestHistogramAlg::queryInterface(const InterfaceID& id, void** ppI)
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
DECLARE_COMPONENT(TestHistogramAlg)
//}
