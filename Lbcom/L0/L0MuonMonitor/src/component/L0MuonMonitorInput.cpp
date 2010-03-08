// $Id: L0MuonMonitorInput.cpp,v 1.1 2010-03-08 15:12:53 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/ODIN.h"

// local
#include "L0MuonMonitorInput.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMonitorInput
//
// 2010-02-25 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonMonitorInput );


//================================================================================================================================
// Standard constructor, initializes variables
//================================================================================================================================
L0MuonMonitorInput::L0MuonMonitorInput( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty( "EnableTAE" , m_enableTAE = true  );
  declareProperty( "UseNZS",     m_useNZS    = false );
  declareProperty( "L0Context" , m_l0Context = ""  );
}
//================================================================================================================================
// Destructor
//================================================================================================================================
L0MuonMonitorInput::~L0MuonMonitorInput() {} 

//================================================================================================================================
// Initialization
//================================================================================================================================
StatusCode L0MuonMonitorInput::initialize() {
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
  
  // Tools
  m_muonBuffer  = tool<IMuonRawBuffer>    ("MuonRawBuffer"    ,"InputMonMuonTool"   ,this);
  m_inputTool   = tool<IL0MuonInputTool>  ("L0MuonInputTool"  ,"InputMonL0MuonTool" ,this);
  m_olerrorTool = tool<IL0MuonOLErrorTool>("L0MuonOLErrorTool","InputMonOLErrorTool",this); 

  // Histogram
  m_summary = book1D("Summary_of_L0Muon_input_errors",-0.5,1.5,2);

  return StatusCode::SUCCESS;
}

//================================================================================================================================
// Main execution
//================================================================================================================================
StatusCode L0MuonMonitorInput::execute() {

  StatusCode sc;

  //debug() << "==> Execute" << endmsg;

  setFilterPassed(false);

  counter("EventTot")++;
  if (!m_enableTAE){
    sc = Compare();
    if (sc.isFailure()) return Error("Comparison failed",StatusCode::SUCCESS,20);
  } else {
    sc = CompareTAE();
    if (sc.isFailure()) return Error("Comparison failed (TAE mode)",StatusCode::SUCCESS,20);
  }
  
  return StatusCode::SUCCESS;
}

