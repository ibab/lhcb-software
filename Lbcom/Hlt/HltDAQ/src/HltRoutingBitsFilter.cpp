// $Id: HltRoutingBitsFilter.cpp,v 1.3 2009-10-14 16:27:22 cattanem Exp $
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
};



//-----------------------------------------------------------------------------
// Implementation file for class : HltRoutingBitsFilter
//
// 2008-07-29 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltRoutingBitsFilter );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltRoutingBitsFilter::HltRoutingBitsFilter( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("VetoMask", m_v = boost::assign::list_of(0x0)(0x0)(0x0));
  declareProperty("RequireMask", m_r = boost::assign::list_of(0xFFFF)(0xFFFF)(0xFFFF));

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

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltRoutingBitsFilter::execute() {

  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
  const std::vector<LHCb::RawBank*>& banks = rawEvent->banks(LHCb::RawBank::HltRoutingBits);
  if (banks.size()!=1) {
    setFilterPassed(true);
    return Error("Unexpected # of HltRoutingBits rawbanks",StatusCode::SUCCESS,0);
  }
  if (banks.front()->size()!=3*sizeof(unsigned int)) {
    setFilterPassed(true);
    return Error("Unexpected HltRoutingBits rawbank size",StatusCode::FAILURE,0);
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
