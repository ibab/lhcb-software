// $Id: MuonHitChecker.cpp,v 1.12 2009-06-15 16:57:49 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "MuonDet/DeMuonDetector.h"
#include "MuonHitChecker.h"
#include "Event/MCHeader.h"

//-----------------------------------------------------------------------------
// This File contains the monitor for muon Hit system
//
// C++ code for 'LHCb Muon package(s)'
//
// 20-04-2005 : A. Sarti
//-----------------------------------------------------------------------------

// Declaration of the Algortihm Factory
DECLARE_ALGORITHM_FACTORY( MuonHitChecker );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonHitChecker::MuonHitChecker( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_detailedMonitor   ( false ) 
{
  declareProperty( "DetailedMonitor"         ,m_detailedMonitor );
  declareProperty( "MCHeader" , m_mcHeader = LHCb::MCHeaderLocation::Default ) ;
  declareProperty( "MuonHits" , m_muonHits = LHCb::MCHitLocation::Muon ) ;
  m_hit_outside_gaps=0;
  
}

//=============================================================================
// Destructor
//=============================================================================
MuonHitChecker::~MuonHitChecker() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonHitChecker::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
  
  debug() << "==> Initialise" << endmsg;

  for(int i=0; i<5; i++) {
    nhit_ri[i] = cnt_ri[i] = 0;
    for(int j=0; j<4; j++) {
      nhit[i][j] = cnt[i][j] = 0;
    }
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonHitChecker::execute() {

  const LHCb::MCHeader* evt = get<LHCb::MCHeader>( m_mcHeader );

  long  m_evt = evt->evtNumber();

  debug() << "==> Execute MuonHitChecker" << endmsg;
  // Header

  int tnhit[5][4];    int tnhit_ri[5];  int tmhit_ri[5];  
  for(int i=0; i<5; i++) {
    tnhit_ri[i] = 0; tmhit_ri[i] = 0;
    for(int j=0; j<4; j++) {
      tnhit[i][j] = 0;
    }
  }
  

  std::vector<float> m_sta,m_reg,m_con,m_x,m_y,m_z,m_time;
  std::vector<float> m_id,m_px,m_py,m_pz,m_E,m_xv,m_yv,m_zv,m_tv, m_mom;
  std::vector<float> m_ple,m_hen,m_dix,m_dxz,m_dyz;

  DeMuonDetector * muonD = getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon"); 

  // get the MCHits
  SmartDataPtr<LHCb::MCHits> hits(eventSvc() , m_muonHits );
  
  LHCb::MCHits::const_iterator iter;
  int MyDetID;
  // Loop over Muon Hits of given type
  if(hits!=0){
    for (iter = hits->begin(); iter < hits->end();iter++){
      
      MyDetID = (*iter)->sensDetID();
      if(MyDetID<0) m_hit_outside_gaps++;
      if(MyDetID<0) continue;
      
      //Needs to extract info from sens ID      
      int station = muonD->stationID(MyDetID);  
      
      int region  = muonD->regionID(MyDetID);   

      //int chamber = muonD->chamberID(MyDetID);        

      debug()<<" Station, Region :: " <<station<<" "<<region<<endmsg;
  
      float xpos=((*iter)->entry().x()+(*iter)->exit().x())/2.0;
      float ypos=((*iter)->entry().y()+(*iter)->exit().y())/2.0;
      float zpos=((*iter)->entry().z()+(*iter)->exit().z())/2.0;
      float time=(*iter)->time();
      
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
      m_time.push_back(time);
      
      //Fill some histos	  
      int hh =station*4+region;
      if (fullDetail() == true) {
	plot( r, hh+2000, "Radial Multiplicity", 0., 6000.,200 );
	plot(tof,hh+1000, "Time multiplicity",   0.,  100.,200 ); 
      }
      
      //MC truth
      const LHCb::MCParticle* particle=(*iter)->mcParticle();
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
	m_tv.push_back(particle->originVertex()->time());
	
	const LHCb::MCParticle * moth=particle->mother();
	if(moth && (abs(moth->particleID().pid())<100000)){
	  m_mom.push_back(moth->particleID().pid()); 
	} else {
	  m_mom.push_back(0);
	}
      } else {
	m_id.push_back(0);	    m_px.push_back(0);	    m_py.push_back(0);
	m_pz.push_back(0);	    m_E.push_back(0);	    m_xv.push_back(0);
	m_yv.push_back(0);	    m_zv.push_back(0);	    m_tv.push_back(0);
	m_mom.push_back(0);
      }          
      
      if(abs(particle->particleID().pid()) == 13) tmhit_ri[station]++;
      tnhit[station][region]++;
      tnhit_ri[station]++;
    }
  }

  for(int s=0; s<5; s++) {
    //Looking at mean number of hits (intregrated over regions)
    cnt_ri[s]++;
    nhit_ri[s]+= tnhit_ri[s];
    for(int r=0; r<4; r++) {
      //Looking at mean number of hits
      cnt[s][r]++;
      nhit[s][r]+= tnhit[s][r];
    }  
  }  

  // book vectors of histos
  if(m_detailedMonitor && (fullDetail() == true) ) {

    Tuple nt1 = nTuple(41,"MC HITS",CLID_ColumnWiseTuple);

    //    nt1->column("Run", m_run,0,1000000);
    int pippo=m_evt;
    nt1->column("Event",pippo,0,10000);

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
  }
   

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
  info()<<" M1        M2        M3        M4        M5 "<<endmsg;
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
  info() << "-----------------------------------------------------------------"
	 << endmsg;
  info() << "-------    Integrated over regions    ---------------------------"
	 << endmsg;
  info() << "-----------------------------------------------------------------"
	 << endmsg;
  for(int s=0; s<5; s++) {
    if(cnt_ri[s])  {
      info()<<format("%5.3lf  ",(double)nhit_ri[s]/cnt_ri[s]);
      //	info()<<(double)nhit[s][r]/cnt[s][r]<<"     ";
    } else {
      info()<<"0.000  ";
    }
  }
  info()<<" allR"<<endmsg;
  info()<<" number of hit generate doutside gaps volume "<<  m_hit_outside_gaps<<endreq;
  
  return StatusCode::SUCCESS;
}



