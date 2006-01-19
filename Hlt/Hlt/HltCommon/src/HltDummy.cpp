// $Id: HltDummy.cpp,v 1.1.1.1 2006-01-19 10:01:59 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "HltDummy.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltDummy
//
// 2006-01-16 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<HltDummy>          s_factory ;
const        IAlgFactory& HltDummyFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDummy::HltDummy( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("HltSummaryLocation", 
                  m_hltSummaryLocation = HltSummaryLocation::Default );
  declareProperty("BoxID", m_boxID = (int) HltSummaryBox::HltBoxUnknown);
  declareProperty("Filter",m_filter = true);
}
//=============================================================================
// Destructor
//=============================================================================
HltDummy::~HltDummy() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltDummy::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize" << endmsg;
  
  debug() << " Dummy Trigger box " << m_boxID << endreq;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltDummy::execute() {

  debug() << "==> Execute" << endmsg;

  m_entries += 1; 
  HltSummary* hltSum = getOrCreate<HltSummary,HltSummary>(m_hltSummaryLocation);
  HltSummaryBox* boxSum = new HltSummaryBox( (HltSummaryBox::HltBox) m_boxID);
  hltSum->setHltSummaryBox( (HltSummaryBox::HltBox) m_boxID, boxSum);

  bool ok = true;
  boxSum->setDecision(ok);

  if (m_filter) setFilterPassed(ok);
  if (ok) m_passed +=1;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltDummy::finalize() {

  debug() << "==> Finalize" << endmsg;

  info() << " Trigger Box ID = \t " << m_boxID << endreq;
  info() << "         entries     = \t " << m_entries << endreq;
  info() << "         passed      = \t " << m_passed << endreq;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
