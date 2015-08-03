// $Id: HltRoutingBitsFilter.cpp,v 1.5 2010-04-07 16:59:08 jpalac Exp $
// Include files 
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h" 
#include "Event/RawEvent.h" 
#include "Event/RawBank.h" 

#include "DAQKernel/DecoderAlgBase.h" 


class HltRoutingBitsFilter : public Decoder::AlgBase {
public: 
  HltRoutingBitsFilter( const std::string& name, ISvcLocator* pSvcLocator );
  ~HltRoutingBitsFilter() override = default; ///< Destructor
  StatusCode initialize() override;    ///< Algorithm initialisation
  StatusCode execute   () override;    ///< Algorithm execution
private:
  std::vector<unsigned int> m_r,m_v;
  
};



//-----------------------------------------------------------------------------
// Implementation file for class : HltRoutingBitsFilter
//
// 2008-07-29 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltRoutingBitsFilter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltRoutingBitsFilter::HltRoutingBitsFilter( const std::string& name,
                                        ISvcLocator* pSvcLocator)
: Decoder::AlgBase ( name , pSvcLocator )
{

  declareProperty("VetoMask", m_v = std::vector<unsigned int>(3, 0x0));
  declareProperty("RequireMask", m_r = std::vector<unsigned int>(3, 0xFFFF));
  
  //new for decoders, initialize search path, and then call the base method
  m_rawEventLocations = {LHCb::RawEventLocation::Trigger, LHCb::RawEventLocation::Copied, LHCb::RawEventLocation::Default};
  initRawEventSearch();
  

}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HltRoutingBitsFilter::initialize() {
  StatusCode sc = Decoder::AlgBase::initialize(); // must be executed first
  if (m_v.size()!=3) {
    return Error("Property VetoMask should contain exactly 3 unsigned integers");
  }
  if (m_r.size()!=3) {
    return Error("Property RequireMask should contain exactly 3 unsigned integers");
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltRoutingBitsFilter::execute() {

  
  LHCb::RawEvent* rawEvent = findFirstRawEvent();
  if( ! rawEvent ){
    setFilterPassed(true);
    return Error("No RawEvent found at any location",
                 StatusCode::SUCCESS,0);
  }  

  const std::vector<LHCb::RawBank*>& banks = rawEvent->banks(LHCb::RawBank::HltRoutingBits);
  if (banks.size()!=1) {
    setFilterPassed(true);
    return Error("Unexpected # of HltRoutingBits rawbanks",
                 StatusCode::SUCCESS,0);
  }
  if (banks.front()->size()!=3*sizeof(unsigned int)) {
    setFilterPassed(true);
    return Error("Unexpected HltRoutingBits rawbank size",
                 StatusCode::FAILURE,0);
  }
  const unsigned int *data = banks.front()->data();

  
  bool veto = false;
  bool req  = false;
  for (unsigned i=0;i<3 && !veto; ++i) {
        veto = veto || ( data[i] & m_v[i] );
        req  = req  || ( data[i] & m_r[i] );
  }
  bool accept = ( req & !veto );
  counter("#accept") += accept;
  setFilterPassed( accept );

  return StatusCode::SUCCESS;
}
