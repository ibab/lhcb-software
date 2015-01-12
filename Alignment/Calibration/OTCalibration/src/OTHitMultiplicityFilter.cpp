// Include files

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// from Kernel
#include "OTDAQ/IOTRawBankDecoder.h"

class OTHitMultiplicityFilter : public GaudiAlgorithm 
{
public: 
  OTHitMultiplicityFilter( const std::string& name,ISvcLocator* pSvcLocator) ;
  virtual ~OTHitMultiplicityFilter()  ;
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  virtual StatusCode initialize  ();    ///< Algorithm finalization

private:
  ToolHandle<IOTRawBankDecoder> m_otdecoder ;
  unsigned int m_maxNumOTHits ;
};

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTHitMultiplicityFilter )

OTHitMultiplicityFilter::OTHitMultiplicityFilter( const std::string& name,
			    ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator ),
  m_otdecoder{"OTRawBankDecoder"}
{
  declareProperty("RawBankDecoder",m_otdecoder) ;
  declareProperty("MaxNumOTHits",m_maxNumOTHits = 10000) ;
}

OTHitMultiplicityFilter::~OTHitMultiplicityFilter() {} 

StatusCode OTHitMultiplicityFilter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  sc = m_otdecoder.retrieve() ;
  return sc ;
}

StatusCode OTHitMultiplicityFilter::execute()
{
  unsigned int nHits = m_otdecoder->totalNumberOfHits() ;
  setFilterPassed( nHits <= m_maxNumOTHits )  ;
  return StatusCode::SUCCESS;
}

StatusCode OTHitMultiplicityFilter::finalize() 
{
  m_otdecoder.release().ignore() ;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
