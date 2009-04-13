#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "HltBase/IHltReferenceClockSvc.h"

class HltFixedRateScaler : public GaudiAlgorithm
{
public:
  enum IntervalType { PERIODIC, RANDOM } ;
  HltFixedRateScaler(const std::string& name,
		     ISvcLocator* pSvcLocator) ;
  StatusCode initialize() ;
  StatusCode execute() ;
private:
  const IHltReferenceClockSvc* m_referenceClockSvc ;
  const IRndmEngine* m_random ;
  double m_outputRate ;
  int m_intervalType ;
  double m_tickInterval ;
  double m_nextTick ;
} ;

/********************************************************************/

#include "GaudiKernel/AlgFactory.h"

DECLARE_ALGORITHM_FACTORY( HltFixedRateScaler ) ;

HltFixedRateScaler::HltFixedRateScaler( const std::string& name,
					ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_referenceClockSvc(0),
    m_random(0),
    m_outputRate(5/*Gaudi::Units::Hz*/),
    m_intervalType(RANDOM),
    m_tickInterval(1),
    m_nextTick(1)
{
  declareProperty("OutputRate",m_outputRate = 5/*Gaudi::Units::Hz*/) ;
  declareProperty("IntervalType",m_intervalType = RANDOM) ;
}


StatusCode
HltFixedRateScaler::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if (sc.isSuccess() ) {
    // get the service
    m_referenceClockSvc = 0 ;
    if( !service( "HltReferenceClockSvc", m_referenceClockSvc).isSuccess()) {
      error() << "Cannot locate HltReferenceClockSvc" << endreq ;
      sc = StatusCode::FAILURE;
    } else {
      // compute the interval
      double refrate = m_referenceClockSvc->rate() ;
      // If the rate is not positive, every event will be vetoed.
      if( m_outputRate > 0 ) m_tickInterval = refrate / m_outputRate ;
      // always accept the first event (can be changed)
      m_nextTick = m_referenceClockSvc->tick() ;
    }
    
    m_random = 0 ;
    if( m_intervalType != PERIODIC ) {
      IRndmGenSvc* rndmGenSvc ;
      if(!service( "RndmGenSvc", rndmGenSvc).isSuccess()) {
	error() << "Cannot locate RndmGenSvc" << endreq ;
	sc = StatusCode::FAILURE;
      } else {
	m_random = rndmGenSvc->engine() ;
      }
    }
  }
  return sc ;
}

#include <cmath>

StatusCode
HltFixedRateScaler::execute()
{
  StatusCode rc = StatusCode::SUCCESS ;
  bool accept = false ;
  if( m_outputRate > 0 ) {
    size_t currenttick   = m_referenceClockSvc->tick() ;
    if( (accept = (currenttick >= m_nextTick)) )
      // note: this still works if m_tickInterval<1 !
      m_nextTick += 
	m_intervalType==PERIODIC 
	? m_tickInterval
	: m_tickInterval * -std::log(m_random->rndm()) ;
  }
  setFilterPassed( accept ) ;
  return StatusCode::SUCCESS ;
}
