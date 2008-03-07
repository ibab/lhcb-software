// $Id: L0MuonMonitor.cpp,v 1.6 2008-03-07 15:34:42 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
 
#include "Kernel/MuonLayout.h"
// from Event
#include "Event/ODIN.h"
#include "Event/L0MuonData.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0MuonInfo.h"
#include "Event/L0MuonProcError.h"
#include "Event/L0MuonCtrlError.h"

// local
#include "L0MuonMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMonitor
//
// 2008-01-22 : 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMonitor::L0MuonMonitor( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  m_muonBuffer = 0;

  declareProperty("Extension"     , m_extension = ""  ); 

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonMonitor::~L0MuonMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonMonitor::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  std::string name;
  
  debug() << "==> Initialize" << endmsg;

  m_odin = tool<IEventTimeDecoder>("OdinTimeDecoder","OdinDecoder",this);

  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer","MuonRawTool",this);

  setLayouts();
  
    
  // Book hitmaps
  
  setHistoDir("L0Muon/Hitmaps");
  bookHitMaps();
  bookHitMaps("Muon");
  bookHitMaps("MuonOnly");
  bookHitMaps("L0MuonOnly");
  
  for (int iq=0; iq<4;++iq){
    // Book histos for candidates
    setHistoDir("L0Muon/Candidates");
    if (iq==1) {
      book1D("NCands_per_event_ctrl","NCands_per_event_ctrl",-0.5,0.5+2*4,1+2*4);
      book1D("NCands_per_event_bcsu","NCands_per_event_bcsu",-0.5,0.5+2*12*4,1+2*12*4);
      book1D("NCands_per_event_pu","NCands_per_event_pu",-0.5,0.5+2*48*4*4,1+2*48*4);
    }
    
    for (int reg =0; reg<4; ++reg){
      MuonLayout lay = MuonLayout(48,8);
      int nx=lay.xGrid()*2;
      int ny=lay.yGrid()*2;
      // 1 2D histo per region with the ctrl candidate's seed location 
      name = quarterName(iq)+"_Seed_"+regionName(reg)+"_ctrl";
      book2D(name,name,-0.5,nx-0.5,nx,-0.5,ny-0.5,ny);
      // 1 2D histo per region with the bcsu candidate's seed location 
      name = quarterName(iq)+"_Seed_"+regionName(reg)+"_bcsu";
      book2D(name,name,-0.5,nx-0.5,nx,-0.5,ny-0.5,ny);
      // 1 2D histo per region with the pu candidate's seed location 
      name = quarterName(iq)+"_Seed_"+regionName(reg)+"_pu";
      book2D(name,name,-0.5,nx-0.5,nx,-0.5,ny-0.5,ny);
    }
  }
  // Histos
  setHistoDir("L0Muon/Decoding");
  // Histogram the ref event number and BId
  name = "L0EventNumber";
  book1D(name,name,-0.5,-0.5+4096,4096);
  name = "L0_B_Id";
  book1D(name,name,-0.5,-0.5+3564,3564);
  name = "Decoding_errors";
  book1D(name,name,-0.5,-0.5+13*4,13*4);
  name = "L0EventNumber_errors";
  book1D(name,name,-0.5,-0.5+4096,4096);
  name = "L0_B_Id_errors";
  book1D(name,name,-0.5,-0.5+3564,3564);
  
  // Record errors : 
  // * 1 bin per TELL1
  name = "L0Muon_Error";
  book1D(name,name,-0.5,-0.5+6.,6);

  // Ctrl boards
  // * 2 bins per quarter (1 for CU and 1 for SU) 
  name = "CtrlError_L0EventNumber";
  book1D(name,name,-0.5,-0.5+8.,8);
  name = "CtrlError_L0_B_Id";
  book1D(name,name,-0.5,-0.5+8.,8);
  name = "CtrlError_BoardIndex";
  book1D(name,name,-0.5,-0.5+8.,8);
  name = "CtrlError_BCID_CU_SU";
  book1D(name,name,-0.5,-0.5+8.,8);  
  // * 2 bins per BCSU (1 for CU and 1 for SU) 
  name = "CtrlError_BCID_BCSU";
  book1D(name,name,-0.5,-0.5+96.,96);
  name = "CtrlError_pb_link";
  book1D(name,name,-0.5,-0.5+96.,96);
  
  // Proc boards
  // * 2 bins per board (1 per frame)
  name = "ProcError_L0EventNumber";
  book1D(name,name,-0.5,-0.5+96.,96);
  name = "ProcError_L0_B_Id";
  book1D(name,name,-0.5,-0.5+96.,96);
  name = "ProcError_BoardIndex";
  book1D(name,name,-0.5,-0.5+96.,96);
  name = "ProcError_BCID_BCSU";
  book1D(name,name,-0.5,-0.5+48.,48);
  name = "ProcError_BCID_PU";
  book1D(name,name,-0.5,-0.5+192.,192);
  name = "ProcError_opt_link";
  book1D(name,name,-0.5,-0.5+192.,192);
  name = "ProcError_ser_link";
  book1D(name,name,-0.5,-0.5+192.,192);
  name = "ProcError_par_link";
  book1D(name,name,-0.5,-0.5+192.,192);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonMonitor::execute() {

  debug() << "==> Execute" << endmsg;  

  // get ODIN
  //debug() << " Get ODIN object for reference slot " <<endreq;
  m_odin->getTime();
  long event=0;
  long run=0 ;
  long bunch=0;
  if( exist<LHCb::ODIN>( LHCb::ODINLocation::Default) ){
    LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default);
    debug() << " ODIN : RUN "<< odin->runNumber() <<" EVENT "<< odin->eventNumber() <<" BID "<< odin->bunchId() << endreq;
    run   = odin->runNumber();
    event =odin->eventNumber() ;
    bunch =odin->bunchId() ;
  } else {
    info() << " ODIN : not found" << endreq;
  }
  
  // Get the Muon Hits
  std::vector<LHCb::MuonTileID> muontiles = muonTiles();
  std::vector<LHCb::MuonTileID>::iterator itmuontiles;
  
  debug() << "Got "<<muontiles.size() <<" digits from the Muon detector "<<endmsg;
  for (itmuontiles=muontiles.begin(); itmuontiles!=muontiles.end(); ++itmuontiles){
    debug() << "\t "<<itmuontiles->toString() <<endmsg;
  }
  // Fill hitmap
  fillHitMaps(muontiles,"Muon");
  
  std::string location;
  std::string name;
  
  // Get L0Muon Hits
  std::vector<LHCb::MuonTileID> l0muontiles;
  std::vector<LHCb::MuonTileID>::iterator itl0muontiles;
  location = LHCb::L0MuonDataLocation::Default + m_extension;
  if (  exist<LHCb::L0MuonDatas>(location ) ) {
    LHCb::L0MuonDatas* pdatas = get<LHCb::L0MuonDatas>( location );
    LHCb::L0MuonDatas::const_iterator itdata;
    for (itdata = pdatas->begin() ; itdata!=pdatas->end() ; ++itdata){
      LHCb::MuonTileID mkey = (*itdata)->key();    
      std::vector<LHCb::MuonTileID> ols = (*itdata)->ols();
      if (ols.size()>0) {
        for (std::vector<LHCb::MuonTileID>::iterator itol=ols.begin(); itol!=ols.end(); ++itol){
          if (!itol->isValid()){
            info()<<"PU "<<mkey.toString()<<" tile is not valid"<<endreq;
          }
          l0muontiles.push_back(*itol);
        }
      }
    }
    debug() << "Got "<<l0muontiles.size() <<" digits from the L0Muon trigger"<<endmsg;
    fillHitMaps(l0muontiles);
    
    std::vector<LHCb::MuonTileID> muontilesOnly;
    for (itmuontiles=muontiles.begin(); itmuontiles!=muontiles.end(); ++itmuontiles){
      bool match=false;
      for (itl0muontiles=l0muontiles.begin();itl0muontiles<l0muontiles.end();++itl0muontiles){
        if ( (*itmuontiles) == (*itl0muontiles) ) {
          match = true;
          break;
        } 
      }
      if (!match) {
        debug()<<"muon hit: "<<itmuontiles->toString()<<" not found in L0Muon" <<endreq;
        muontilesOnly.push_back(*itmuontiles);
      }
    }
    fillHitMaps(muontilesOnly,"MuonOnly");
    
    std::vector<LHCb::MuonTileID> l0muontilesOnly;
    for (itl0muontiles=l0muontiles.begin(); itl0muontiles!=l0muontiles.end(); ++itl0muontiles){
      bool match=false;
      for (itmuontiles=muontiles.begin();itmuontiles<muontiles.end();++itmuontiles){
        if ( (*itl0muontiles) == (*itmuontiles) ) {
          match = true;
          break;
        } 
      }
      if (!match) {
        debug()<<"L0Muon hit: "<<itl0muontiles->toString()<<" not found in Muon" <<endreq;
        l0muontilesOnly.push_back(*itl0muontiles);
      }
    }
    fillHitMaps(l0muontilesOnly,"L0MuonOnly");

  }

  // Get L0Muon candidates from controller board
  location = LHCb::L0MuonCandidateLocation::Default + m_extension;
  if (  exist<LHCb::L0MuonCandidates>(location ) ) {
    LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>( location );
    LHCb::L0MuonCandidates::const_iterator itcand;
    name = "NCands_per_event_ctrl";
    fill(histo1D(name),cands->size(),1,name);
    for ( itcand= cands->begin(); itcand<cands->end();++itcand) {
      LHCb::MuonTileID seed = (*itcand)->muonTileIDs(2)[0];
      int x = seed.nX();
      int y = seed.nY();
      int reg = seed.region();
      int qua = seed.quarter();
      name = quarterName(qua)+"_Seed_"+regionName(reg)+"_ctrl";
      fill(histo2D(name),x,y,1,name);
    }
  }

  // Get L0Muon candidates from BCSU
  location = LHCb::L0MuonCandidateLocation::BCSU + m_extension;
  if (  exist<LHCb::L0MuonCandidates>(location ) ) {
    LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>( location );
    LHCb::L0MuonCandidates::const_iterator itcand;
    name = "NCands_per_event_bcsu";
    fill(histo1D(name),cands->size(),1,name);
    for ( itcand= cands->begin(); itcand<cands->end();++itcand) {
      LHCb::MuonTileID seed = (*itcand)->muonTileIDs(2)[0];
      int x = seed.nX();
      int y = seed.nY();
      int reg = seed.region();
      int qua = seed.quarter();
      name = quarterName(qua)+"_Seed_"+regionName(reg)+"_bcsu";
      fill(histo2D(name),x,y,1,name);
    }
  }

  // Get L0Muon candidates from PU
  location = LHCb::L0MuonCandidateLocation::PU + m_extension;
  if (  exist<LHCb::L0MuonCandidates>(location ) ) {
    LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>( location );
    LHCb::L0MuonCandidates::const_iterator itcand;
    name = "NCands_per_event_pu";
    fill(histo1D(name),cands->size(),1,name);
    for ( itcand= cands->begin(); itcand<cands->end();++itcand) {
      LHCb::MuonTileID seed = (*itcand)->muonTileIDs(2)[0];
      int x = seed.nX();
      int y = seed.nY();
      int reg = seed.region();
      int qua = seed.quarter();
      //       info() << "PU Candidates Q"<<qua+1<<" "<<seed.toString()<< endreq;
      name = quarterName(qua)+"_Seed_"+regionName(reg)+"_pu";
      fill(histo2D(name),x,y,1,name);
    }
  }

  
  //
  int L0EventNumber=-1;
  int L0_B_Id=-1;
  location = LHCb::L0MuonInfoLocation::Default + m_extension;
  if (exist<LHCb::L0MuonInfo>(location) ){
    LHCb::L0MuonInfo* pl0minfo = get<LHCb::L0MuonInfo>(location);
    L0EventNumber=pl0minfo->L0EventNumber();
    L0_B_Id=pl0minfo->L0_B_Id();
  }
  name = "L0EventNumber";
  fill(histo1D(name),L0EventNumber,1,name);
  name = "L0_B_Id";
  fill(histo1D(name),L0_B_Id,1,name);

  // If no reference has been found, return
  if ( (L0EventNumber==-1) || (L0_B_Id==-1) ) return StatusCode::SUCCESS;

  if (L0EventNumber!=(event&0xFFF)){
    name = "L0EventNumber_errors";
    fill(histo1D(name),event,1,name);
    info()<<"Event number discrepancy ! "<<"L0Ref= "<<L0EventNumber<<" -VS- ODIN "<<event<<endreq;
  }
  if (L0_B_Id!=(bunch&0xFFF)){
    name = "L0_B_Id_errors";
    fill(histo1D(name),event,1,name);
    info()<<"Bunch ID  ! "<<std::hex<<"L0Ref= 0x"<<L0_B_Id<<" -VS- ODIN 0x"<<bunch<<std::dec<<endreq;
  }
  
  int ibin;
  
  location = LHCb::L0MuonCtrlErrorLocation::Default + m_extension;
  if (exist<LHCb::L0MuonCtrlErrors>(location) ){ //If container found in TES
    LHCb::L0MuonCtrlErrors* perrors = get<LHCb::L0MuonCtrlErrors>(location);
    LHCb::L0MuonCtrlErrors::iterator iterror;
    for (iterror = perrors->begin() ; iterror!=perrors->end() ; ++iterror){ // Loop over ctrl errors
      int iq = (*iterror)->key();
      if ( (*iterror)->decoding() ){ // If decoding error detected
        name = "Decoding_errors";
        fill(histo1D(name),iq*13+12,1,name);
      } else { // If NO decoding error detected
        int header=(*iterror)->header();
        for (int ich=0; ich<2; ++ich){
          ibin = ich*4+iq;
          name = "CtrlError_L0EventNumber";
          fill(histo1D(name),ibin,(header>>(0+(ich*3)))&1,name);
          name = "CtrlError_L0_B_Id";
          fill(histo1D(name),ibin,(header>>(1+(ich*3)))&1,name);
          name = "CtrlError_BoardIndex";
          fill(histo1D(name),ibin,(header>>(2+(ich*3)))&1,name);
        }
        for (int ib=0; ib<12; ++ib){ // Loop over BCSUs
          for (int ich=0; ich<2; ++ich){ // Loop over CU and SU frames
            if ((*iterror)->bcsu_bcid_error(ib,ich)!=-1) {
              ibin = ich*48 + iq*12 + ib;
              name = "CtrlError_BCID_BCSU";   
              fill(histo1D(name),ibin,1,name);
            }
          }// End of loop over CU and SU frames
          if ((*iterror)->cu_link_error(ib)>0) {
            ibin = 0*48 + iq*12 + ib;
            name = "CtrlError_pb_link";   
            fill(histo1D(name),ibin,1,name);
          }
          if ((*iterror)->su_link_error(ib)>0) {
            ibin = 1*48 + iq*12 + ib;
            name = "CtrlError_pb_link";   
            fill(histo1D(name),ibin,1,name);
          }
        }// End of loop over BCSUs
        if ((*iterror)->cu_bcid_error()!=-1){
          name = "d_CtrlError_BCID_CU_SU";
          fill(histo1D(name),iq,1,name);
        }
        if ((*iterror)->su_bcid_error()!=-1){
          name = "d_CtrlError_BCID_CU_SU";
          fill(histo1D(name),iq+4,1,name);
        }
      } // End if NO decoding error detected
    } // End of loop over ctrl errors
  } // End if container found in TES
  
  location =  LHCb::L0MuonProcErrorLocation::Default + m_extension;
  if (exist<LHCb::L0MuonProcErrors>(location) ){ // If container found in TES
    LHCb::L0MuonProcErrors* perrors = get<LHCb::L0MuonProcErrors>(location);
    LHCb::L0MuonProcErrors::iterator iterror;
    for (iterror = perrors->begin() ; iterror!=perrors->end() ; ++iterror){ // Loop over proc errors
      LHCb::MuonTileID mid = (*iterror)->key();
      int iq = mid.quarter();
      int ib = (*iterror)->board_index();
      if ( (*iterror)->decoding() ){// If decoding error detected
        name = "Decoding_errors";
        fill(histo1D(name),iq*13+ib,1,name);
      } else {// If NO decoding error detected
        int header=(*iterror)->header();
        for (int ich=0; ich<2; ++ich){ // Loop over the proc board transmission channels 
          ibin = ich*4+iq;
          name = "ProcError_L0EventNumber";
          fill(histo1D(name),ibin,(header>>(0+(ich*3)))&1,name);
          name = "ProcError_L0_B_Id";
          fill(histo1D(name),ibin,(header>>(1+(ich*3)))&1,name);
          name = "ProcError_BoardIndex";
          fill(histo1D(name),ibin,(header>>(2+(ich*3)))&1,name);
        } // End of loop over the proc board transmission channels 
        if ((*iterror)->bcsu_bcid_error()!=-1){
          ibin =iq*12 + ib;
          name = "ProcError_BCID_BCSU";
          fill(histo1D(name),ibin,1,name);
        }
        for (int ipu=0; ipu<4; ++ipu){// Loop over the 4 PUs of a proc board
          if ((*iterror)->pu_bcid_error(ipu)!=-1){
            ibin = iq*48 + ib*4 + ipu;
            name = "ProcError_BCID_PU";
            fill(histo1D(name),ibin,1,name);
          }
          if ((*iterror)->pu_opt_link_error(ipu)!=-1){
            ibin = iq*48 + ib*4 + ipu;
            name = "ProcError_opt_link";
            fill(histo1D(name),ibin,1,name);
          }
          if ((*iterror)->pu_ser_link_error(ipu)!=-1){
            ibin = iq*48 + ib*4 + ipu;
            name = "ProcError_ser_link";
            fill(histo1D(name),ibin,1,name);
          }
          if ((*iterror)->pu_par_link_error(ipu)!=-1){
            ibin = iq*48 + ib*4 + ipu;
            name = "ProcError_par_link";
            fill(histo1D(name),ibin,1,name);
          }
        } // End of loop over the 4 PUs of a proc board
      } // End if NO decoding error detected
    } // End of loop over proc errors
  } // End if container found in TES

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