//================================================================================================================================
//  Finalize
//================================================================================================================================
StatusCode L0MuonMonitorInput::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//================================================================================================================================
// Compare
//================================================================================================================================
StatusCode L0MuonMonitorInput::Compare()
{

  StatusCode sc = StatusCode::SUCCESS;

  // Optical links in error
  sc = m_olerrorTool->getTiles(m_optlinks);
  if (sc.isFailure()) return Error("Failed to get list of optical links in error",StatusCode::SUCCESS,20);
  
  // Hits from muon
  std::vector<LHCb::MuonTileID> muontiles;
  sc = getMuonTiles(muontiles);
  if (sc.isFailure() ) return Error( "Unable to get muon tiles",StatusCode::FAILURE,20);
  
  // Hits from L0Muon
  std::vector<LHCb::MuonTileID> l0muontiles;
  sc = m_inputTool->getTiles(l0muontiles);
  if (sc.isFailure() ) return Error( "Unable to get l0muon tiles",StatusCode::FAILURE,20);

  // Comparison
  bool diff = areDifferent(muontiles,l0muontiles);

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
StatusCode L0MuonMonitorInput::CompareTAE()
{

  StatusCode sc = StatusCode::SUCCESS;

  int tae_size = 0;
  if (exist<LHCb::ODIN>(LHCb::ODINLocation::Default,false)) {
    // TAE size from odin
    LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default,false);
    tae_size = int(odin->timeAlignmentEventWindow());
  } else {
    Warning("ODIN not found at "+LHCb::ODINLocation::Default+", TAE mode requested but not used"
            ,StatusCode::FAILURE,50).ignore();
  }

  bool diff_tae = false;

  int ntae=0;
  for (int itae = -1*tae_size; itae<=tae_size; ++itae){
    std::string rootInTes = m_tae_items[itae];

    // Optical links in error
    sc = m_olerrorTool->getTiles(m_optlinks,rootInTes);
    if (sc.isFailure()) {
      Warning("Failed to get list of optical links in error",StatusCode::SUCCESS,20).ignore();
      continue;
    }
    
    // Hits from muon
    std::vector<LHCb::MuonTileID> muontiles;
    sc= getMuonTiles(muontiles,rootInTes);
    if( sc.isFailure() ) {
      Warning( "Unable to get Muon tiles for "+rootInTes,StatusCode::FAILURE,20).ignore();
      continue;
    }

    // Hits from l0muon
    std::vector<LHCb::MuonTileID> l0muontiles;
    sc = m_inputTool->getTiles(l0muontiles, rootInTes);
    if( sc.isFailure() ) {
      Warning( "Unable to get L0Muon tiles for "+rootInTes,StatusCode::FAILURE,20).ignore();
      continue;
    }
    
    // Comparison
    bool diff = areDifferent(muontiles,l0muontiles);
    diff_tae |= diff;

    // --------------------------------------------------------------------------
    if ( msgLevel(MSG::DEBUG) ) { 
      if (diff) {
        LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default, false);
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
// getMuonTiles
//================================================================================================================================
StatusCode L0MuonMonitorInput::getMuonTiles(std::vector<LHCb::MuonTileID> & tiles, std::string rootInTes)
{
  StatusCode sc = StatusCode::SUCCESS;

  IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
  if (!prop) return Error( "Unable to locate IProperty interface of MuonRawBuffer",StatusCode::FAILURE,20);
  sc = prop->setProperty( "RootInTES", rootInTes );
  if( sc.isFailure() ) return Error( "Unable to set RootInTES property of MuonRawBuffer",StatusCode::FAILURE,20);

  if (m_useNZS) {
    std::vector<std::pair<LHCb::MuonTileID,unsigned int> > tilesAndTDC;
    sc = m_muonBuffer->getNZSupp(tilesAndTDC);
    if (sc.isFailure() ) return Error( "Unable to get muon tiles from NZS banks (MuonRawBuffer Tool)",StatusCode::FAILURE,20);
    for(std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator it=tilesAndTDC.begin();it<tilesAndTDC.end();++it){
      tiles.push_back(it->first);
    }
  }
  else {
    sc = m_muonBuffer->getTile(tiles);
    if (sc.isFailure() ) return Error( "Unable to get muon tiles (MuonRawBuffer Tool)",StatusCode::FAILURE,20);
  }
  m_muonBuffer->forceReset();

  sc = prop->setProperty( "RootInTES", "" );
  if( sc.isFailure() ) return Error( "Unable to reset RootInTES property of MuonRawBuffer",StatusCode::FAILURE,20);

  return StatusCode::SUCCESS;
}

//================================================================================================================================
// perform comparison 
//================================================================================================================================
bool L0MuonMonitorInput::areDifferent(std::vector<LHCb::MuonTileID> & muontiles, std::vector<LHCb::MuonTileID> & l0muontiles)
{
  bool difference = false;

  // 1. check that all muon hits are match to a l0muon hit
  for (std::vector<LHCb::MuonTileID>::iterator itmuon=muontiles.begin(); itmuon<muontiles.end();++itmuon){
    
    // Discard the tile if it belongs to an optical link in error.
    bool discard = false;
    for (std::vector<LHCb::MuonTileID>::iterator itol=m_optlinks.begin();itol<m_optlinks.end();++itol) {
      LHCb::MuonTileID inter = itol->intercept(*itmuon);
      if (inter.isValid()) {
        discard = true;
        break;
      }
    }
    if (discard) continue;
    
    std::vector<LHCb::MuonTileID>::iterator itl0muon_matched = l0muontiles.end();
    for (std::vector<LHCb::MuonTileID>::iterator itl0muon=l0muontiles.begin(); itl0muon<l0muontiles.end();++itl0muon){
      if ( (*itmuon) == (*itl0muon) ) {
        itl0muon_matched = itl0muon;
        break;
      }
    }
    if (itl0muon_matched<l0muontiles.end()) {
      l0muontiles.erase(itl0muon_matched);
    } else {
      difference = true;
    }
  }
  
  // 2. check that all l0muon hits have been matched
  if (l0muontiles.size()>0) difference = true;

  return difference;
  
}

