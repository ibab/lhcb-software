//
// This File contains the monitor for muon Digi system
//
// C++ code for 'LHCb Muon package(s)'
//
//   Author: A. Sarti
//   Created: 20-04-2005
#include <cmath>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/Tuples.h"
#include "GaudiAlg/GaudiHistoAlg.h"

// local
#include "MuonDet/MuonBasicGeometry.h"
#include "Event/MCMuonHit.h"
#include "MuonDigitChecker.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
// from Event 
#include "Event/EventHeader.h"

static const  AlgFactory<MuonDigitChecker>  s_Factory ;
const        IAlgFactory& MuonDigitCheckerFactory = s_Factory ; 

std::string MuonDigitChecker::spill[5] = 
{"","/Prev","/PrevPrev","/Next","/NextNext"};
std::string MuonDigitChecker::numreg[4] = 
{"1","2","3","4"};
std::string MuonDigitChecker::numsta[5]= 
{"1","2","3","4","5"};
std::string MuonDigitChecker::TESPathOfHitsContainer[4]=
{"Hits","ChamberNoiseHits","FlatSpilloverHits","BackgroundHits"};
std::string MuonDigitChecker::HitOr[6]=
  {"Geant","FlastSpillOver","Low Energy Background","Chamber Noise",
   "Crosstalk","Electronic Noise"};      

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonDigitChecker::MuonDigitChecker( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_hitMonitor   ( false ), 
    m_detailedMonitor   ( false )
{
  setProperty( "HistoTopDir",  "MUON/" );
  setProperty( "NTupleTopDir", "MUON/" );
  
  declareProperty( "hitMonitor"              ,m_hitMonitor );
  declareProperty( "DetailedMonitor"         ,m_detailedMonitor );
}

//=============================================================================
// Destructor
//=============================================================================
MuonDigitChecker::~MuonDigitChecker() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonDigitChecker::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
  
  StatusCode scs=toolSvc()->retrieveTool("MuonTileIDXYZ",m_MuonTileXYZ); 
  // sc=toolSvc()->retrieveTool("MuonGetInfoTool",m_pGetInfo);
  if(scs.isFailure())return StatusCode::FAILURE;

  debug() << "==> Initialise" << endmsg;

  MuonBasicGeometry basegeometry(detSvc(),msgSvc());
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();  int i=0;

  while(i<m_stationNumber){
    numsta[i]=basegeometry.getStationName(i);
    debug()<<" station "<<i<<" "<<numsta[i]<<endmsg;
    i++;
  }
  m_partition=basegeometry.getPartitions();

  for(int ix=0; ix<6; ix++) {
    if(ix == 0 && m_hitMonitor) {
      for(int i=0; i<5; i++) {
	for(int j=0; j<4; j++) {
	  nhit[i][j] = cnt[i][j] = 0;
	}
      }
    }
    for(int i=0; i<5; i++) {
      for(int j=0; j<4; j++) {
	nDhit[i][j][ix] = Dcnt[i][j][ix] = 0;
      }
    }
  }
  return StatusCode::SUCCESS;
};