void L0MuonMonitor::bookHitMaps(std::string sid){
  
  std::string name;
  
 
  for (int iq=0; iq<4;++iq){
    for (int reg =0; reg<4; ++reg){
      for (int sta =0; sta<5; ++sta){
        for (L0MuonMonitor::Channel_type type =L0MuonMonitor::Pad; type<L0MuonMonitor::nb_channel_types; type++){
          name = histoName(type,reg,sta,iq,sid);
          MuonLayout lay = m_channel_layout[type].stationLayout(sta).regionLayout(reg);
          if (lay.isDefined()){  
            int nx=lay.xGrid()*2;
            int ny=lay.yGrid()*2;
            // 1 2D histo per region and per station with the location of the hits
            book2D(name,name,-0.5,nx-0.5,nx,-0.5,ny-0.5,ny);
          }
        }
      }
    }
  }
  
}

void L0MuonMonitor::fillHitMaps(std::vector<LHCb::MuonTileID> tiles,std::string sid){

  std::vector<LHCb::MuonTileID>::iterator it;
  std::string name;
  
  for (it=tiles.begin();it<tiles.end();++it){
    int sta = it->station();
    int reg = it->region();
    MuonLayout lay = it->layout();
    for (L0MuonMonitor::Channel_type type =L0MuonMonitor::Pad; type<L0MuonMonitor::nb_channel_types; type++){
      if (lay==m_channel_layout[type].stationLayout(sta).regionLayout(reg)) {
        int x = it->nX();
        int y = it->nY();
        int qua = it->quarter();
        name = histoName(type,reg,sta,qua,sid);
        fill(histo2D(name),x,y,1,name);
      }
    }
  }

}



