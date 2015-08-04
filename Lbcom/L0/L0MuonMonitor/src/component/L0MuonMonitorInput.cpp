// Include files 
#include <fstream>

#include "Event/ODIN.h"
#include "Event/RawBankReadoutStatus.h"
#include "MuonDet/MuonDAQHelper.h"

// ROOT
#include "GaudiUtils/Aida2ROOT.h"
#include "TH1D.h"
#include "TAxis.h"

// local
#include "L0MuonMonitorInput.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMonitorInput
//
// 2010-02-25 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonMonitorInput )


//================================================================================================================================
// Standard constructor, initializes variables
//================================================================================================================================
L0MuonMonitorInput::L0MuonMonitorInput( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_summary(0)
  , m_muonBuffer(0)
  , m_inputTool(0)
  , m_olerrorTool(0)
{
  declareProperty( "EnableTAE" , m_enableTAE = true  );
  declareProperty( "UseNZS",     m_useNZS    = false );
  declareProperty( "L0Context" , m_l0Context = ""  );
  declareProperty( "Offline"   , m_offline   = false );
  std::vector<std::string> empty;
  declareProperty( "MaskedOptLinks" ,m_forcedOL = empty);
  declareProperty( "MaskedChannels" ,m_maskedCh = empty);
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
  m_muonBuffer  = tool<IMuonRawBuffer>    ("MuonRawBuffer"    ,"InputMuonTool"   ,this);
  m_inputTool   = tool<IL0MuonInputTool>  ("L0MuonInputTool"  ,"InputL0MuonTool" ,this);
  m_olerrorTool = tool<IL0MuonOLErrorTool>("L0MuonOLErrorTool","InputOLErrorTool",this); 

  // Histogram
  m_summary = book1D("Summary_of_L0Muon_input_errors",-0.5,3.5,4);

  TH1D * hist = Gaudi::Utils::Aida2ROOT::aida2root( m_summary );
  if (hist==0) return Error("Can not get TH1D for PT_GeV",StatusCode::SUCCESS);
  TAxis* axis = hist -> GetXaxis();
  axis -> SetBinLabel(1,"Processed");
  axis -> SetBinLabel(2,"Truncated");
  axis -> SetBinLabel(3,"Bad");
  axis -> SetBinLabel(4,"Skipped");

  if ( fullDetail() ){
    m_padsLayout[0]=MuonLayout(24,8);
    m_padsLayout[1]=MuonLayout(48,8);
    m_padsLayout[2]=MuonLayout(48,8);
    m_padsLayout[3]=MuonLayout(12,8);
    m_padsLayout[4]=MuonLayout(12,8);
    for (int sta=0; sta<5; ++sta) {
      std::stringstream station("");
      station<<"M"<<(sta+1);
      int xgrid=m_padsLayout[sta].xGrid();
      int ygrid=m_padsLayout[sta].yGrid();
      for (int reg=0; reg<4; ++reg){
        int f=2*(1<<reg);
        std::stringstream region("");
        region<<"R"<<(reg+1);
        m_pads[sta][reg] = book2D("Bad_"+station.str()+"_"+region.str()
                                  ,-xgrid*f,xgrid*f,4*xgrid,-ygrid*f,ygrid*f,4*ygrid);
      }
    }
  }

  // Build list of forced optical links
  for (std::vector<std::string>::iterator itol=m_forcedOL.begin();itol<m_forcedOL.end();++itol){
    LHCb::MuonTileID mid(*itol);
    m_optlinksForced.push_back(mid);
  }
  if ( msgLevel(MSG::DEBUG) ) {
    debug()<<"List of optical links forced to one"<<endmsg;
    for (std::vector<LHCb::MuonTileID>::iterator itol=m_optlinksForced.begin();itol<m_optlinksForced.end();++itol) {
      debug()<<"\tForced OL: "<<itol->toString()<<endmsg;
    }
  }
  // Build list of masked channels
  for (std::vector<std::string>::iterator itol=m_maskedCh.begin();itol<m_maskedCh.end();++itol){
    LHCb::MuonTileID mid(*itol);
    m_channelsMasked.push_back(mid);
  }
  if ( msgLevel(MSG::DEBUG) ) {
    debug()<<"List of channels masked in the monitoring of inputs"<<endmsg;
    for (std::vector<LHCb::MuonTileID>::iterator itol=m_channelsMasked.begin();itol<m_channelsMasked.end();++itol) {
      debug()<<"\tForced OL: "<<itol->toString()<<endmsg;
    }
  }
  
  
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
  } else {
    sc = CompareTAE();
  }
  
  if (sc.isFailure()) {
    fill(m_summary,3,1.);
    return Error("Comparison failed",StatusCode::SUCCESS,20);
  }
  
  return StatusCode::SUCCESS;
}

