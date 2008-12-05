#include "MDF/MDFIO.h"
#include "Event/RawEvent.h"
#include "GaudiOnline/FIDManipAlg.h"
#include "GaudiOnline/FIDManipulator.h"

#include "GaudiKernel/AlgFactory.h" 
using namespace LHCb;
using namespace std;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(FIDManipAlg);

/// Standard constructor
FIDManipAlg::FIDManipAlg(const string& nam, ISvcLocator* pSvc) 
: Algorithm(nam,pSvc), m_printCnt(0)
{
  declareProperty("Action",       m_action=DUMP);
  declareProperty("BankLocation", m_location=RawEventLocation::Default);    
  declareProperty("DataType",     m_type=MDFIO::MDF_RECORDS);
  declareProperty("PrintFreq",    m_printFreq=100);
  declareProperty("Debug",        m_debug=0);
}

/// Algorithm initialization
StatusCode FIDManipAlg::initialize() {
  m_printCnt = 0;
  return StatusCode::SUCCESS;
}

/// Algorithm execution
StatusCode FIDManipAlg::execute() {
  time_t now  = int(time(0)/180);
  string guid = "b918f659-6973-4978-9008-6d99a1fe15e7";
  FIDManipulator manip(m_location,m_type,msgSvc(),eventSvc());
  if ( m_debug ) manip.setDebug(m_debug);

  switch ( m_action )  {
    case ADD:      {
      return manip.add(now,guid);
    }
    case REMOVE:  {
      return manip.remove();
    }
    case DUMP:
      if ( ++m_printCnt == m_printFreq ) {
	m_printCnt = 0;
	return manip.print();
      }
      return StatusCode::SUCCESS;
    default:       return StatusCode::FAILURE;
  }
  return StatusCode::FAILURE;
}