std::vector<LHCb::MuonTileID> L0MuonMonitor::muonTiles(){

  // Get the Muon Hits
  std::vector<LHCb::MuonTileID> ddigits;

  // Use the MuonRawBufer tool to produce the MuonDigits from the raw buffer (DaVinci)
  debug() << "Getting Muon hits from Muon Raw Buffer "<<m_muonBuffer<<endmsg;

  if(!m_muonBuffer) {
    error() << "No pointer to muon raw buffer interface tool"<<m_muonBuffer<<endmsg;
  } else {
    m_muonBuffer->getTile(ddigits);
  } 
  return ddigits;
  
}


void L0MuonMonitor::setLayouts()
{
  MuonSystemLayout pad_layout =MuonSystemLayout(MuonStationLayout(MuonLayout(24,8)),
                                                MuonStationLayout(MuonLayout(0,0)),
                                                MuonStationLayout(MuonLayout(0,0)),
                                                MuonStationLayout(MuonLayout(12,8),
                                                                  MuonLayout(0,0),
                                                                  MuonLayout(0,0),
                                                                  MuonLayout(0,0)),
                                                MuonStationLayout(MuonLayout(12,8),
                                                                  MuonLayout(0,0),
                                                                  MuonLayout(0,0),
                                                                  MuonLayout(0,0)));
  
  MuonSystemLayout stripH_layout =MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                                                   MuonStationLayout(MuonLayout(8,8),
                                                                     MuonLayout(4,8),
                                                                     MuonLayout(2,8),
                                                                     MuonLayout(2,8)),
                                                   MuonStationLayout(MuonLayout(8,8),
                                                                     MuonLayout(4,8),
                                                                     MuonLayout(2,8),
                                                                     MuonLayout(2,8)),
                                                   MuonStationLayout(MuonLayout(0,0),
                                                                     MuonLayout(4,8),
                                                                     MuonLayout(2,8),
                                                                     MuonLayout(2,8)),
                                                   MuonStationLayout(MuonLayout(0,0),
                                                                     MuonLayout(4,8),
                                                                     MuonLayout(2,8),
                                                                     MuonLayout(2,8)));
  
  MuonSystemLayout stripV_layout =MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                                                   MuonStationLayout(MuonLayout(48,1),
                                                                     MuonLayout(48,2),
                                                                     MuonLayout(48,2),
                                                                     MuonLayout(48,2)),
                                                   MuonStationLayout(MuonLayout(48,1),
                                                                     MuonLayout(48,2),
                                                                     MuonLayout(48,2),
                                                                     MuonLayout(48,2)),
                                                   MuonStationLayout(MuonLayout(0,0),
                                                                     MuonLayout(12,2),
                                                                     MuonLayout(12,2),
                                                                     MuonLayout(12,2)),
                                                   MuonStationLayout(MuonLayout(0,0),
                                                                     MuonLayout(12,2),
                                                                     MuonLayout(12,2),
                                                                     MuonLayout(12,2)));

  m_channel_layout[L0MuonMonitor::Pad]=pad_layout;
  m_channel_layout[L0MuonMonitor::StripV]=stripV_layout;
  m_channel_layout[L0MuonMonitor::StripH]=stripH_layout;
  
}
