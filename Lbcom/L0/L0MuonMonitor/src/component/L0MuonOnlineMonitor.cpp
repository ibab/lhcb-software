// $Id: L0MuonOnlineMonitor.cpp,v 1.1.1.1 2008-04-02 12:41:15 jucogan Exp $
// Include files 

#include "boost/format.hpp"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Event/ODIN.h"
#include "Event/L0MuonData.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0MuonInfo.h"
#include "Event/L0MuonProcError.h"
#include "Event/L0MuonCtrlError.h"

// local
#include "L0MuonOnlineMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonOnlineMonitor
//
// 2008-04-02 : 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonOnlineMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonOnlineMonitor::L0MuonOnlineMonitor( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  std::vector<int> time_slots;
  for (int i=-7;i<=7;++i) time_slots.push_back(i);
  declareProperty( "TimeSlots"  , m_time_slots = time_slots);

  std::vector<int> quarters;
  for (int i=0;i<4;++i) quarters.push_back(i);
  declareProperty( "Quarters"  , m_quarters = quarters);
  
  std::vector<int> stations;
  for (int i=0;i<5;++i) stations.push_back(i);
  declareProperty( "Stations"  , m_stations = stations);

  std::vector<int> regions;
  for (int i=0;i<5;++i) regions.push_back(i);
  declareProperty( "Regions"  , m_regions = regions);

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonOnlineMonitor::~L0MuonOnlineMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonOnlineMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  setLayouts();

  std::string name;

  debug() << "==> Initialize" << endmsg;

  // Book histos:
  for (int iq=0; iq<4;++iq){
    for (int i=0; i<3; ++i) m_decoding_status[iq][i]=NULL;
  }
  
  
  setHistoDir("L0Muon/Online");

  // Decoding
  for (std::vector<int>::iterator itq=m_quarters.begin(); itq<m_quarters.end(); ++itq){
    int iq = (*itq);
    name="Decoding_errors_"+quarterName(iq);
    m_decoding_status[iq][2]=book1D(name,name,-0.5,-0.5+16,16);
    name="Inconsistencies_"+quarterName(iq);
    m_decoding_status[iq][1]=book1D(name,name,-0.5,-0.5+16,16);
    name="Valid_"+quarterName(iq);
    m_decoding_status[iq][0]=book1D(name,name,-0.5,-0.5+16,16);
  }
  
  // Physical channels
  for (std::vector<int>::iterator itq=m_quarters.begin(); itq<m_quarters.end(); ++itq){
    int iq = (*itq);
    for (std::vector<int>::iterator itr=m_regions.begin(); itr<m_regions.end(); ++itr){
      int reg = (*itr);
      for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
        int sta = (*its);
        for (Channel_type type=Pad; type<nb_channel_types; type++){
          name = "Hits_"+quarterName(iq)+"_"+stationName(sta)+"_"+regionName(reg)+"_"+channelTypeName(type);
          MuonLayout lay = m_channel_layout[type].stationLayout(sta).regionLayout(reg);
          if (lay.isDefined()){  
            int nbins=lay.xGrid()*lay.yGrid()*3;
            book1D(name,name,-0.5,nbins-0.5,nbins);
          }
        }
      }
    }
  }

  // Logical channels
  for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
    int sta = (*its);
    MuonLayout lay =m_pad_layout.stationLayout(sta).regionLayout(0);
    int xgrid=lay.xGrid();
    int ygrid=lay.yGrid();
    int nx=xgrid*16;
    int ny=ygrid*16;
    name = "Pads_"+stationName(sta);
    book2D(name,name,-nx,nx,2*nx,-ny,ny,2*ny);
  }

  // Candidates
  book1D("NCands_per_event","NCands_per_event",-0.5,0.5+2*m_quarters.size(),1+2*m_quarters.size());
  book2D("NCands_per_bx","NCands_per_bx",
         (-1*int(m_time_slots.size()/2))-0.5,(int(m_time_slots.size()/2))+0.5,m_time_slots.size(),
         -0.5,0.5+2*m_quarters.size(),1+2*m_quarters.size());
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonOnlineMonitor::execute() {
  StatusCode sc;
  
  debug() << "==> Execute" << endmsg;

  int ncand=0;

  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){
    setProperty("RootInTes",timeSlot(*it_ts));
    //     if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;
    
    std::string location;
    std::string name;
    
    location = LHCb::L0MuonCtrlErrorLocation::Default ;
    if (exist<LHCb::L0MuonCtrlErrors>(location) ){ //If container found in TES
      LHCb::L0MuonCtrlErrors* perrors = get<LHCb::L0MuonCtrlErrors>(location);
      LHCb::L0MuonCtrlErrors::iterator iterror;
      for (iterror = perrors->begin() ; iterror!=perrors->end() ; ++iterror){ // Loop over ctrl errors
        int iq = (*iterror)->key();
        int header=(*iterror)->header();
        for (int i=0;i<3;++i){
          if ( ((header>>i)&1) ) {
            fill( m_decoding_status[iq][i] , 15.  , 1.);
          }
        }
      }// End of loop over ctrl errors
    } // End if container found in TES
    
    location =  LHCb::L0MuonProcErrorLocation::Default ;
    if (exist<LHCb::L0MuonProcErrors>(location) ){ // If container found in TES
      LHCb::L0MuonProcErrors* perrors = get<LHCb::L0MuonProcErrors>(location);
      LHCb::L0MuonProcErrors::iterator iterror;
      for (iterror = perrors->begin() ; iterror!=perrors->end() ; ++iterror){ // Loop over proc errors
        LHCb::MuonTileID mid = (*iterror)->key();
        int iq = mid.quarter();
        int ib = (*iterror)->board_index();
        int header=(*iterror)->header();
        for (int i=0;i<3;++i){
          if ( ((header>>i)&1) ) {
            if (m_decoding_status[iq][i]!=NULL) fill( m_decoding_status[iq][i] , ib  , 1.);
          }
          
        }
      }// End of loop over proc errors
    } // End if container found in TES
    
    
    // Get L0Muon Hits
    std::vector<LHCb::MuonTileID> l0muontiles;
    location = LHCb::L0MuonDataLocation::Default ;
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

            if (!quarterInUse(itol->quarter())) continue;
            if (!stationInUse(itol->station())) continue;
            if (!regionInUse(itol->region())  ) continue;
            
            l0muontiles.push_back(*itol);
          }
        }
      }
    } // End if container found in TES
    

    // Physical channels
    std::vector<LHCb::MuonTileID>::iterator itl0muontiles;
    for (itl0muontiles=l0muontiles.begin();itl0muontiles<l0muontiles.end();++itl0muontiles){
      int sta = itl0muontiles->station();
      int reg = itl0muontiles->region();
      MuonLayout lay = itl0muontiles->layout();
      for (Channel_type type =Pad; type<nb_channel_types; type++){
        if (lay==m_channel_layout[type].stationLayout(sta).regionLayout(reg)) {
          int xgrid=lay.xGrid();
          int ygrid=lay.yGrid();
          int x = itl0muontiles->nX();
          int y = itl0muontiles->nY();
          int qua = itl0muontiles->quarter();
          int offset = (int)(x/xgrid) +  ( ( ((int)(y/ygrid)) <<1)&2 ) -1;
          int ind = (x%xgrid) + (y%ygrid)*xgrid + offset*xgrid*ygrid;

//           debug()<<"x: "<<x<<" y: "<<y<< " offset: "<<offset<<" ind: "<<ind
//                  <<" max: "<<xgrid*ygrid*3<<" xGrid: "<<xgrid<<" yGrid: "<<ygrid
//                  <<endmsg;
          
          std::string hname = "Hits_"+quarterName(qua)+"_"+stationName(sta)+"_"+regionName(reg)+"_"+channelTypeName(type);
          AIDA::IHistogram1D *h = histo1D(hname);
          if (h!=NULL) fill(h,ind,1,name);
        }
      }
    }

    location = LHCb::L0MuonCandidateLocation::Default ;
    if (  exist<LHCb::L0MuonCandidates>(location ) ) {
      LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>( location );
      AIDA::IHistogram2D *hcand2 = histo2D(std::string("NCands_per_bx"));
      if (hcand2!=NULL) fill(hcand2,(*it_ts),cands->size(),1.);
      ncand+=cands->size();
    }

    // Logical channels
    std::vector<LHCb::MuonTileID> l0muonpads;
    sc = makePads(l0muontiles,l0muonpads);
    std::vector<LHCb::MuonTileID>::iterator itl0muonpads;
    for (itl0muonpads=l0muonpads.begin();itl0muonpads<l0muonpads.end();++itl0muonpads){
      int sta = itl0muonpads->station();
      std::string hname = "Pads_"+stationName(sta);
      AIDA::IHistogram2D *hpad2 = histo2D(hname);
      if (hpad2==NULL) continue;
      int qua = itl0muonpads->quarter();
      int X  =itl0muonpads->nX();
      int Y  =itl0muonpads->nY();
      int reg=itl0muonpads->region();
      int f=1<<reg;
      for (int ix=X*f; ix<X*f+f; ++ix){
        for (int iy=Y*f; iy<Y*f+f; ++iy){
          int x= ix;
          int y= iy;
          flipCoord(x,y,qua);
          fill(hpad2,x,y,1);
        }
      }
    }
    
  } // End of loop over time slots
  
  AIDA::IHistogram1D *hcand=histo1D(std::string("NCands_per_event"));
  if (hcand!=NULL) fill(hcand,ncand,1.);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonOnlineMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
