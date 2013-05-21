//
// This File contains the monitor for muon Digi system
//
// C++ code for 'LHCb Muon package(s)'
//
//   Author: A. Sarti
//   Created: 20-04-2005

// from STL
#include <string>

// From Gaudi
#include "GaudiKernel/AlgFactory.h"

//Local
#include "Event/MCHeader.h"
#include "Event/MCHit.h"
#include "Event/MuonDigit.h"
#include "Event/MCMuonDigit.h"
#include "MuonDet/DeMuonDetector.h"   
#include "MuonDigitChecker.h"
#include <sstream>

DECLARE_ALGORITHM_FACTORY( MuonDigitChecker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonDigitChecker::MuonDigitChecker( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_hitMonitor   ( false )
{
  declareProperty( "hitMonitor"              ,m_hitMonitor );
  //  setProperty( "NTupleProduce", "false" );
  setProperty( "HistoTopDir",  "MUON/" );
  setProperty( "NTupleTopDir", "MUON/" );

}

//=============================================================================
// Destructor
//=============================================================================
MuonDigitChecker::~MuonDigitChecker() {delete m_base;}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonDigitChecker::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
  
  for(int ix=0; ix<6; ix++) {
    if(ix <4 && m_hitMonitor) {
      for(int i=0; i<5; i++) {
	for(int j=0; j<4; j++) {
	  nhit[i][j][ix] = cnt[i][j][ix] = 0;
	}
      }
    }
    for(int i=0; i<5; i++) {
      for(int j=0; j<4; j++) {
        nDhit[i][j][ix] = Dcnt[i][j][ix] = 0;
      }
    }
  }

  m_base = new MuonBasicGeometry(this->detSvc(), this->msgSvc());
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonDigitChecker::execute() {

  const LHCb::MCHeader* evt = get<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default,IgnoreRootInTES);

  int tnhit[5][4][4];    int tnDhit[5][4][6];  
  for(int ix=0; ix<6; ix++) {
    if(ix <4 && m_hitMonitor) {
      for(int i=0; i<5; i++) {
	for(int j=0; j<4; j++) {
	  tnhit[i][j][ix] = 0;
	}
      }
    }
    for(int i=0; i<5; i++) {
      for(int j=0; j<4; j++) {
        tnDhit[i][j][ix] = 0;
      }
    }
  }

  std::vector<float> m_sta,m_reg,m_cha,m_con,m_x,m_y,m_z,m_time, m_id;
  std::vector<float> m_px,m_py,m_pz,m_E,m_xv,m_yv,m_zv,m_tv, m_mom;
  std::vector<float> m_ple,m_hen,m_dix,m_dxz,m_dyz;

  std::vector<float> m_digit_s,m_digit_r,m_digit_x,m_digit_y;
  std::vector<float> m_digit_z,m_digit_dx,m_digit_dy,m_digit_dz,m_digit_time;
  std::vector<float> m_digit_origin,m_digit_bx,m_digit_firing,m_digit_multi;

  DeMuonDetector * muonD = getDet<DeMuonDetector>(DeMuonLocation::Default); 


  std::string spill[5] = {"","/Prev","/PrevPrev","/Next","/NextNext"};
  std::string TESContainer[4]=
    {"Hits","ChamberNoiseHits","FlatSpilloverHits","BackgroundHits"};

  // loop over Muon Hits only if required
  if(m_hitMonitor) {

    // get the MCHits
    // container :  0 = Geant
    //              1 = Chamber Noise
    //              2 = Flat Spill Background
    //              3 = Low Energy Background
    
    for(int container=0; container<4; container++){
      // spill[0] = current event
      //       n  (n=1,4) = event preceeding the current for n*25 ns 
      
      std::string path = "/Event" + spill[0] + 
	LHCb::MCHitLocation::Muon + TESContainer[container];
      
      LHCb::MCHits* hits = get<LHCb::MCHits>(LHCb::MCHitLocation::Muon);

      LHCb::MCHits::const_iterator iter;
      int MyDetID;
      // Loop over Muon Hits of given type
      if(hits!=0){
	for (iter = hits->begin(); iter < hits->end();iter++){
	  
	  MyDetID = (*iter)->sensDetID();
	  if(MyDetID<0)continue;
	  //Needs to extract info from sens ID      
	  int station = muonD->stationID(MyDetID);  
	  int region  = muonD->regionID(MyDetID);   
	  int chamber = muonD->chamberID(MyDetID);        
	  
	  debug()<<" region:: " <<region<<endmsg;
	  
	  float xpos=(float)(((*iter)->entry().x()+(*iter)->exit().x())/2.0);
	  float ypos=(float)(((*iter)->entry().y()+(*iter)->exit().y())/2.0);
	  float zpos=(float)(((*iter)->entry().z()+(*iter)->exit().z())/2.0);
	  float time=(float)((*iter)->time());
	  
	  double tof=time-sqrt(xpos*xpos+ypos*ypos+zpos*zpos)/300.0;
	  if(tof<0.1)tof=0.1;
	  float r=sqrt(xpos*xpos+ypos*ypos);
	  
	  m_sta.push_back((float)station);
	  m_reg.push_back((float)region);
	  m_cha.push_back((float)chamber);
	  m_con.push_back((float)container);
	  
	  //Temporary monitoring (need to check if already available)
	  m_ple.push_back((float)((*iter)->pathLength()));
	  m_hen.push_back((float)((*iter)->energy()));
	  m_dix.push_back((float)((*iter)->displacement().x()));
	  m_dxz.push_back((float)((*iter)->dxdz()));
	  m_dyz.push_back((float)((*iter)->dydz()));
	  
	  m_x.push_back(xpos); m_y.push_back(ypos); m_z.push_back(zpos);
	  m_time.push_back(time);
	  
	  //Fill some histos	  
	  int hh =station*4+region;
	  
	  plot( r, hh+2000, "Radial Multiplicity", 0., 6000.,200 );
	  plot(tof,hh+1000, "Time multiplicity",   0.,  100.,200 ); 
	  
	  //MC truth
	  const LHCb::MCParticle* particle=(*iter)->mcParticle();
	  if(particle){            
	    if(abs(particle->particleID().pid())<100000){
	      m_id.push_back((float)particle->particleID().pid());
	    }
	    
	    m_px.push_back((float)particle->momentum().px());
	    m_py.push_back((float)particle->momentum().py());
	    //Pz sign tells you the particle direction
	    m_pz.push_back((float)particle->momentum().pz());
	    m_E.push_back((float)particle->momentum().e());
	    
	    //Particle Vertex studies	    
	    m_xv.push_back((float)particle->originVertex()->position().x());
	    m_yv.push_back((float)particle->originVertex()->position().y());
	    m_zv.push_back((float)particle->originVertex()->position().z());
	    m_tv.push_back((float)particle->originVertex()->time());
	    
	    const LHCb::MCParticle * moth=particle->mother();
	    if(moth && (abs(moth->particleID().pid())<100000)){
	      m_mom.push_back((float)moth->particleID().pid()); 
	    } else {
	      m_mom.push_back(0);
	    }
	  } else {
	    m_id.push_back(0);	    m_px.push_back(0);	    m_py.push_back(0);
	    m_pz.push_back(0);	    m_E.push_back(0);	    m_xv.push_back(0);
	    m_yv.push_back(0);	    m_zv.push_back(0);	    m_tv.push_back(0);
	    m_mom.push_back(0);
	  }          
	  tnhit[station][region][container]++;
	}
      }        
    }   
    for(int ic=0; ic<4; ic++) {
      for(int r=0; r<4; r++) {
	for(int s=0; s<5; s++) {
	  //Looking at mean number of hits
	  cnt[s][r][ic]++;
	  nhit[s][r][ic]+= tnhit[s][r][ic];
	}  
      }  
    }    
  }    

  // Loop on Digits (strips)
  // get the MCMuonDigits
  LHCb::MuonDigits* digit = get<LHCb::MuonDigits>(LHCb::MuonDigitLocation::MuonDigit);
  LHCb::MCMuonDigits* mcdigit = get<LHCb::MCMuonDigits>(LHCb::MCMuonDigitLocation::MCMuonDigit);
  
  LHCb::MuonDigits::const_iterator jdigit;
  int Dsta, Dreg, Dcon;
  double Dfir;
  bool Deve;
  double x,y,z;  double dx,dy,dz;  
  
  for(jdigit=digit->begin();jdigit<digit->end();jdigit++){
    Dsta = (*jdigit)->key().station();
    Dreg = (*jdigit)->key().region();
    
    m_digit_s.push_back((float)Dsta);
    m_digit_r.push_back((float)Dreg);
    m_digit_time.push_back((float)(*jdigit)->TimeStamp());
    
    // Timestamp is one of the 8 intervals (8 digits) in which
    // the 20ns acceptance window is subdivided after beam crossing
    StatusCode sc=muonD->Tile2XYZ((*jdigit)->key(),x,dx,y,dy,z,dz);
    if(sc.isFailure())debug()<<"error in tile "<<endmsg;
    m_digit_x.push_back((float)x);
    m_digit_y.push_back((float)y);
    m_digit_z.push_back((float)z);
    
    m_digit_dx.push_back((float)dx);
    m_digit_dy.push_back((float)dy);
    m_digit_dz.push_back((float)dz);
    
    //Match with "true" MC digit    
    LHCb::MCMuonDigit * MCmd = mcdigit->object((*jdigit)->key());
    if(!mcdigit) {
      error() << "Could not find the match for " << (*jdigit)->key()
	      << " in " << LHCb::MCMuonDigitLocation::MCMuonDigit << endreq;
      return StatusCode::FAILURE;
    }
    
    LHCb::MCMuonDigitInfo digInfo = MCmd->DigitInfo();
    
    // Hit orginin (codes in MuonEvent/v2r1/Event/MuonOriginFlag.h)
    // Geant                 = 0
    // FlastSpillOver        = 1
    // Low Energy Background = 2
    // Chamber Noise         = 3
    // Crosstalk             = 4
    // Electronic Noise      = 5
    Dcon = digInfo.natureOfHit();
    Deve = digInfo.doesFiringHitBelongToCurrentEvent();
    // bool Dali = digInfo.isAlive(); // not used
    Dfir = MCmd->firingTime();
    m_digit_firing.push_back((float)Dfir);
    m_digit_origin.push_back((float)Dcon);
    m_digit_bx.push_back((float)Deve);
    
    // Hits mupltiplicity
    m_digit_multi.push_back( (float)MCmd->mcHits().size());
    //if(Deve) 
    tnDhit[Dsta][Dreg][Dcon]++;
    //if(globalTimeOffset()>0)info()<<" qui "<<Dcon<<" "<<Dfir<<" "<<Deve<<" "<<Dsta<<" "<<Dreg<<endreq;
  }
  
  //Looking at mean number of hits
  for(int c=0; c<6; c++) {
    for(int r=0; r<4; r++) {
      for(int s=0; s<5; s++) {
	Dcnt[s][r][c]++;
	nDhit[s][r][c]+= tnDhit[s][r][c];
      }  
    }    
  }      
  if(m_hitMonitor) {
    Tuple nt1 = nTuple(41,"MC HITS",CLID_ColumnWiseTuple);
    
    longlong m_evt = evt->evtNumber();  
    StatusCode sc=nt1->column("Event",m_evt,0LL,10000LL);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("is", m_sta ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("ir", m_reg ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("ch", m_cha ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("ic", m_con ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("x",  m_x   ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("y",  m_y   ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("z",  m_z   ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("t",  m_time,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("id", m_id  ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("px", m_px  ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("py", m_py  ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("pz", m_pz  ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("E",  m_E   ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("xv", m_xv  ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("yv", m_yv  ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("zv", m_zv  ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("tv", m_tv  ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("idm",m_mom ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("pl", m_ple ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("he", m_hen ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("dx", m_dix ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("xz", m_dxz ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    nt1->farray("yz", m_dyz ,"Nhits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt1->write();  
    if(sc.isFailure())debug()<<" nt error "<<endmsg;

    Tuple nt2 = nTuple(42,"DIGITS",CLID_ColumnWiseTuple);
    
    longlong m_digit_evt = evt->evtNumber();
    //    StatusCode sc=nt2->column("Event", m_digit_evt, 0LL,10000LL);
    sc=nt2->column("Event", m_digit_evt, 0LL,10000LL);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->farray("is",    m_digit_s,      "Ndigits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->farray("ir",    m_digit_r,      "Ndigits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->farray("x" ,    m_digit_x,      "Ndigits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->farray("y" ,    m_digit_y,      "Ndigits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->farray("z" ,    m_digit_z,      "Ndigits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->farray("dx",    m_digit_dx,     "Ndigits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->farray("dy",    m_digit_dy,     "Ndigits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->farray("dz",    m_digit_dz,     "Ndigits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->farray("t" ,    m_digit_time,   "Ndigits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->farray("origin",m_digit_origin, "Ndigits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->farray("bx"    ,m_digit_bx,     "Ndigits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->farray("firing",m_digit_firing, "Ndigits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->farray("multip",m_digit_multi,  "Ndigits",1000);
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
    sc=nt2->write();  
    if(sc.isFailure())debug()<<" nt error "<<endmsg;
  }  
  return StatusCode::SUCCESS;
}
  
//=============================================================================
//  Finalize
//=============================================================================

StatusCode MuonDigitChecker::finalize() {

  int nStations = m_base->getStations();
  info()<<"Stazioni trovate: "<<nStations<<endmsg;;
  std::stringstream statStr;
  for(int i=0;i<nStations;i++){
    std::string statName = m_base->getStationName(i);
    statStr<<statName<<"     ";
  }
    


  info() << "-----------------------------------------------------------------"
	 << endmsg;
  info() << "       Muon Monitoring Table " << endmsg;
  info() << "-----------------------------------------------------------------"
	 << endmsg;
  if(fullDetail() == true && m_hitMonitor) {
    std::string TESn[4]=
      {"Hits","ChamberNoiseHits","FlatSpilloverHits","BackgroundHits"};

    info() << "       Hit Information     " << endmsg;

    for(int c=0; c<4; c++) {
      std::string myTes = TESn[c];
      info() << "-----------------------------------------------------------------" << endmsg;
      info() << "----- TES Container: "<< myTes << "------------------------" << endmsg;
      info() << "-----------------------------------------------------------------" << endmsg;
      //      info()<<" M1      M2      M3      M4      M5 "<<endmsg;
      info()<<statStr.str()<<endmsg;
      for(int r=0; r<4; r++) {
	for(int s=0; s<nStations; s++) {
	  if(cnt[s][r][c])  {
	    info()<<format("%5.3lf  ",(double)nhit[s][r][c]/cnt[s][r][c]);
	  } else {
	    info()<<"0.000  ";
	  }
	}
	info()<<" R"<<r+1<<endmsg;
      }
    }
  }

  info() << "-----------------------------------------------------------------" << endmsg;
  info() << "    Digi Information     " << endmsg;
  info() << "-----------------------------------------------------------------" << endmsg;

  std::string HitOr[6]=
  {"Geant","FlastSpillOver","Low Energy Background","Chamber Noise",
   "Crosstalk","Electronic Noise"};      


  for(int c=0; c<6; c++) {
    std::string myCon = HitOr[c];
    info() << "-----------------------------------------------------------------" << endmsg;
    info() << "-----   Container: "<< myCon << "------------------------" << endmsg;
    info() << "-----------------------------------------------------------------" << endmsg;
    //    info()<<"   M1     M2     M3     M4     M5 "<<endmsg;
    info()<<statStr.str()<<endmsg;
    for(int r=0; r<4; r++) {
      for(int s=0; s<nStations; s++) {
	if(Dcnt[s][r][c])  {
	  info()<<format("%5.3lf  ",(double)nDhit[s][r][c]/Dcnt[s][r][c]);
	} else {
	  info()<<"0.000  ";
	}
      }
      info()<<" R"<<r+1<<endmsg;
    }
  }
  
  // Execute the base class finalize
  return GaudiTupleAlg::finalize();
}
