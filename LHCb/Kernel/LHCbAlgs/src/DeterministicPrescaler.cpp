#include "DeterministicPrescaler.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from LHCb core
#include "Event/ODIN.h"

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DeterministicPrescaler );

DeterministicPrescaler::DeterministicPrescaler(const std::string& name, ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator) 
{
  declareProperty( "AcceptPattern" , m_prescaleSpec = "" ) ;
}

DeterministicPrescaler::~DeterministicPrescaler( )
{
}

StatusCode
DeterministicPrescaler::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( !sc) return sc;
  
  // scan m_prescaleSpec and fill m_prescale accordingly
  // add support for 'repeats' at a later point...
  for (std::string::const_iterator  i=m_prescaleSpec.begin();
       i!=m_prescaleSpec.end(); ++i ) {
    if(*i!='A'&&*i!='R') {
      err()  << "Prescale pattern should be consists of a string of 'A' and 'R'"
             << " to indicate Accept and Reject" << endmsg;
      return StatusCode::FAILURE;
    }
    m_prescale.push_back(*i=='A');
  }
  if (!m_prescale.empty()) {
    info() << "Prescaling events according to the pattern "
           << m_prescale << "[evtnum % " << m_prescale.size() << "]" << endmsg;
  }
  return sc;
}

StatusCode
DeterministicPrescaler::execute()
{
  // Get event number from the ODIN bank
  LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
  ulonglong evtNumber = odin->eventNumber();

  bool accept = m_prescale.empty() || m_prescale[evtNumber%m_prescale.size()];
  setFilterPassed(accept);
  debug() << " event # " << evtNumber << " : " << (accept?"Accepted":"Rejected") << endmsg ;
  return StatusCode::SUCCESS;
}
