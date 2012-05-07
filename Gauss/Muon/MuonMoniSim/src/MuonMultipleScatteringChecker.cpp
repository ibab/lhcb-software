// $Id: MuonMultipleScatteringChecker.cpp,v 1.1 2009-04-29 14:23:21 svecchi Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// from Boost
#include <boost/lexical_cast.hpp>

// local
#include "MuonMultipleScatteringChecker.h"
#include "MuonDet/DeMuonDetector.h"
#include "Event/MCHeader.h"
//-----------------------------------------------------------------------------
// Implementation file for class : MuonMultipleScatteringChecker
//
// 2009-03-12 : Stefania Vecchi
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonMultipleScatteringChecker );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonMultipleScatteringChecker::MuonMultipleScatteringChecker( const std::string& name,
                                                              ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty( "fillNtuple"         ,m_fillNtuple=false);
  declareProperty( "MCHeader" , m_mcHeader = LHCb::MCHeaderLocation::Default ) ;
  declareProperty( "MuonHits" , m_muonHits = LHCb::MCHitLocation::Muon ) ;
}
//=============================================================================
// Destructor
//=============================================================================
MuonMultipleScatteringChecker::~MuonMultipleScatteringChecker() {} 
//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonMultipleScatteringChecker::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonMultipleScatteringChecker::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  const LHCb::MCHeader* evt = get<LHCb::MCHeader>(m_mcHeader);

  int  m_evt = evt->evtNumber();

  DeMuonDetector * muonD = getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  // get the MCHits
  SmartDataPtr<LHCb::MCHits> hits(eventSvc(),m_muonHits);
  
  LHCb::MCHits::const_iterator iter;
  int MyDetID;
  std::string name, title; 
  std::vector<float> m_station,m_chamber,m_region,m_x,m_y,m_z,m_time,m_dxdz,m_dydz,m_ene;
  std::vector<float> m_p,m_pz,m_id,m_tx,m_ty;
  
  // Loop over Muon Hits of given type
  if(hits!=0){
    for (iter = hits->begin(); iter < hits->end();iter++){
      
      MyDetID = (*iter)->sensDetID();
      if(MyDetID<0) continue;
      
      //Needs to extract info from sens ID      
      int station = muonD->stationID(MyDetID);
      int region = muonD->regionID(MyDetID);
      int chamber = muonD->chamberID(MyDetID);
  
      float xpos=(*iter)->midPoint().x();
      float ypos=(*iter)->midPoint().y();
      float zpos=(*iter)->midPoint().z();
      float time=(*iter)->time();
      
      float ene=(*iter)->energy();
      float dxdz=(*iter)->dxdz();
      float dydz=(*iter)->dydz();
      
      
      m_station.push_back(station);
      m_region.push_back(region);
      m_chamber.push_back(chamber);

      debug()<<" Station " <<station<<" chamber "<<chamber<<"x= "<<xpos<<", y= "<<ypos<<" ,z="<<zpos<<endmsg;
      
      m_x.push_back(xpos); m_y.push_back(ypos); m_z.push_back(zpos);
      m_time.push_back(time);
      m_ene.push_back(ene);
      m_dxdz.push_back(dxdz);
      m_dydz.push_back(dydz);
      
      //MC truth
      const LHCb::MCParticle* particle=(*iter)->mcParticle();
      if(particle){            
        if(abs(particle->particleID().pid())<100000){
          m_id.push_back(particle->particleID().pid());
        } else m_id.push_back(99999);

        m_tx.push_back(particle->momentum().px()/particle->momentum().pz());
        m_ty.push_back(particle->momentum().py()/particle->momentum().pz());
        //Pz sign tells you the particle direction
        m_pz.push_back(particle->momentum().pz());
        m_p.push_back(particle->p());
        
      } else {
        m_id.push_back(99999);
        m_tx.push_back(99999);
        m_ty.push_back(99999);
        m_pz.push_back(99999);        
        m_p.push_back(99999);
      }
    }

    std::vector<float> c_station,c_chamber,c_region,c_x,c_y,c_z,c_time,c_dxdz,c_dydz,c_ene;
    std::vector<float> c_p,c_pz,c_id,c_tx,c_ty;
    debug()<<" m-vector size "<< m_x.size()<<endmsg;    
    for(int i1 = 0; i1<int(m_station.size()); ++i1){
      debug()<<" "<<m_x[i1]<<" "<<m_y[i1]<<" "<<m_z[i1]<<
               " "<<m_station[i1]<<" "<<m_p[i1]<<" "<<m_id[i1]<<" -- "<<m_time[i1]<<endmsg;
      bool flag =true;      
      for(int i2 = 0; i2<int(c_station.size()); ++i2){
        if(m_station[i1]==c_station[i2]&&m_p[i1]==c_p[i2]) flag=false;
      }
      if(flag){        
        c_station.push_back(m_station[i1]);
        c_region.push_back(m_region[i1]);
        c_chamber.push_back(m_chamber[i1]);
        c_x.push_back(0);
        c_y.push_back(0);
        c_z.push_back(0);
        c_dxdz.push_back(0);
        c_dydz.push_back(0);
        c_time.push_back(0);
        c_ene.push_back(0);
        c_p.push_back(m_p[i1]);
        c_pz.push_back(m_pz[i1]);
        c_id.push_back(m_id[i1]);
        c_tx.push_back(m_tx[i1]);
        c_ty.push_back(m_ty[i1]);
      }
    }
    for(int i2 = 0; i2<int(c_station.size()); ++i2){
      float ngap=0;      
      for(int i1 = 0; i1<int(m_station.size()); ++i1){
        if(m_station[i1]==c_station[i2]&&m_p[i1]==c_p[i2]) {
          
          c_x[i2]+=m_x[i1];
          c_y[i2]+=m_y[i1];
          c_z[i2]+=m_z[i1];
          c_dxdz[i2]+=m_dxdz[i1];
          c_dydz[i2]+=m_dydz[i1];
          c_time[i2]+=m_time[i1];
          c_ene[i2]+=m_ene[i1];
          ++ngap;        
        }      
      }
      c_x[i2]=c_x[i2]/ngap;
      c_y[i2]=c_y[i2]/ngap;
      c_z[i2]=c_z[i2]/ngap;
      c_dxdz[i2]=c_dxdz[i2]/ngap;
      c_dydz[i2]=c_dydz[i2]/ngap;
      c_time[i2]=c_time[i2]/ngap;
    }
    
    debug()<<" c-vector size "<< c_x.size()<<endmsg;    
    for(int i1 = 0; i1<int(c_station.size()); ++i1){
      debug()<<" "<<c_x[i1]<<" "<<c_y[i1]<<" "<<c_z[i1]<<
               " "<<c_station[i1]<<" "<<c_p[i1]<<" "<<c_id[i1]<<" -- "<<c_time[i1]<<endmsg;
      
      for(int i2 = i1+1; i2<int(c_station.size()); ++i2){
        if(c_p[i1]== 99999) continue;      
        if(fabs(c_id[i1])!= 13) continue;      
        if(c_p[i1] == c_p[i2] && c_pz[i1] == c_pz[i2] ) {
          if(c_station[i2] - c_station[i1] == 1) {
            double deltax= c_x[i2]-(c_x[i1]+tan(c_dxdz[i1])*(c_z[i2]-c_z[i1]));
            double deltay= c_y[i2]-(c_y[i1]+tan(c_dydz[i1])*(c_z[i2]-c_z[i1]));;
            double deltaTx= c_dxdz[i2]-c_dxdz[i1];
            double deltaTy= c_dydz[i2]-c_dydz[i1];
            debug()<<"("<<c_station[i1]<<","<<c_station[i2]<<
              ") deltaX= "<<deltax<<" deltaY= "<<deltay<<           
              "deltaTx= "<<deltaTx<<" deltaTy= "<<deltaTy<<" p "<<c_p[i1]<<endmsg;
            
            int ista=int(c_station[i1]);
            if(c_station[i2]>ista)ista=int(c_station[i2]);
            
            name = "dxdTx_MF"+boost::lexical_cast<std::string>(ista);
            title = "delta Tx vs delta x after MF"+boost::lexical_cast<std::string>(ista);
            double xmax, xmin;
            xmax=20;            xmin=-xmax;
            double ymax, ymin;
            ymax=5;    if(ista==1)ymax=20;      
            ymin=-ymax;
            plot2D(deltaTx*1000, deltax, name,title, xmin, xmax, ymin, ymax, 201, 201);
            name = "dydTy_MF"+boost::lexical_cast<std::string>(ista);
            title = "delta Ty vs delta y after MF"+boost::lexical_cast<std::string>(ista);
            plot2D(deltaTy*1000, deltay, name,title, xmin, xmax, ymin, ymax, 201, 201);
            
            name = "dxp_MF"+boost::lexical_cast<std::string>(ista);
            title = "delta x vs p after MF"+boost::lexical_cast<std::string>(ista);
            plot2D(deltax,c_p[i1]/Gaudi::Units::GeV, name,title, ymin, ymax, 0., 100., 201, 20);
            
            name = "dyp_MF"+boost::lexical_cast<std::string>(ista);
            title = "delta y vs p after MF"+boost::lexical_cast<std::string>(ista);
            plot2D(deltay,c_p[i1]/Gaudi::Units::GeV, name,title, ymin, ymax, 0., 100., 201, 20);
            
            name = "dTxp_MF"+boost::lexical_cast<std::string>(ista);
            title = "delta Tx vs p after MF"+boost::lexical_cast<std::string>(ista);
            plot2D(deltaTx*1000,c_p[i1]/Gaudi::Units::GeV, name,title,xmin, xmax, 0., 100., 201, 20);
            name = "dTyp_MF"+boost::lexical_cast<std::string>(ista);
            title = "delta Ty vs p after MF"+boost::lexical_cast<std::string>(ista);
            plot2D(deltaTy*1000,c_p[i1]/Gaudi::Units::GeV, name,title,xmin, xmax, 0., 100., 201, 20);
          }        
        }        
      }
    }
    
  
    // book vectors of histos
    if(m_fillNtuple ) {
      Tuple nt1 = nTuple(1,"MC Muon HITS",CLID_ColumnWiseTuple);
      //nt1->column("Event",m_evt,(short int) 0,(short int) 10000);
      int pippo=m_evt;
      nt1->column("Event",pippo, 0, 10000);
      nt1->farray("station", c_station ,"Nhits",1000);
      nt1->farray("region", c_region ,"Nhits",1000);
      nt1->farray("chamber", c_chamber ,"Nhits",1000);
      nt1->farray("x", c_x ,"Nhits",1000);
      nt1->farray("y", c_y ,"Nhits",1000);
      nt1->farray("z", c_z ,"Nhits",1000);
      nt1->farray("dxdz", c_dxdz ,"Nhits",1000);
      nt1->farray("dydz", c_dydz ,"Nhits",1000);
      nt1->farray("time", c_time ,"Nhits",1000);
      nt1->farray("ene", c_ene ,"Nhits",1000);
      nt1->farray("id", c_id ,"Nhits",1000);
      nt1->farray("p", c_p ,"Nhits",1000);
      nt1->farray("pz", c_pz ,"Nhits",1000);
      nt1->farray("tx", c_tx ,"Nhits",1000);
      nt1->farray("ty", c_ty ,"Nhits",1000);
      nt1->write();
    }
    
    
  }
  
  




  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonMultipleScatteringChecker::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
