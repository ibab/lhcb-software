// $Id: HltEntry.cpp,v 1.1.1.1 2006-01-19 10:01:59 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "HltEntry.h"

#include "Event/L0DUReport.h"
#include "Event/L0Trig.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltEntry
//
// 2006-01-16 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<HltEntry>          s_factory ;
const        IAlgFactory& HltEntryFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltEntry::HltEntry( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

  // counters
  m_entries = 0;
  m_passed = 0;

  // posibilities: "muon","hadron","calo"
  declareProperty("BoxID", m_boxID = (int) (HltSummaryBox::HltBoxUnknown) );
  declareProperty("L0Location ", m_l0Location = L0DUReportLocation::Default );
  declareProperty("SummaryLocation ", 
                  m_hltSummaryLocation = HltSummaryLocation::Default );

  declareProperty("Filter", m_filter = true);
  
}
//=============================================================================
// Destructor
//=============================================================================
HltEntry::~HltEntry() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltEntry::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  debug() << " Entry type " << m_boxID << endreq;

  m_entries = 0;
  m_passed = 0;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltEntry::execute() {  

  debug() << "==> Execute" << endmsg;

  m_entries += 1; 
  HltSummary* hltSum = getOrCreate<HltSummary,HltSummary>(m_hltSummaryLocation);
  HltSummaryBox* boxSum = new HltSummaryBox( (HltSummaryBox::HltBox) m_boxID);
  hltSum->setHltSummaryBox( (HltSummaryBox::HltBox) m_boxID,boxSum);

  bool ok = isEntry();
  boxSum->setDecision(ok);

  if (m_filter) setFilterPassed(ok);
  if (ok) m_passed +=1;

  debug() << " entry type " << m_boxID << " passed? " << ok << endreq;

  return StatusCode::SUCCESS;
};


bool HltEntry::isEntry() {

  L0DUReport* l0dec = get<L0DUReport>(m_l0Location);
  if (l0dec == NULL) return false;
  
  debug() << "LO type \t" << l0dec->typeL0TrigName() << endreq;

  if (m_boxID == HltSummaryBox::MuonEntry) {
    if(l0dec->isTrig( L0Trig::Mu1)) return true;
    if(l0dec->isTrig( L0Trig::SumMu)) return true;  
  } else if (m_boxID == HltSummaryBox::HadEntry) {
    if (l0dec->isTrig( L0Trig::Hadron ) ) return true;
  } else if (m_boxID == HltSummaryBox::CalEntry) {
    if(l0dec->isTrig( L0Trig::Electron)) return true;
    if(l0dec->isTrig( L0Trig::Photon)) return true;
    if(l0dec->isTrig( L0Trig::Pi0Local)) return true;
    if(l0dec->isTrig( L0Trig::Pi0Global)) return true;
    if(l0dec->isTrig( L0Trig::Pi0Global)) return true;
  }
  return false;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltEntry::finalize() {

  debug() << "==> Finalize" << endmsg;

  info() << " Trigger Entry Point = \t " << m_boxID << endreq;
  info() << "         entries     = \t " << m_entries << endreq;
  info() << "         passed      = \t " << m_passed << endreq;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
