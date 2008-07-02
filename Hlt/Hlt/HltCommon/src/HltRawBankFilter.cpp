// $Id: HltRawBankFilter.cpp,v 1.1 2008-07-02 14:21:14 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/RawEvent.h"

// local
#include "HltRawBankFilter.h"

using namespace std;
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltRawBankFilter
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltRawBankFilter );

namespace {
struct map2type {
    RawBank::BankType operator()(const string& name) const {
        for (RawBank::BankType i = RawBank::BankType(0); 
             i < RawBank::LastType; i=RawBank::BankType(i+1)) {
            if (RawBank::typeName(i)==name) return i;
        }
        throw GaudiException("Unknown RawBank name",name,StatusCode::FAILURE);
        return RawBank::BankType(0); // never reached, keep compiler happy...
    }
};
}
void
HltRawBankFilter::BankTypeList::updateHandler(Property&) {
    m_types.clear();
    transform(m_property.value().begin(),m_property.value().end(),
              back_inserter(m_types), 
              map2type());
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltRawBankFilter::HltRawBankFilter( const string& name,
                    ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("RawBanks",   m_rawBanks.property() );
  declareProperty("RawEventLocation", m_rawEventLocation = RawEventLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
HltRawBankFilter::~HltRawBankFilter() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltRawBankFilter::initialize() {
  debug() << "==> Initialize" << endmsg;
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // no selection as input, but we do have an output selection,
  // of 'no candate' type..
  registerSelection();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltRawBankFilter::execute() {
  RawEvent *evt = get<RawEvent>(m_rawEventLocation);
  bool pass = true;
  for (BankTypeList::iter_type i=m_rawBanks.begin();  i!= m_rawBanks.end() && pass; ++i) 
        pass = !( evt->banks(*i).empty() );
  setDecision(pass);
  if (msgLevel(MSG::DEBUG)) 
        debug() << "HltRawBankFilter: " << (pass?"accept":"reject") << endreq;
  return StatusCode::SUCCESS;
};
