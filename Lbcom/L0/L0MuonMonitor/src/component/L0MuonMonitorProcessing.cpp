// Include files 

// from Gaudi
#include "Event/ODIN.h"
#include "Event/L0MuonCandidate.h"

// ROOT
#include "GaudiUtils/Aida2ROOT.h"
#include "TH1D.h"
#include "TAxis.h"

// local
#include "L0MuonMonitorProcessing.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMonitorProcessing
//
// 2010-02-25 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonMonitorProcessing )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMonitorProcessing::L0MuonMonitorProcessing( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_summary(0)
{
  declareProperty( "EnableTAE" , m_enableTAE = false  );
  declareProperty( "L0Context1", m_l0Context1 = "");
  declareProperty( "L0Context2", m_l0Context2 = "replay");
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonMonitorProcessing::~L0MuonMonitorProcessing() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonMonitorProcessing::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  debug() << "==> Initialize" << endmsg;

  // TAE slots names
  if (m_enableTAE){
    m_tae_items[-7] = "Prev7/";
    m_tae_items[-6] = "Prev6/";
    m_tae_items[-5] = "Prev5/";
    m_tae_items[-4] = "Prev4/";
    m_tae_items[-3] = "Prev3/";
    m_tae_items[-2] = "Prev2/";
    m_tae_items[-1] = "Prev1/";
    m_tae_items[ 0] = "";
    m_tae_items[ 1] = "Next1/";
    m_tae_items[ 2] = "Next2/";
    m_tae_items[ 3] = "Next3/";
    m_tae_items[ 4] = "Next4/";
    m_tae_items[ 5] = "Next5/";
    m_tae_items[ 6] = "Next6/";
    m_tae_items[ 7] = "Next7/";
  }

  // Histogram
  m_summary = book1D("Summary_of_L0Muon_processing_errors",-0.5,1.5,2);

  TH1D * hist = Gaudi::Utils::Aida2ROOT::aida2root( m_summary );
  if (hist==0) return Error("Can not get TH1D for PT_GeV",StatusCode::SUCCESS);
  TAxis* axis = hist -> GetXaxis();
  axis -> SetBinLabel(1,"Processed");
  axis -> SetBinLabel(2,"Bad");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonMonitorProcessing::execute() {

  StatusCode sc;

  //debug() << "==> Execute" << endmsg;
  
  counter("EventTot")++;
  setFilterPassed(false);

  if (!m_enableTAE){
    sc = Compare();
    if (sc.isFailure()) return Error("Comparison failed",StatusCode::SUCCESS,20);
  } else {
    sc = CompareTAE();
    if (sc.isFailure()) return Error("Comparison failed (TAE mode)",StatusCode::SUCCESS,20);
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonMonitorProcessing::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//================================================================================================================================
// Compare
//================================================================================================================================
StatusCode L0MuonMonitorProcessing::Compare()
{
  
  StatusCode sc = StatusCode::SUCCESS;
  
  std::string location1;
  std::string location2;

  // Candidates sent to the L0DU
  location1 = LHCb::L0MuonCandidateLocation::Default + m_l0Context1;
  location2 = LHCb::L0MuonCandidateLocation::Default + m_l0Context2;
  if (!exist<LHCb::L0MuonCandidates>( location1 ))
    return Error("L0MuonCandidates not found at "+location1,StatusCode::FAILURE,20);
  if (!exist<LHCb::L0MuonCandidates>( location2 ))
    return Error("L0MuonCandidates not found at "+location2,StatusCode::FAILURE,20);
  bool diff = areDifferent(location1, location2);

  // Candidates found by the PU
  location1 = LHCb::L0MuonCandidateLocation::PU + m_l0Context1;
  location2 = LHCb::L0MuonCandidateLocation::PU + m_l0Context2;
  if (!exist<LHCb::L0MuonCandidates>( location1 ))
    return Error("L0MuonCandidates not found at "+location1,StatusCode::FAILURE,20);
  if (!exist<LHCb::L0MuonCandidates>( location2 ))
    return Error("L0MuonCandidates not found at "+location2,StatusCode::FAILURE,20);
  diff |= areDifferent(location1, location2);

  // Histogram
  counter("EventProcessed")++;
  fill(m_summary,0,1.);
  if (diff) {
    counter("EventBad")++;
    fill(m_summary,1,1.);
  }
  
  // ----------------------------------------------------------------------------
  if ( msgLevel(MSG::DEBUG) ) {
    if (diff) {
      LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
      debug()<<"Compare : difference found"
             <<", run: "<<odin->runNumber()
             <<", event: "<<odin->eventNumber()
             <<", bid: "<<odin->bunchId()
             << endmsg;
    }
  }
  // ----------------------------------------------------------------------------

  if (diff) setFilterPassed(true);
  return StatusCode::SUCCESS;
  
}

//================================================================================================================================
// Compare (TAE mode)
//================================================================================================================================
StatusCode L0MuonMonitorProcessing::CompareTAE()
{

  StatusCode sc = StatusCode::SUCCESS;

  std::string location1;
  std::string location2;

  bool diff_tae = false;

  int tae_size = 0;
  if (exist<LHCb::ODIN>(LHCb::ODINLocation::Default,false)) {
    // TAE size from odin
    LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default,false);
    tae_size = int(odin->timeAlignmentEventWindow());
  } else {
    Warning("ODIN not found at "+LHCb::ODINLocation::Default+", TAE mode requested but not used"
            ,StatusCode::FAILURE,50).ignore();
  }

  int ntae=0;
  for (int itae = -1*tae_size; itae<=tae_size; ++itae){

    std::string rootInTes = m_tae_items[itae];
    sc=setProperty("RootInTES",rootInTes);
    if (sc.isFailure()) {
      Error("Can not set RootInTES ",StatusCode::FAILURE,20).ignore();
      continue;
    }
    
    // Candidates sent to the L0DU
    location1 = LHCb::L0MuonCandidateLocation::Default + m_l0Context1;
    location2 = LHCb::L0MuonCandidateLocation::Default + m_l0Context2;
    if (!exist<LHCb::L0MuonCandidates>( location1 )) {
      Warning("L0MuonCandidates not found at "+rootInTes+"/..."+location1,StatusCode::FAILURE,20).ignore();
      continue;
    }
    if (!exist<LHCb::L0MuonCandidates>( location2 )) {
      Warning("L0MuonCandidates not found at "+rootInTes+"/..."+location2,StatusCode::FAILURE,20).ignore();
      continue;
    }
    bool diff = areDifferent(location1, location2);
  
    // Candidates found by the PU
    location1 = LHCb::L0MuonCandidateLocation::PU + m_l0Context1;
    location2 = LHCb::L0MuonCandidateLocation::PU + m_l0Context2;
    if (!exist<LHCb::L0MuonCandidates>( location1 )) {
      Warning("L0MuonCandidates not found at "+rootInTes+"/..."+location1,StatusCode::FAILURE,20).ignore();
      continue;
    }
    if (!exist<LHCb::L0MuonCandidates>( location2 )) {
      Warning("L0MuonCandidates not found at "+rootInTes+"/..."+location2,StatusCode::FAILURE,20).ignore();
      continue;
    }
    diff |= areDifferent(location1, location2);

    diff_tae |= diff;
    
    // --------------------------------------------------------------------------
    if ( msgLevel(MSG::DEBUG) ) { 
      if (diff) {
        LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default,false);
        debug()<<"Compare (tae) : difference found"
               <<", run: "<<odin->runNumber()
               <<", event: "<<odin->eventNumber()
               <<", bid: "<<odin->bunchId()
               <<", slot: "<<rootInTes
               << endmsg;
      }
    }
    // --------------------------------------------------------------------------

    ++ntae;
  }
  if (ntae==0) return Error( "No valid time slice found",StatusCode::FAILURE,20);
  
  // Histogram
  counter("EventProcessed")++;
  fill(m_summary,0,1.);
  if (diff_tae) {
    counter("EventBad")++;
    fill(m_summary,1,1.);
  }
  
  if (diff_tae) setFilterPassed(true);
  return StatusCode::SUCCESS;
}
//================================================================================================================================
// perform comparison
//================================================================================================================================
bool L0MuonMonitorProcessing::areDifferent(std::string location1, std::string location2)
{
  
  LHCb::L0MuonCandidates* cands1 = get<LHCb::L0MuonCandidates>( location1 );
  LHCb::L0MuonCandidates* cands2 = get<LHCb::L0MuonCandidates>( location2 );

  bool difference = false;

  LHCb::L0MuonCandidates::const_iterator it1;
  LHCb::L0MuonCandidates::const_iterator it2;

  for ( it1= cands1->begin(); it1<cands1->end();++it1) {
    bool found=false;
    for ( it2= cands2->begin(); it2<cands2->end();++it2) {
      if (**it2==**it1) {
        found=true;
        break;
      }
    }
    if (!found) {
      difference = true;
      break;
    }
  }
  
  if (difference) return difference;
  
  for ( it2= cands2->begin(); it2<cands2->end();++it2) {
    bool found=false;
    for ( it1= cands1->begin(); it1<cands1->end();++it1) {
      if (**it2==**it1) {
        found=true;
        break;
      }
    }
    if (!found) {
      difference = true;
      break;
    }
  }
  return difference;
}
//================================================================================================================================