//================================================================================================================================
//  Finalize
//================================================================================================================================
StatusCode L0MuonMonitorInput::finalize() {

  debug() << "==> Finalize" << endmsg;

  if (fullDetail() && m_offline){
    std::ofstream fout;
    fout.open("L0MuonMonitorInput.out");
    for (std::set<LHCb::MuonTileID>::iterator ittile=m_bad_tiles.begin(); ittile!=m_bad_tiles.end(); ++ittile){
      fout<<ittile->toString()<<"\n";
    }
    fout.close();
  }

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
  if (sc.isFailure()) return Error("Failed to get list of optical links in error",StatusCode::FAILURE,20);
  
  // Hits from muon
  std::vector<LHCb::MuonTileID> muontiles;
  bool truncated = false;
  sc = getMuonTiles(muontiles,truncated);
  if (sc.isFailure() ) return Error( "Unable to get muon tiles",StatusCode::FAILURE,20);
  
  // Hits from L0Muon
  std::vector<LHCb::MuonTileID> l0muontiles;
  sc = m_inputTool->getTiles(l0muontiles);
  if (sc.isFailure() ) return Error( "Unable to get l0muon tiles",StatusCode::FAILURE,20);

  // Comparison
  bool diff = false;
  if (!truncated) diff = areDifferent(muontiles,l0muontiles);

  // Histogram
  counter("EventProcessed")++;
  fill(m_summary,0,1.);
  if (truncated) {
    counter("MuonTruncated")++;
    fill(m_summary,1,1.);
  }
  if (diff) {
    counter("EventBad")++;
    fill(m_summary,2,1.);
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

  // Fill maps with unmatched channel
  if (fullDetail() && diff){
    fillMaps(muontiles);
    fillMaps(l0muontiles);
    
    if (m_offline){
      std::vector<LHCb::MuonTileID>::iterator ittile;
      for (ittile=muontiles.begin();ittile<muontiles.end(); ++ittile){
        m_bad_tiles.insert( *ittile );
      }
      for (ittile=l0muontiles.begin();ittile<l0muontiles.end(); ++ittile){
        m_bad_tiles.insert( *ittile );
      }
    }
    
  }
    
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
  bool truncated_tae = false;

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
    bool truncated = false;
    sc= getMuonTiles(muontiles,truncated,rootInTes);
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
    bool diff = false;
    if (!truncated) diff = areDifferent(muontiles,l0muontiles);
    truncated_tae |= truncated;
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
    
    // Fill maps with unmatched channel
    if (fullDetail() && diff){
      fillMaps(muontiles);
      fillMaps(l0muontiles);
    }
    
    ++ntae;
  }
  if (ntae==0) return Error( "No valid time slice found",StatusCode::FAILURE,20);

  // Histogram
  counter("EventProcessed")++;
  fill(m_summary,0,1.);
  if (truncated_tae) {
    counter("MuonTruncated")++;
    fill(m_summary,1,1.);
  }
  if (diff_tae) {
    counter("EventBad")++;
    fill(m_summary,2,1.);
  }
  
  if (diff_tae) setFilterPassed(true);
  return StatusCode::SUCCESS;
}

//================================================================================================================================
// getMuonTiles
//================================================================================================================================
StatusCode L0MuonMonitorInput::getMuonTiles(std::vector<LHCb::MuonTileID> & tiles, bool & truncated, std::string rootInTes)
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
    LHCb::RawBankReadoutStatus ro_status = m_muonBuffer->status();
    if (ro_status.status() != LHCb::RawBankReadoutStatus::OK) 
      return Error( "Readout out status of muon is not OK",StatusCode::FAILURE,20);
    for (unsigned int TellNum=0;TellNum<MuonDAQHelper_maxTell1Number;TellNum++){
      for (unsigned int linkNum=0;linkNum<MuonDAQHelper_linkNumber;linkNum++){
        truncated |= m_muonBuffer->LinkReachedHitLimit(TellNum,linkNum);
      }
    }
  }
  
  m_muonBuffer->forceReset();

  sc = prop->setProperty( "RootInTES", "" );
  if( sc.isFailure() ) return Error( "Unable to reset RootInTES property of MuonRawBuffer",StatusCode::FAILURE,20);

  return StatusCode::SUCCESS;
}

