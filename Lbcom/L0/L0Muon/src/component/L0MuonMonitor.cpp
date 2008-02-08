// $Id: L0MuonMonitor.cpp,v 1.3 2008-02-08 11:17:00 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
 
#include "Kernel/MuonLayout.h"
// from Event
#include "Event/L0MuonData.h"
#include "Event/L0MuonCandidate.h"

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
