//
// This File contains the monitor for muon Hit system
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
#include "MuonHitChecker.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
// from Event 
#include "Event/EventHeader.h"

static const  AlgFactory<MuonHitChecker>  s_Factory ;
const        IAlgFactory& MuonHitCheckerFactory = s_Factory ; 

std::string MuonHitChecker::spill[5] = 
{"","/Prev","/PrevPrev","/Next","/NextNext"};
std::string MuonHitChecker::numreg[4] = 
{"1","2","3","4"};
std::string MuonHitChecker::numsta[5]= 
{"1","2","3","4","5"};


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonHitChecker::MuonHitChecker( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_detailedMonitor   ( false ) 
{
  declareProperty( "DetailedMonitor"         ,m_detailedMonitor );
}

//=============================================================================
// Destructor
//=============================================================================
MuonHitChecker::~MuonHitChecker() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonHitChecker::initialize() {

  //  if( "" == histoTopDir() ) setHistoTopDir("ST/");
  
  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
  
  debug() << "==> Initialise" << endmsg;

  MuonBasicGeometry basegeometry(detSvc(),msgSvc());
  m_stationNumber= basegeometry.getStations();
  m_regionNumber = basegeometry.getRegions();  int i=0;

  for(int i=0; i<5; i++) {
    for(int j=0; j<4; j++) {
      nhit[i][j] = cnt[i][j] = 0;
    }
  }
  while(i<m_stationNumber){
    numsta[i]=basegeometry.getStationName(i);
    debug()<<" station "<<i<<" "<<numsta[i]<<endmsg;
    i++;
  }
  m_partition=basegeometry.getPartitions();

  return StatusCode::SUCCESS;
};

StatusCode MuonHitChecker::crNtuples() {
  
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
  nt1->farray("pl", m_ple ,"Nhits",1000);
  nt1->farray("he", m_hen ,"Nhits",1000);
  nt1->farray("dx", m_dix ,"Nhits",1000);
  nt1->farray("xz", m_dxz ,"Nhits",1000);
  nt1->farray("yz", m_dyz ,"Nhits",1000);
  nt1->write();  

  return StatusCode::SUCCESS;

}
//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonHitChecker::execute() {

  info() << "==> Execute MuonHitChecker" << endmsg;
 // Header
  const EventHeader* evt = get<EventHeader>(EventHeaderLocation::Default);
  int tnhit[5][4];  
  for(int i=0; i<5; i++) {
    for(int j=0; j<4; j++) {
      tnhit[i][j] = 0;
    }
  }
  m_run = evt->runNum();
  m_evt = evt->evtNum();
  m_run = 1;
  //m_evt = 1;

  // loop over Muon Chambers : 4 regions x 5 stations
  m_sta.clear();  m_reg.clear();  m_con.clear();  m_x.clear();  m_y.clear();
  m_z.clear();  m_time.clear();   m_id.clear();  m_px.clear();  m_py.clear();
  m_pz.clear();  m_E.clear();  m_xv.clear();  m_yv.clear();  m_zv.clear();
  m_tv.clear();   m_mom.clear();  m_ple.clear();  m_hen.clear();  m_dix.clear();
  m_dxz.clear();   m_dyz.clear();  
  
  for(int iterRegion=0; iterRegion<m_partition; iterRegion++){
    int station=iterRegion/m_regionNumber;
    int region=iterRegion%m_regionNumber;    
    debug()<<" region:: " <<iterRegion<<endmsg;
    
    // loop over Muon Hits type
    // spill[0] = evento corrente
    //       n  (n=1,4) = evento a n*25 ns precedente
    
    std::string path="/Event"+spill[0]+"/MC/Muon/"+
      numsta[station]+"/R"+numreg[region]+"/Hits";
    
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
	
	//New monitored quantities
	float plen=(*iter)->pathLength();
	float hene=(*iter)->energy();
	float hdis=(*iter)->displacement().x();
	float hdxz=(*iter)->dxdz();
	float hdyz=(*iter)->dydz();
	
	//Temporary monitoring (need to check if already available)
	m_ple.push_back(plen);
	m_hen.push_back(hene);
	m_dix.push_back(hdis);
	m_dxz.push_back(hdxz);
	m_dyz.push_back(hdyz);
	
	double tof=time-sqrt(xpos*xpos+ypos*ypos+zpos*zpos)/300.0;
	if(tof<0.1)tof=0.1;
	float r=sqrt(xpos*xpos+ypos*ypos);
	
	m_sta.push_back(station);
	m_reg.push_back(region);
	//Only Geant Container available
	m_con.push_back(0);
	
	m_x.push_back(xpos); m_y.push_back(ypos); m_z.push_back(zpos);
	m_time.push_back(time/1000);
	
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
  for(int r=0; r<4; r++) {
    for(int s=0; s<5; s++) {
      if(tnhit[s][r]) {
	//Looking at mean number of hits
	cnt[s][r]++;
	nhit[s][r]+= tnhit[s][r];
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
StatusCode MuonHitChecker::finalize() {
 info() << "-----------------------------------------------------------------"
	 << endmsg;
  info() << "       Muon Monitoring Table " << endmsg;
  info() << "-----------------------------------------------------------------"
	 << endmsg;
  info()<<" S1        S2        S3        S4        S5 "<<endmsg;
  for(int r=0; r<4; r++) {
    for(int s=0; s<5; s++) {
      if(cnt[s][r])  {
	info()<<format("%5.3lf  ",(double)nhit[s][r]/cnt[s][r]);
	//	info()<<(double)nhit[s][r]/cnt[s][r]<<"     ";
      } else {
	info()<<0;
      }
    }
    info()<<" R"<<r+1<<endmsg;
  }
  
  return StatusCode::SUCCESS;
}