void L0MuonOnlineMonitor::setLayouts()
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

  m_channel_layout[Pad]=pad_layout;
  m_channel_layout[StripV]=stripV_layout;
  m_channel_layout[StripH]=stripH_layout;

  m_pad_layout =MuonSystemLayout(MuonStationLayout(MuonLayout(24,8)),
                                 MuonStationLayout(MuonLayout(48,8)),
                                 MuonStationLayout(MuonLayout(48,8)),
                                 MuonStationLayout(MuonLayout(12,8)),
                                 MuonStationLayout(MuonLayout(12,8)));
  

}
StatusCode L0MuonOnlineMonitor::makePads(std::vector<LHCb::MuonTileID>& strips,std::vector<LHCb::MuonTileID>& pads)
{

  pads.clear();

  std::vector<LHCb::MuonTileID>::iterator i1;
  std::vector<LHCb::MuonTileID>::iterator i2;
  for (i1=strips.begin(); i1<strips.end(); ++i1){
    if (i1->station()==0) {
      pads.push_back(*i1);// M1 contains pads already
    } else if ((i1->region()==0) & (i1->station()==3 || i1->station()==4)){
      pads.push_back(*i1);// M4 and M5 contains pads already in R1
    } else {
      for (i2=i1+1; i2<strips.end(); ++i2){
        LHCb::MuonTileID pad = (*i1).intercept(*i2);
        if (pad.isValid()){
          pads.push_back(pad);
        }
      }
    }
  }

  return StatusCode::SUCCESS;

}
