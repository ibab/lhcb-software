// $Id: L0MuonMonitor.cpp,v 1.5 2008-02-19 09:40:15 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
 
#include "Kernel/MuonLayout.h"
// from Event
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

  setLayouts();
  
  for (int iq=0; iq<4;++iq){
    
    // Book hitmaps
    setHistoDir("L0Muon/Hitmaps");
    for (int reg =0; reg<4; ++reg){
      for (int sta =0; sta<5; ++sta){
        for (L0MuonMonitor::Channel_type type =L0MuonMonitor::Pad; type<L0MuonMonitor::nb_channel_types; type++){
          name = histoName(type,reg,sta,iq);
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
  std::string location;
  std::string name;
  
  // Get L0Muon Hits
  location = rootInTES() + LHCb::L0MuonDataLocation::Default + m_extension;
  if (  exist<LHCb::L0MuonDatas>(location ) ) {
    std::vector<LHCb::MuonTileID> ddigits;
    LHCb::L0MuonDatas* pdatas = get<LHCb::L0MuonDatas>( location );
    LHCb::L0MuonDatas::const_iterator itdata;
    for (itdata = pdatas->begin() ; itdata!=pdatas->end() ; ++itdata){
      LHCb::MuonTileID mkey = (*itdata)->key();    
      std::vector<LHCb::MuonTileID> ols = (*itdata)->ols();
      if (ols.size()>0) {
        for (std::vector<LHCb::MuonTileID>::iterator itol=ols.begin(); itol!=ols.end(); ++itol){
          ddigits.push_back(*itol);
        }
      }
    }

    // Fill hitmap
    std::vector<LHCb::MuonTileID>::iterator itddigits;
    for (itddigits=ddigits.begin();itddigits<ddigits.end();++itddigits){
      int sta = itddigits->station();
      int reg = itddigits->region();
      MuonLayout lay = itddigits->layout();
      for (L0MuonMonitor::Channel_type type =L0MuonMonitor::Pad; type<L0MuonMonitor::nb_channel_types; type++){
        if (lay==m_channel_layout[type].stationLayout(sta).regionLayout(reg)) {
          int x = itddigits->nX();
          int y = itddigits->nY();
          int qua = itddigits->quarter();
          name = histoName(type,reg,sta,qua);
          fill(histo2D(name),x,y,1,name);
        }
      }
    }
  }

  // Get L0Muon candidates from controller board
  location = rootInTES() + LHCb::L0MuonCandidateLocation::Default + m_extension;
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
  location = rootInTES() + LHCb::L0MuonCandidateLocation::BCSU + m_extension;
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
  location = rootInTES() + LHCb::L0MuonCandidateLocation::PU + m_extension;
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
  location = rootInTES() + LHCb::L0MuonInfoLocation::Default + m_extension;
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

  int ibin;
  
  location = rootInTES() + LHCb::L0MuonCtrlErrorLocation::Default + m_extension;
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
  
  location = rootInTES() + LHCb::L0MuonProcErrorLocation::Default + m_extension;
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
