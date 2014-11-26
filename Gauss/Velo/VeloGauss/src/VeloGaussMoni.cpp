// $Id: VeloGaussMoni.cpp,v 1.15 2009-03-26 22:02:12 robbep Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiAlg/Tuples.h"
#include "GaudiKernel/Vector4DTypes.h"

// from LHcb
#include "Event/MCParticle.h"

// velo
#include "VeloDet/DeVelo.h"

// local
#include "VeloGaussMoni.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloGaussMoni
//
// 2005-12-13 : Tomasz Szumlak & Chris Parkes
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloGaussMoni )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloGaussMoni::VeloGaussMoni( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_veloDetLocation ( DeVeloLocation::Default ),
    m_veloMCHits ( ),
    m_veloPileUpMCHits ( ),
    m_print ( 0 ),
    m_printInfo (false),
    m_detailedMonitor ( true ),
    m_testMCHit ( true ),
    m_testPileUpMCHit ( true ),
    m_nMCH ( 0. ),
    m_nMCH2 ( 0. ),
    m_nPUMCH ( 0. ),
    m_nPUMCH2 ( 0. ),
    m_nEvent ( 0 )
{
  declareProperty("Print", m_print);
  declareProperty("PrintInfo", m_printInfo);
  declareProperty("TestMCHit", m_testMCHit);
  declareProperty("TestPileUpMCHit", m_testPileUpMCHit);
  declareProperty("DetailedMonitor", m_detailedMonitor);
  declareProperty("VeloDetLocation", m_veloDetLocation);
  declareProperty("VeloMCHits" , 
                  m_veloMCHitsLocation = LHCb::MCHitLocation::Velo ) ;
  declareProperty("PuVetoMCHits" , 
                  m_puVetoMCHitsLocation = LHCb::MCHitLocation::PuVeto ) ;
}
//=============================================================================
// Destructor
//=============================================================================
VeloGaussMoni::~VeloGaussMoni() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloGaussMoni::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  //
  m_veloDet=( getDet<DeVelo>(m_veloDetLocation ) );
  setHistoTopDir("Velo/");
  //
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloGaussMoni::execute() {

  debug() << "==> Execute" << endmsg;
  //
  m_nEvent++;
  getData();
  if(m_testMCHit) veloMCHitMonitor();
  if(m_testPileUpMCHit) veloPileUpMCHitMonitor();
  if(!m_detailedMonitor) basicMonitor();
  checkTests();
  //
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloGaussMoni::finalize() {

  debug() << "==> Finalize" << endmsg;  
  m_nMCH/=m_nEvent;
  m_nMCH2/=m_nEvent;
  double err_nMCH=sqrt((m_nMCH2-(m_nMCH*m_nMCH))/m_nEvent);
  m_nPUMCH/=m_nEvent;
  m_nPUMCH2/=m_nEvent;
  double err_nPUMCH=sqrt((m_nPUMCH2-(m_nPUMCH*m_nPUMCH))/m_nEvent);
  //
  info()<< "------------------------------------------------------" <<endmsg;
  info()<< "                - VeloGaussMoni table -                 " <<endmsg;
  info()<< "------------------------------------------------------" <<endmsg;
  if(m_nMCH>0){
    info()<< "| Number of MCHits/Event:       " << m_nMCH << "+/-"
          << err_nMCH <<endmsg;
    info()<< "| Number of PileUpMCHits/Event: " << m_nPUMCH << "+/-"
          << err_nPUMCH <<endmsg;
  }else{
    info()<< "| ==> No MCHits found! " <<endmsg;
  }
  info()<< "------------------------------------------------------" <<endmsg;
  //
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//
StatusCode VeloGaussMoni::getData()
{
  debug()<< " ==> VeloGaussMoni::getData" <<endmsg;

  if(!exist<LHCb::MCHits>( m_veloMCHitsLocation )){
    error()<< "There is no MCHits at MC/Velo/Hits in TES!" <<endmsg;
    m_testMCHit=false;
  }else{
    m_veloMCHits=get<LHCb::MCHits>( m_veloMCHitsLocation );
  }
  //
  if(!exist<LHCb::MCHits>( m_puVetoMCHitsLocation )){
    error()<< "There is no MCHits at MC/PuVeto/Hits in TES!" <<endmsg;
    m_testPileUpMCHit=false;
  }else{
    m_veloPileUpMCHits=get<LHCb::MCHits>( m_puVetoMCHitsLocation );
  }
  //  
  if(m_printInfo){
    info()<< "----------------------------------------------------" <<endmsg;
    if(m_testMCHit)
      info()<< " ==> Number of MCHits found in Velo Detector: "
            << m_veloMCHits->size() <<endmsg;
    if(m_testPileUpMCHit)
      info()<< " ==> Number of Pile Up MCHits found in Velo Detector: "
            << m_veloPileUpMCHits->size() <<endmsg;
    info()<< "---------------------------------------------------" <<endmsg;
  }
  //
  return StatusCode::SUCCESS;
}
//
StatusCode VeloGaussMoni::veloMCHitMonitor()
{
  debug()<< " ==> VeloGaussMoni::VeloMCHitMonitor " <<endmsg;
  //
  double size=m_veloMCHits->size();
  m_nMCH+=size;
  m_nMCH2+=size*size;
  //
  plot(size, "nMCHits", 
       "Number of hits in Velo per event",
       0., 3000., 100);
  //
  LHCb::MCHits::iterator It;
  // loop over all MCHits stored into the container
  for(It=m_veloMCHits->begin(); It!=m_veloMCHits->end(); It++){
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
           -20., 80., -5., 5., 1000, 50);
    plot2D((*It)->entry().x()/Gaudi::Units::cm,
           (*It)->entry().y()/Gaudi::Units::cm, "entryXY",
           "Particle entry point in Si [cm] - XY plane",
           -5., 5., -5., 5., 50, 50);
    plot2D((*It)->exit().z()/Gaudi::Units::cm,
           (*It)->exit().x()/Gaudi::Units::cm, "exitZX",
           "Particle exit point in Si [cm] - ZX plane",
           -20., 80., -5., 5., 1000, 50);
    plot2D((*It)->exit().x()/Gaudi::Units::cm,
           (*It)->exit().y()/Gaudi::Units::cm, "exitXY",
           "Particle exit point in Si [cm] - XY plane",
           -5., 5., -5., 5., 50, 50);
    plot((*It)->time()/Gaudi::Units::ns, "TOF",
         "Time Of Flight [ns]", 
         0., 50., 100);
    //
    const DeVeloSensor* sensor=m_veloDet->sensor((*It)->sensDetID());
    double x=(*It)->entry().x()/Gaudi::Units::cm;
    double y=(*It)->entry().y()/Gaudi::Units::cm;
    double z=(*It)->entry().z()/Gaudi::Units::cm;
    //
    if(sensor->isRight()){
      if(sensor->isR()){
        plot2D(x, y, "entryRRXY",
               "Particle entry point in RRight [cm] - XY plane",
               -5., 5., -5., 5., 50, 50);
        plot2D(z, x, "entryRRZX",
               "Particle entry point in RRight [cm] - ZX plane",
               -20., 80., -5., 5., 1000, 50);
      }else if(sensor->isPhi()){
        if(sensor->isDownstream()){
          plot2D(x, y, "entryPDRXY",
                 "Particle entry point in PhiDR [cm] - XY plane",
                 -5., 5., -5., 5., 50, 50);
        }else{
          plot2D(x, y, "entryPURXY",
                 "Particle entry point in PhiUR [cm] - XY plane",
                 -5., 5., -5., 5., 50, 50);
        }
      }
    }else{
      if(sensor->isR()){
        plot2D(x, y, "entryRLXY",
               "Particle entry point in RLeft [cm] - XY plane",
               -5., 5., -5., 5., 50, 50);
        plot2D(z, x, "entryRLZX",
               "Particle entry point in RLeft [cm] - ZX plane",
               -20, 80, -5., 5., 1000, 50);
      }else if(sensor->isPhi()){
        if(sensor->isDownstream()){
          plot2D(x, y, "entryPDLXY",
                 "Particle entry point in PhiDR [cm] - XY plane",
                 -5., 5., -5., 5., 50, 50);
        }else{    
          plot2D(x, y, "entryPULXY",
                 "Particle entry point in PhiUL [cm] - XY plane",
                 -5., 5., -5., 5., 50, 50);
       }
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
StatusCode VeloGaussMoni::veloPileUpMCHitMonitor()
{
  debug()<< " ==> VeloGaussMoni::VeloPileUpMCHitMonitor " <<endmsg;
  //
  double size=m_veloPileUpMCHits->size();
  m_nPUMCH+=size;
  m_nPUMCH2+=size*size;
  //
  plot(size, "nMCPUHits",
       "Number of PileUp hits per event",
       0., 3000., 100);
  //
  LHCb::MCHits::iterator It;
  // loop over all hits sotred into the Pile Up Hits container
  for(It=m_veloPileUpMCHits->begin(); It!=m_veloPileUpMCHits->end(); It++){
    if(m_printInfo){
      info()<< " ==> Test Pile Up MCHit: \n"
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
    plot((*It)->energy()/Gaudi::Units::eV, "eDepSiPU",
         "PileUp: Energy deposited in Si [eV]",
         0., 300000., 100);
    plot2D((*It)->entry().x()/Gaudi::Units::cm,
           (*It)->entry().y()/Gaudi::Units::cm, "entryXYPU",
           "PileUp: Particle entry point in Si [cm] - XY plane",
           -5., 5., -5., 5., 50, 50);
    plot2D((*It)->entry().z()/Gaudi::Units::cm,
           (*It)->entry().x()/Gaudi::Units::cm, "entryXYPU",
           "PileUp: Particle entry point in Si [cm] - ZX plane",
           -40., -10., -5., 5., 1000, 50);
    plot2D((*It)->exit().x()/Gaudi::Units::cm,
           (*It)->exit().y()/Gaudi::Units::cm, "exitXYPU",
           "PileUp: Particle exit point in Si [cm] - XY plane",
           -5., 5., -5., 5., 50, 50);
    plot2D((*It)->exit().z()/Gaudi::Units::cm,
           (*It)->exit().x()/Gaudi::Units::cm, "exitZXPU",
           "PileUp: Particle exit point in Si [cm] - ZX plane",
           -40., -10., -5., 5., 1000, 50);
    plot((*It)->time()/Gaudi::Units::ns, "TOFPU",
         "PileUp: Time Of Flight [ns]",
         0., 50., 100);
    }
   // get access to the MCParticle which made the hit, and write out 4-mom
   const LHCb::MCParticle* myMCParticle=(*It)->mcParticle();
   if(0!=myMCParticle){
     Gaudi::LorentzVector fMom=myMCParticle->momentum();
     plot(fMom.e()/Gaudi::Units::GeV, "eMCPartPU",
          "Particle energy [GeV]",
          0., 50., 100);        
      if(m_printInfo){
        info()<< " ==> MCHit - MCParticle: "
              << "\np_x = " << fMom.px()/Gaudi::Units::GeV
              << "\np_y = " << fMom.py()/Gaudi::Units::GeV
              << "\np_z = " << fMom.pz()/Gaudi::Units::GeV
              << "\nE = " << fMom.e()/Gaudi::Units::GeV
              <<endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}
//
StatusCode VeloGaussMoni::basicMonitor()
{
  debug()<< " ==> VeloGaussMoni::basicMonitor " <<endmsg;
  
  double size=0.;
  if(m_testMCHit){
    if(0==m_veloMCHits){
      error()<< " -- No MCHit container retrieved! -- " <<endmsg;
      return StatusCode::FAILURE;
    }else{
      debug()<< " -- Retrieved MCHit container --" <<endmsg;
    }
    size=m_veloMCHits->size();    
    m_nMCH+=size;
    m_nMCH2+=size*size;
    plot(size, "nMCHits", "Number of MCHits per event" , 0., 3000., 100);
  }
  //
  if(m_testPileUpMCHit){
    if(0==m_veloPileUpMCHits){
      error()<< " -- No PileUp MCHit container retrieved! -- " <<endmsg;
      return StatusCode::FAILURE;
    }else{
      debug()<< " -- Retrieved PileUp MCHit container --" <<endmsg;
    }
    size=m_veloPileUpMCHits->size();
    m_nPUMCH+=size;
    m_nPUMCH2+=size*size;
    plot(size, "nMCHitsPU", "Number of PileUp MCHits per event" , 0., 3000., 100);
  }
  //
  return StatusCode::SUCCESS;
}
//
StatusCode VeloGaussMoni::checkTests()
{
  if(!(m_testMCHit||m_testPileUpMCHit)){
    error()<< " ==> VeloGaussMoni asked to monitor nothing! " <<endmsg;
    return StatusCode::FAILURE;
  }
  //
  return StatusCode::SUCCESS;
}   
//=============================================================================
