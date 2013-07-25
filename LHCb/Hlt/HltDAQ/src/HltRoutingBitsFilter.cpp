// $Id: HltRoutingBitsFilter.cpp,v 1.5 2010-04-07 16:59:08 jpalac Exp $
// Include files 
#include <vector>
#include "boost/assign/list_of.hpp"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiAlg/GaudiAlgorithm.h" 
#include "Event/RawEvent.h" 
#include "Event/RawBank.h" 


class HltRoutingBitsFilter : public GaudiAlgorithm {
public: 
  HltRoutingBitsFilter( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~HltRoutingBitsFilter( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialisation
  virtual StatusCode execute   ();    ///< Algorithm execution
private:
  std::vector<unsigned int> m_r,m_v;
  std::string m_inputRawEventLocation; 
  std::vector<std::string> m_rawEventLocations;
  
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
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_inputRawEventLocation("")
{

  declareProperty("VetoMask", m_v = std::vector<unsigned int>(3, 0x0));
  declareProperty("RequireMask", m_r = std::vector<unsigned int>(3, 0xFFFF));
  declareProperty("RawEventLocation", m_inputRawEventLocation);

}
//=============================================================================
// Destructor
//=============================================================================
HltRoutingBitsFilter::~HltRoutingBitsFilter() {
} 

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HltRoutingBitsFilter::initialize() {

  if (m_v.size()!=3) {
    return Error("Property VetoMask should contain exactly 3 unsigned integers");
  }
  if (m_r.size()!=3) {
    return Error("Property RequireMask should contain exactly 3 unsigned integers");
  }
  m_rawEventLocations.clear();
  if( m_inputRawEventLocation != "" )m_rawEventLocations.push_back(m_inputRawEventLocation);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Trigger);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Copied);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltRoutingBitsFilter::execute() {

  
  LHCb::RawEvent* rawEvent = 0;
  std::vector<std::string>::const_iterator iLoc = m_rawEventLocations.begin();
  for (; iLoc != m_rawEventLocations.end() ; ++iLoc ) {
    //    try RootInTES independent path first
    if (exist<LHCb::RawEvent>(*iLoc, false)) {
      rawEvent = get<LHCb::RawEvent>(*iLoc, false);
      break;
    }
    //   now try RootInTES dependent path
    if (exist<LHCb::RawEvent>(*iLoc)) {
      rawEvent = get<LHCb::RawEvent>(*iLoc);
      break;
    }
  }
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
