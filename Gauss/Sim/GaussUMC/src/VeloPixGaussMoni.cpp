// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiAlg/Tuples.h"
#include "GaudiKernel/Vector4DTypes.h"

// from LHCb
#include "Event/MCParticle.h"

// velo
#include "VeloPixDet/DeVeloPix.h"

// local
#include "VeloPixGaussMoni.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloPixGaussMoni
//
// 2009-06-05 : Victor Coco, based on VeloGaussMoni
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloPixGaussMoni );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloPixGaussMoni::VeloPixGaussMoni( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_velopixDetLocation ( DeVeloPixLocation::Default ),
    m_velopixMCHits ( ),
    m_print ( 0 ),
    m_printInfo (false),
    m_detailedMonitor ( true ),
    m_testMCHit ( true ),
    m_nMCH ( 0. ),
    m_nMCH2 ( 0. ),
    m_nEvent ( 0 )
{
  declareProperty("Print", m_print);
  declareProperty("PrintInfo", m_printInfo);
  declareProperty("TestMCHit", m_testMCHit);
  declareProperty("DetailedMonitor", m_detailedMonitor);
  declareProperty("VeloPixDetLocation", m_velopixDetLocation);
  declareProperty("VeloPixMCHits" ,
                  m_velopixMCHitsLocation = LHCb::MCHitLocation::VP /* "MC/VP/Hits" */ ) ;
}
//=============================================================================
// Destructor
//=============================================================================
VeloPixGaussMoni::~VeloPixGaussMoni() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloPixGaussMoni::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  //
  m_velopixDet=( getDet<DeVeloPix>(m_velopixDetLocation ) );
  setHistoTopDir("VP/");
  //
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloPixGaussMoni::execute() {

  debug() << "==> Execute" << endmsg;
  //
  m_nEvent++;
  getData();
  if(m_testMCHit) velopixMCHitMonitor();
  if(!m_detailedMonitor) basicMonitor();
  checkTests();
  //
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloPixGaussMoni::finalize() {

  debug() << "==> Finalize" << endmsg;  
  m_nMCH/=m_nEvent;
  m_nMCH2/=m_nEvent;
  double err_nMCH=sqrt((m_nMCH2-(m_nMCH*m_nMCH))/m_nEvent);

  //
  info()<< "------------------------------------------------------" <<endmsg;
  info()<< "                - VeloPixGaussMoni table -                 " <<endmsg;
  info()<< "------------------------------------------------------" <<endmsg;
  if(m_nMCH>0){
    info()<< "| Number of MCHits/Event:       " << m_nMCH << "+/-"
          << err_nMCH <<endmsg;
  }else{
    info()<< "| ==> No MCHits found! " <<endmsg;
  }
  info()<< "------------------------------------------------------" <<endmsg;
  //
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//
StatusCode VeloPixGaussMoni::getData()
{
  debug()<< " ==> VeloPixGaussMoni::getData" <<endmsg;

  if(!exist<LHCb::MCHits>( m_velopixMCHitsLocation )){
    error()<< "There is no MCHits at "<<m_velopixMCHitsLocation<<" in TES!" <<endmsg;
    m_testMCHit=false;
  }else{
    m_velopixMCHits=get<LHCb::MCHits>( m_velopixMCHitsLocation );
  }
  // printf("VeloPixGaussMoni::getData(): %d MC hits\n", m_velopixMCHits->size() );
  //  
  if(m_printInfo){
    info()<< "----------------------------------------------------" <<endmsg;
    if(m_testMCHit)
      info()<< " ==> Number of MCHits found in VeloPix Detector: "
            << m_velopixMCHits->size() <<endmsg;
    info()<< "---------------------------------------------------" <<endmsg;
  }
  //
  return StatusCode::SUCCESS;
}
//
StatusCode VeloPixGaussMoni::velopixMCHitMonitor()
{
  debug()<< " ==> VeloPixGaussMoni::VeloMCHitMonitor " <<endmsg;
  //
  double size=m_velopixMCHits->size();
  m_nMCH+=size;
  m_nMCH2+=size*size;
  //
  plot(size, "nMCHits", 
       "Number of hits in VeloPix per event",
       0., 3000., 100);
  //
  LHCb::MCHits::iterator It;

  // loop over all MCHits stored into the container
  for(It=m_velopixMCHits->begin(); It!=m_velopixMCHits->end(); It++){
    if(m_printInfo){
      info()<< " ==> Test MCHit: \n"
            << " sensor: " << ((*It)->sensDetID())
            << "\nentry: x= " << ((*It)->entry()).x()/Gaudi::Units::mm << " mm"
            << "\nentry: y= " << ((*It)->entry()).y()/Gaudi::Units::mm << " mm"
            << "\nentry: z= " << ((*It)->entry()).z()/Gaudi::Units::mm << " mm"
            << "\nexit: x= " << ((*It)->exit()).x()/Gaudi::Units::mm << " mm"
            << "\nexit: y= " << ((*It)->exit()).y()/Gaudi::Units::mm << " mm"
            << "\nexit: z= " << ((*It)->exit()).z()/Gaudi::Units::mm << " mm"
            << "energy: " << ((*It)->energy())/Gaudi::Units::eV << " eV"
            << "TOF: " << ((*It)->time())/Gaudi::Units::ns << " ns"
            <<endmsg;
    }
    
    //
    if(m_detailedMonitor){
      plot((*It)->energy()/Gaudi::Units::eV, "eDepSi",
           "Energy deposited in Si [eV]",
           0., 300000., 100);
      plot2D((*It)->entry().z()/Gaudi::Units::cm,
             (*It)->entry().x()/Gaudi::Units::cm, "entryZX",
             "Particle entry point in Si [cm] - ZX plane",
             -20., 80., -10., 10., 1000, 50);
      plot2D((*It)->entry().x()/Gaudi::Units::cm,
             (*It)->entry().y()/Gaudi::Units::cm, "entryXY",
             "Particle entry point in Si [cm] - XY plane",
             -10., 10., -10., 10., 500, 500);
      plot2D((*It)->exit().z()/Gaudi::Units::cm,
             (*It)->exit().x()/Gaudi::Units::cm, "exitZX",
             "Particle exit point in Si [cm] - ZX plane",
             -20., 80., -10., 10., 1000, 50);
      plot2D((*It)->exit().x()/Gaudi::Units::cm,
             (*It)->exit().y()/Gaudi::Units::cm, "exitXY",
             "Particle exit point in Si [cm] - XY plane",
             -10., 10., -10., 10., 500, 500);
      plot((*It)->time()/Gaudi::Units::ns, "TOF",
           "Time Of Flight [ns]", 
           0., 50., 100);
      //
      const DeVeloPixSensor* sensor=m_velopixDet->sensor((*It)->sensDetID());
      double x=(*It)->entry().x()/Gaudi::Units::cm;
      double y=(*It)->entry().y()/Gaudi::Units::cm;
      double z=(*It)->entry().z()/Gaudi::Units::cm;
      //
      if(sensor->isRight()){
        if(sensor->isX()){
          plot2D(x, y, "entryRX_XY",
                 "Particle entry point in XRight [cm] - XY plane",
                 -10., 10., -10., 10., 50, 50);
          plot2D(z, x, "entryRX_ZX",
                 "Particle entry point in XRight [cm] - ZX plane",
                 -20., 80., -10., 10., 1000, 50);
        }else if(sensor->isY()){
          plot2D(x, y, "entryRY_XY",
                 "Particle entry point in YRight [cm] - XY plane",
                 -10., 10., -10., 10., 50, 50);
          plot2D(z, x, "entryRY_ZX",
                 "Particle entry point in YRight [cm] - ZX plane",
                 -20., 80., -10., 10., 1000, 50);
        }else if(sensor->isSquare()){
          plot2D(x, y, "entryRY_XY",
                 "Particle entry point in SquareRight [cm] - XY plane",
                 -10., 10., -10., 10., 50, 50);
          plot2D(z, x, "entryRY_ZX",
                 "Particle entry point in SquareRight [cm] - ZX plane",
                 -20., 80., -10., 10., 1000, 50);
        }
      }else{
        if(sensor->isX()){
          plot2D(x, y, "entryLX_XY",
                 "Particle entry point in XLeft [cm] - XY plane",
                 -10., 10., -10., 10., 50, 50);
          plot2D(z, x, "entryLX_ZX",
                 "Particle entry point in XLeft [cm] - ZX plane",
                 -20., 80., -10., 10., 1000, 50);
        }else if(sensor->isY()){
          plot2D(x, y, "entryLY_XY",
                 "Particle entry point in YLeft [cm] - XY plane",
                 -10., 10., -10., 10., 50, 50);
          plot2D(z, x, "entryLY_ZX",
                 "Particle entry point in YLeft [cm] - ZX plane",
                 -20., 80., -10., 10., 1000, 50);
        }else if(sensor->isSquare()){
          plot2D(x, y, "entryLY_XY",
                 "Particle entry point in SquareLeft [cm] - XY plane",
                 -10., 10., -10., 10., 50, 50);
          plot2D(z, x, "entryLY_ZX",
                 "Particle entry point in SquareLeft [cm] - ZX plane",
                 -20., 80., -10., 10., 1000, 50);
        }
      }
    }
    // get access to the MCParticle which made the hit, and write out 4-mom
    const LHCb::MCParticle* myMCParticle=(*It)->mcParticle();
    if(0!=myMCParticle){
      Gaudi::LorentzVector fMom=myMCParticle->momentum();
      plot(fMom.e()/Gaudi::Units::GeV, "eMCPart",
           "Particle energy [GeV]",
           0., 50., 100);       
      //Get the production vertex position
      const LHCb::MCVertex * 	myMVVert = myMCParticle->originVertex () ;
      //      const LHCb::MCVertex * 	myPVVert = myMCParticle->primaryVertex () ;
      
      plot2D(myMVVert->position().x(),myMVVert->position().y(), "MCVertexPosXY",
             "Position of production MC Vertex of MCParticles giving hits - XY [cm]",
             -10., 10., -10., 10., 500, 500);      
      plot2D(myMVVert->position().z(),myMVVert->position().y(), "MCVertexPosZY",
             "Position of production MC Vertex of MCParticles giving hits - ZY [cm]",
             -20., 80., -10., 10., 500, 500); 
      if(m_printInfo){
        info()<< " ==> MCHit - MCParticle: "
              << "\np_x = " << fMom.px()/Gaudi::Units::GeV
              << "\np_y = " << fMom.py()/Gaudi::Units::GeV
              << "\np_z = " << fMom.pz()/Gaudi::Units::GeV
              <<endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}
//
StatusCode VeloPixGaussMoni::basicMonitor()
{
  debug()<< " ==> VeloPixGaussMoni::basicMonitor " <<endmsg;
  
  double size=0.;
  if(m_testMCHit){
    if(0==m_velopixMCHits){
      error()<< " -- No MCHit container retrieved! -- " <<endmsg;
      return StatusCode::FAILURE;
    }else{
      debug()<< " -- Retrieved MCHit container --" <<endmsg;
    }
    size=m_velopixMCHits->size();    
    m_nMCH+=size;
    m_nMCH2+=size*size;
    plot(size, "nMCHits", "Number of MCHits per event" , 0., 3000., 100);
  }
  //
  //
  return StatusCode::SUCCESS;
}
//
StatusCode VeloPixGaussMoni::checkTests()
{
  if(!(m_testMCHit)){
    error()<< " ==> VeloPixGaussMoni asked to monitor nothing! " <<endmsg;
    return StatusCode::FAILURE;
  }
  //
  return StatusCode::SUCCESS;
}   
//=============================================================================
