// $Id: HltRoutingBitsFilter.cpp,v 1.1 2009-10-09 14:13:09 graven Exp $
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
// Main execution
//=============================================================================
StatusCode HltRoutingBitsFilter::execute() {

  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
  const std::vector<LHCb::RawBank*>& banks = rawEvent->banks(LHCb::RawBank::HltRoutingBits);
  if (banks.size()!=1) {
    return Error("Unexpected # of rawbanks",StatusCode::FAILURE,10);
  }
  if (banks.front()->size()!=3*sizeof(unsigned int)) {
    return Error("Unexpected rawbank size",StatusCode::FAILURE,10);
  }
  const unsigned int *data = banks.front()->data();

  if (m_v.size()!=3) {
        return Error("Property VetoMask should contain exactly 3 unsigned integers",StatusCode::FAILURE,10);
  }
  if (m_r.size()!=3) {
        return Error("Property RequireMask should contain exactly 3 unsigned integers", StatusCode::FAILURE,10);
  }
  
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
