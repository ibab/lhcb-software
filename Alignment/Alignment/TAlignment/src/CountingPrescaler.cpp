// $Id: CountingPrescaler.cpp,v 1.2 2009-07-07 12:30:41 wouter Exp $



// Gaudi
#include "GaudiKernel/AlgFactory.h"

// track interfaces
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

class CountingPrescaler: public GaudiAlgorithm {

public:

  // Constructors and destructor
  CountingPrescaler(const std::string& name,
		 ISvcLocator* pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();
private:
  size_t m_interval ;
  size_t m_offset ;
  size_t m_printfreq ;
  size_t m_counter ;
};

DECLARE_ALGORITHM_FACTORY( CountingPrescaler )

CountingPrescaler::CountingPrescaler(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator),
  m_counter(0)
{
  declareProperty("Interval",m_interval = 1 ) ;
  declareProperty("Offset",m_offset = 0 ) ;
  declareProperty("PrintFreq",m_printfreq = 0 ) ;
}

StatusCode CountingPrescaler::initialize()
{
  info() << "Interval, offset = " << m_interval << ", " << m_offset << endreq ;
  m_counter = 0 ;
  return GaudiAlgorithm::initialize() ;
}

StatusCode CountingPrescaler::execute()
{
  bool passed =  (m_counter >= m_offset) && ((m_counter - m_offset)%m_interval == 0 ) ;
  setFilterPassed ( passed ) ;
  
  if( passed &&
      m_printfreq > 0 &&
      (((m_counter - m_offset)/m_interval)%m_printfreq ==0 ) ) 
    info() << "Interval=" << m_interval 
	   << " Offset=" << m_offset 
	   << " Accepting event : " << m_counter << endreq ;
  
  ++m_counter ;
  return StatusCode::SUCCESS ;
}