StatusCode MuonDigitChecker::crNtuples() {

  if(m_hitMonitor) {
    Tuple nt1 = nTuple(41,"MC HITS",CLID_ColumnWiseTuple);
    
    nt1->column("Run", m_run,0,1000000);
    nt1->column("Event",m_evt,0,10000);
    
    nt1->farray("is", m_sta ,"Nhits",1000);
    nt1->farray("ir", m_reg ,"Nhits",1000);
    nt1->farray("ic", m_con ,"Nhits",1000);
    nt1->farray("x",  m_x   ,"Nhits",1000);
    nt1->farray("y",  m_y   ,"Nhits",1000);
    nt1->farray("z",  m_z   ,"Nhits",1000);
    nt1->farray("t",  m_time,"Nhits",1000);
    nt1->farray("id", m_id  ,"Nhits",1000);
    nt1->farray("px", m_px  ,"Nhits",1000);
    nt1->farray("py", m_py  ,"Nhits",1000);
    nt1->farray("pz", m_pz  ,"Nhits",1000);
    nt1->farray("E",  m_E   ,"Nhits",1000);
    nt1->farray("xv", m_xv  ,"Nhits",1000);
    nt1->farray("yv", m_yv  ,"Nhits",1000);
    nt1->farray("zv", m_zv  ,"Nhits",1000);
    nt1->farray("tv", m_tv  ,"Nhits",1000);
    nt1->farray("idm",m_mom ,"Nhits",1000);
    nt1->write();  
  }

  Tuple nt2 = nTuple(42,"DIGITS",CLID_ColumnWiseTuple);
 
  nt2->column("Run"  , m_digit_run, 0,1000000);
  nt2->column("Event", m_digit_evt, 0,10000);
  nt2->farray("is",    m_digit_s,      "Ndigits",1000);
  nt2->farray("ir",    m_digit_r,      "Ndigits",1000);
  nt2->farray("x" ,    m_digit_x,      "Ndigits",1000);
  nt2->farray("y" ,    m_digit_y,      "Ndigits",1000);
  nt2->farray("z" ,    m_digit_z,      "Ndigits",1000);
  nt2->farray("dx",    m_digit_dx,     "Ndigits",1000);
  nt2->farray("dy",    m_digit_dy,     "Ndigits",1000);
  nt2->farray("dz",    m_digit_dz,     "Ndigits",1000);
  nt2->farray("t" ,    m_digit_time,   "Ndigits",1000);
  nt2->farray("origin",m_digit_origin, "Ndigits",1000);
  nt2->farray("bx"    ,m_digit_bx,     "Ndigits",1000);
  nt2->farray("firing",m_digit_firing, "Ndigits",1000);
  nt2->farray("multip",m_digit_multi,  "Ndigits",1000);
  nt2->write();  

  return StatusCode::SUCCESS;

}
//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonDigitChecker::execute() {

  debug() << "==> Execute MuonDigitChecker" << endmsg;
 // Header
  const EventHeader* evt = get<EventHeader>(EventHeaderLocation::Default);
  int tnhit[5][4];    int tnDhit[5][4][6];  
  for(int ix=0; ix<6; ix++) {
    if(ix == 0 && m_hitMonitor) {
      for(int i=0; i<5; i++) {
	for(int j=0; j<4; j++) {
	  tnhit[i][j] = 0;
	}
      }
    }
    for(int i=0; i<5; i++) {
      for(int j=0; j<4; j++) {
	tnDhit[i][j][ix] = 0;
      }
    }
  }
  m_run = evt->runNum();
  m_evt = evt->evtNum();
  m_run = 1;
  //m_evt = 1;

  m_digit_run = evt->runNum();
  m_digit_evt = evt->evtNum();
  m_digit_run = 1;
  // m_digit_evt = 1;
  
  // loop over Muon Chambers : 4 regions x 5 stations
  if(m_hitMonitor) {
    m_sta.clear();  m_reg.clear();  m_con.clear();  m_x.clear();  m_y.clear();
    m_z.clear();  m_time.clear();   m_id.clear();  m_px.clear();  m_py.clear();
    m_pz.clear();  m_E.clear();  m_xv.clear();  m_yv.clear();  m_zv.clear();
    m_tv.clear();   m_mom.clear();  
  }
  m_digit_s.clear();  m_digit_r.clear();
  m_digit_x.clear();  m_digit_y.clear();  m_digit_z.clear();  
  m_digit_dx.clear();
  m_digit_dy.clear();  m_digit_dz.clear();  m_digit_time.clear();  
  m_digit_origin.clear();  m_digit_bx.clear();
  m_digit_multi.clear(); m_digit_firing.clear();

  // loop over Muon Hits only if required
  if(m_hitMonitor) {
    for(int iterRegion=0; iterRegion<m_partition; iterRegion++){
      int station=iterRegion/m_regionNumber;
      int region=iterRegion%m_regionNumber;    
      debug()<<" region:: " <<iterRegion<<endmsg;
    
      // loop over Muon Hits type
      // container :  0 = Geant
      //              1 = Chamber Noise
      //              2 = Flat Spill Background
      //              3 = Low Energy Background
      for(int container=0; container<4; container++){
      
	// spill[0] = evento corrente
	//       n  (n=1,4) = evento a n*25 ns precedente
      
	std::string path="/Event"+spill[0]+"/MC/Muon/"+
	  numsta[station]+"/R"+numreg[region]+"/"+
	  TESPathOfHitsContainer[container];
      
	debug() <<" path" <<path <<endmsg;
      
	SmartDataPtr<MCMuonHits> hitPointer(eventSvc(),path);
	MCMuonHits::const_iterator iter;	 

	// Loop over Muon Hits of given type
	if(hitPointer!=0){
	  for (iter=(hitPointer)->begin();iter<(hitPointer)->end();iter++){
	  
	    //          int chamber=(*iter)->chamberID();
	    float xpos=((*iter)->entry().x()+(*iter)->exit().x())/2.0;
	    float ypos=((*iter)->entry().y()+(*iter)->exit().y())/2.0;
	    float zpos=((*iter)->entry().z()+(*iter)->exit().z())/2.0;
	    float time=(*iter)->timeOfFlight();

	    //New monitored quantities. Not used for now
      //	    float plen=(*iter)->pathLength();
      //	    float hene=(*iter)->energy();
      //	    float hdis=(*iter)->displacement().x();
      //	    float hdxz=(*iter)->dxdz();
      //	    float hdyz=(*iter)->dydz();

	    double tof=time-sqrt(xpos*xpos+ypos*ypos+zpos*zpos)/300.0;
	    if(tof<0.1)tof=0.1;
	    float r=sqrt(xpos*xpos+ypos*ypos);

	    m_sta.push_back(station);
	    m_reg.push_back(region);
	    m_con.push_back(container);

	    m_x.push_back(xpos); m_y.push_back(ypos); m_z.push_back(zpos);
	    m_time.push_back(time);

	    //Fill some histos	  
	    int hh =station*4+region;

	    if (fullDetail() == true) {
	      plot( r, hh+2000, "Radial Multiplicity", 0., 6000.,200 );
	      plot(tof,hh+1000, "Time multiplicity",   0.,  100.,200 ); 
	    }
	    //MC truth
	    MCParticle* particle=(*iter)->mcParticle();
	    if(particle){            
	      if(abs(particle->particleID().pid())<100000){
		m_id.push_back(particle->particleID().pid());
	      }

	      m_px.push_back(particle->momentum().px());
	      m_py.push_back(particle->momentum().py());
	      //Pz sign tells you the particle direction
	      m_pz.push_back(particle->momentum().pz());
	      m_E.push_back(particle->momentum().e());

	      //Particle Vertex studies	    
	      m_xv.push_back(particle->originVertex()->position().x());
	      m_yv.push_back(particle->originVertex()->position().y());
	      m_zv.push_back(particle->originVertex()->position().z());
	      m_tv.push_back(particle->originVertex()->timeOfFlight());
	    
	      const MCParticle * madre=particle->mother();
	      if(madre && (abs(madre->particleID().pid())<100000)){
		m_mom.push_back(madre->particleID().pid()); 
	      } else {
		m_mom.push_back(0);
	      }
	    } else {
	      m_id.push_back(0);	    m_px.push_back(0);	    m_py.push_back(0);
	      m_pz.push_back(0);	    m_E.push_back(0);	    m_xv.push_back(0);
	      m_yv.push_back(0);	    m_zv.push_back(0);	    m_tv.push_back(0);
	      m_mom.push_back(0);
	    }          

	    tnhit[station][region]++;
	  } 
	}
      }
    }
    for(int r=0; r<4; r++) {
      for(int s=0; s<5; s++) {
	//Looking at mean number of hits
	cnt[s][r]++;
	nhit[s][r]+= tnhit[s][r];
      }  
    }    
  }    
  
  // Loop on Digits (strips)
  SmartDataPtr<MuonDigits> digit(eventSvc(),
				 MuonDigitLocation::MuonDigit);
  SmartDataPtr<MCMuonDigits> mcdigit(eventSvc(),
				     MCMuonDigitLocation::MCMuonDigit);
  
  MuonDigits::const_iterator jdigit;
  int Dsta, Dreg, Dcon;
  double Dfir;
  bool Deve, Dali;
  double x,y,z;  double dx,dy,dz;  

  for(jdigit=digit->begin();jdigit<digit->end();jdigit++){
    Dsta = (*jdigit)->key().station();
    Dreg = (*jdigit)->key().region();

    m_digit_s.push_back(Dsta);
    m_digit_r.push_back(Dreg);
    m_digit_time.push_back((*jdigit)->TimeStamp());

    // attenzione !
    // TimeStamp e' uno degli 8 intervalli (8 digits) in cui
    // e' divisa la finestra di accettanza di 20 ns dopo il beam crossing
    
    m_MuonTileXYZ->calcTilePos((*jdigit)->key(),x,dx,y,dy,z,dz);
    m_digit_x.push_back(x);
    m_digit_y.push_back(y);
    m_digit_z.push_back(z);
    
    m_digit_dx.push_back(dx);
    m_digit_dy.push_back(dy);
    m_digit_dz.push_back(dz);
    
    debug()<<" Digits" << x<<y<<z <<endmsg;

    //Match with "true" MC digit    
    ContainedObject* MCd=mcdigit->containedObject((*jdigit)->key());  
    MCMuonDigit* MCmd=dynamic_cast<MCMuonDigit *> (MCd);       

    MCMuonDigitInfo digInfo = MCmd->DigitInfo();
    
    // Hit orginin (codes in MuonEvent/v2r1/Event/MuonOriginFlag.h)
    // Geant                 = 0
    // FlastSpillOver        = 1
    // Low Energy Background = 2
    // Chamber Noise         = 3
    // Crosstalk             = 4
    // Electronic Noise      = 5
    Dcon = digInfo.natureOfHit();
    Deve = digInfo.doesFiringHitBelongToCurrentEvent();
    Dali = digInfo.isAlive();
    Dfir = MCmd->firingTime();
    m_digit_firing.push_back(Dfir);
    m_digit_origin.push_back(Dcon);
    m_digit_bx.push_back(Deve);

    // Hits mupltiplicity
    m_digit_multi.push_back( MCmd->mcMuonHits().size());
    if(Deve) tnDhit[Dsta][Dreg][Dcon]++;
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
  
  // book vectors of histos
  StatusCode sc;
  if(m_detailedMonitor && (fullDetail() == true) ) sc = crNtuples();
   

 return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonDigitChecker::finalize() {
  info() << "-----------------------------------------------------------------"
	 << endmsg;
  info() << "       Muon Monitoring Table " << endmsg;
  info() << "-----------------------------------------------------------------"
	 << endmsg;
  if(m_detailedMonitor && (fullDetail() == true) && m_hitMonitor) {
    info() << "       Hit Information     " << endmsg;
    info()<<" M1      M2      M3      M4      M5 "<<endmsg;
    for(int r=0; r<4; r++) {
      for(int s=0; s<5; s++) {
	if(cnt[s][r])  {
	  info()<<format("%5.3lf  ",(double)nhit[s][r]/cnt[s][r]);
	  //	info()<<(double)nhit[s][r]/cnt[s][r]<<"     ";
	} else {
	  info()<<"0.000  ";
	}
      }
      info()<<" R"<<r+1<<endmsg;
    }
    info() << "-----------------------------------------------------------------" << endmsg;
  }

  info() << "    Digi Information     " << endmsg;
  info() << "-----------------------------------------------------------------" << endmsg;

  for(int c=0; c<6; c++) {
    std::string myCon = HitOr[c];
    info() << "-----------------------------------------------------------------" << endmsg;
    info() << "-----   Container: "<< myCon << "------------------------" << endmsg;
    info() << "-----------------------------------------------------------------" << endmsg;
    info()<<"   M1     M2     M3     M4     M5 "<<endmsg;
    for(int r=0; r<4; r++) {
      for(int s=0; s<5; s++) {
	if(Dcnt[s][r][c])  {
	  info()<<format("%5.3lf  ",(double)nDhit[s][r][c]/Dcnt[s][r][c]);
	} else {
	  info()<<"0.000  ";
	}
      }
      info()<<" R"<<r+1<<endmsg;
    }
  }
  

  if( m_MuonTileXYZ ) toolSvc()->releaseTool( m_MuonTileXYZ );
  return StatusCode::SUCCESS;
}