//================================================================================================================================
// perform comparison 
// - returns true if differences between the 2 list have been found, 
// - modify the input list such that they contains only the hits which are missing in the other.
//================================================================================================================================
bool L0MuonMonitorInput::areDifferent(std::vector<LHCb::MuonTileID> & muontiles, std::vector<LHCb::MuonTileID> & l0muontiles)
{
  bool difference = false;
  std::vector<LHCb::MuonTileID> muontiles_tmp;

  // 0. remove tiles from optical links forced to 1 or from masked channels
  std::vector<LHCb::MuonTileID> l0muontiles_tmp;
  l0muontiles_tmp.reserve(l0muontiles.size());
  for (std::vector<LHCb::MuonTileID>::iterator itl0muon=l0muontiles.begin(); itl0muon<l0muontiles.end();++itl0muon){
    bool discard = false;
    for (std::vector<LHCb::MuonTileID>::iterator itol=m_optlinksForced.begin();itol<m_optlinksForced.end();++itol) {
      LHCb::MuonTileID inter = itol->intercept(*itl0muon);
      if (inter.isValid()) {
        discard = true;
        break;
      }
    }
    for (std::vector<LHCb::MuonTileID>::iterator itch=m_channelsMasked.begin();itch<m_channelsMasked.end();++itch) {
      if ( (*itl0muon) == (*itch)) {
        discard = true;
        break;
      }
    }
    if (!discard) l0muontiles_tmp.push_back(*itl0muon);
  }
  l0muontiles = l0muontiles_tmp;

  // 1. check that all muon hits are match to a l0muon hit
  for (std::vector<LHCb::MuonTileID>::iterator itmuon=muontiles.begin(); itmuon<muontiles.end();++itmuon){
    // Discard the tile if it belongs to an optical link either in error 
    bool discard = false;
    for (std::vector<LHCb::MuonTileID>::iterator itol=m_optlinks.begin();itol<m_optlinks.end();++itol) {
      LHCb::MuonTileID inter = itol->intercept(*itmuon);
      if (inter.isValid()) {
        discard = true;
        break;
      }
    }
    // Discard the tile if it belongs to an optical link forced to 1
    for (std::vector<LHCb::MuonTileID>::iterator itol=m_optlinksForced.begin();itol<m_optlinksForced.end();++itol) {
      LHCb::MuonTileID inter = itol->intercept(*itmuon);
      if (inter.isValid()) {
        discard = true;
        break;
      }
    }
    // Discard the tile if it is masked
    for (std::vector<LHCb::MuonTileID>::iterator itch=m_channelsMasked.begin();itch<m_channelsMasked.end();++itch) {
      if ( (*itmuon) == (*itch)) {
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
      muontiles_tmp.push_back(*itmuon);
      difference = true;
    }
  }
  
  // 2. check that all l0muon hits have been matched
  if (l0muontiles.size()>0) difference = true;

  // 3. finalize list of unmatched muon hits
  muontiles.clear();
  muontiles.insert(muontiles.end(), muontiles_tmp.begin(), muontiles_tmp.end());

  return difference;
  
}

void L0MuonMonitorInput::fillMaps(std::vector<LHCb::MuonTileID> & tiles)
{
  for (std::vector<LHCb::MuonTileID>::iterator ittile=tiles.begin(); ittile<tiles.end(); ++ittile){
    int sta = ittile->station();
    int qua = ittile->quarter();
    int reg = ittile->region();
    int f = 1<<reg;
    
    std::vector<LHCb::MuonTileID> pads = m_padsLayout[sta].tiles(*ittile);
    for (std::vector<LHCb::MuonTileID>::iterator itpad=pads.begin(); itpad<pads.end(); ++itpad){
      int X   = itpad->nX();
      int Y   = itpad->nY();
      double x = X+0.5;
      if ( (qua==2) || (qua==3) ) x = -x;
      double y = Y+0.5;
      if ( (qua==1) || (qua==2) ) y = -y;
      fill(m_pads[sta][reg],x*f,y*f,1.);
    }
  }
}

