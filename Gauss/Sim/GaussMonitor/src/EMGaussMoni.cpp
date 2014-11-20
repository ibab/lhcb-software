// $Id: EMGaussMoni.cpp, Based off VeloGaussMoni v 1.12 2007/01/12 15:56:24 ranjard Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiAlg/Tuples.h"
#include "Event/MCParticle.h"
//#include "Kernel/LHCbMath.h"
//#include "Kernel/Vector4DTypes.h"

#include "GaudiKernel/RndmGenerators.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElement.h"

// velo
//#include "VeloDet/DeVelo.h"

// local
#include "EMGaussMoni.h"
//



//-----------------------------------------------------------------------------
// Implementation file for class : EMGaussMoni
//
// VeloGaussMoni: 2005-12-13 : Tomasz Szumlak & Chris Parkes
// EMGaussMoni:   2014-15-10 : Peter Griffith & Nigel Watson		
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( EMGaussMoni );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EMGaussMoni::EMGaussMoni( const std::string& name,
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
}
//=============================================================================
// Destructor
//=============================================================================
EMGaussMoni::~EMGaussMoni() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode EMGaussMoni::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

   // Gaussian

  sc = randSvc()->generator(Rndm::Gauss(0.,1.),m_gaussGen.pRef());

  debug() << "==> Initialize" << endmsg;
  //
  // m_veloDet=( getDet<DeVelo>(m_veloDetLocation ) );

  DeVelo* m_veloDet_test= getDet<DeVelo>(m_veloDetLocation ) ;
  m_veloDet = m_veloDet_test;

  setHistoTopDir("EMGaussMoni");
  //
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode EMGaussMoni::execute() {

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
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode EMGaussMoni::finalize() {

  debug() << "==> Finalize" << endmsg;  
  m_nMCH/=m_nEvent;
  m_nMCH2/=m_nEvent;
  double err_nMCH=sqrt((m_nMCH2-(m_nMCH*m_nMCH))/m_nEvent);
  m_nPUMCH/=m_nEvent;
  m_nPUMCH2/=m_nEvent;
  double err_nPUMCH=sqrt((m_nPUMCH2-(m_nPUMCH*m_nPUMCH))/m_nEvent);
  //
  info()<< "------------------------------------------------------" <<endmsg;
  info()<< "                - EMGaussMoni table -                 " <<endmsg;
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
StatusCode EMGaussMoni::getData()
{
  debug()<< " ==> EMGaussMoni::getData" <<endmsg;

  if(!exist<LHCb::MCHits>(LHCb::MCHitLocation::Velo)){
    error()<< "There is no MCHits at MC/Velo/Hits in TES!" <<endmsg;
    m_testMCHit=false;
  }else{
    m_veloMCHits=get<LHCb::MCHits>(LHCb::MCHitLocation::Velo);
  }
  //
  if(!exist<LHCb::MCHits>(LHCb::MCHitLocation::PuVeto)){
    error()<< "There is no MCHits at MC/PuVeto/Hits in TES!" <<endmsg;
    m_testPileUpMCHit=false;
  }else{
    m_veloPileUpMCHits=get<LHCb::MCHits>(LHCb::MCHitLocation::PuVeto);
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
StatusCode EMGaussMoni::veloMCHitMonitor()
{
  debug()<< " ==> EMGaussMoni::VeloMCHitMonitor " <<endmsg;
  //
  double size=m_veloMCHits->size();
  m_nMCH+=size;
  m_nMCH2+=size*size;
  //
  plot(size, 100, 
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
    if ((*It)->pathLength() > 0.290 && (*It)->pathLength() < 0.310 && (*It)->mcParticle()->particleID().abspid() == 13) {
      plot((*It)->energy()/Gaudi::Units::keV, 101,"Energy deposited in Si [eV]",0., 400., 800); 
      double dE = (*It)->energy();
      for(LHCb::MCHits::iterator It2=m_veloMCHits->begin(); It2!=m_veloMCHits->end(); It2++){
        if ((*It)->sensDetID() ==(*It2)->sensDetID()){
          const LHCb::MCParticle* part = (*It2)->mcParticle();
          if (part !=0){
            plot(part->p()/Gaudi::Units::GeV, "p", 0., 200.);
            const LHCb::MCParticle* mother = part->mother();
            const LHCb::MCVertex* vert = part->originVertex();
            if (mother && vert && vert->type() == LHCb::MCVertex::DeltaRay && mother == (*It)->mcParticle()){
	      std::cout << "pick up delta ray " << (*It2)->mcParticle()->p() << std::endl;
              plot((*It2)->energy()/Gaudi::Units::keV,10004, "delta en", 0., 500., 200);
              plot((*It2)->mcParticle()->p(),10005, "delta en", 0., 500., 200);
              dE += (*It2)->energy();
	    }
            else if (vert && mother ) {
	      //std::cout << "mother id "  << mother->particleID().abspid() << std::endl; 
	    }
            else {
	      //std::cout << "no mother and vertex  " << part->particleID().pid() << std::endl;
	    }
	  } 
	}
      }

      plot(dE/Gaudi::Units::keV, 1001,"Energy deposited in Si [eV], drayed",0., 400., 800); 

    }
    else {
      const LHCb::MCParticle* part = (*It)->mcParticle();
      if (part !=0){
        const LHCb::MCParticle* mother = part->mother();
//        const LHCb::MCVertex* vert = part->originVertex();
        if (mother !=0){
	  //  std::cout << "test2: " << part->particleID() << " " << mother->particleID() 
	  //         << " " << vert->type();
        } 
      }
    }
 
    plot2D((*It)->entry().z()/Gaudi::Units::cm,
           (*It)->entry().x()/Gaudi::Units::cm, 102,
           "Particle entry point in Si [cm] - ZX plane",
           -20., 80., -5., 5., 1000, 50);
    plot2D((*It)->entry().x()/Gaudi::Units::cm,
           (*It)->entry().y()/Gaudi::Units::cm, 103,
           "Particle entry point in Si [cm] - XY plane",
           -5., 5., -5., 5., 50, 50);
    plot2D((*It)->exit().z()/Gaudi::Units::cm,
           (*It)->exit().x()/Gaudi::Units::cm, 104,
           "Particle exit point in Si [cm] - ZX plane",
           -20., 80., -5., 5., 1000, 50);
    plot2D((*It)->exit().x()/Gaudi::Units::cm,
           (*It)->exit().y()/Gaudi::Units::cm, 105,
           "Particle exit point in Si [cm] - XY plane",
           -5., 5., -5., 5., 50, 50);
    plot((*It)->time()/Gaudi::Units::ns, 106,
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
        plot2D(x, y, 107,
               "Particle entry point in RRight [cm] - XY plane",
               -5., 5., -5., 5., 50, 50);
        plot2D(z, x, 108,
               "Particle entry point in RRight [cm] - ZX plane",
               -20., 80., -5., 5., 1000, 50);
      }else if(sensor->isPhi()){
        if(sensor->isDownstream()){
          plot2D(x, y, 109,
                 "Particle entry point in PhiDR [cm] - XY plane",
                 -5., 5., -5., 5., 50, 50);
        }else{
          plot2D(x, y, 110,
                 "Particle entry point in PhiUR [cm] - XY plane",
                 -5., 5., -5., 5., 50, 50);
        }
      }
    }else{
      if(sensor->isR()){
        plot2D(x, y, 111,
               "Particle entry point in RLeft [cm] - XY plane",
               -5., 5., -5., 5., 50, 50);
        plot2D(z, x, 112,
               "Particle entry point in RLeft [cm] - ZX plane",
               -20, 80, -5., 5., 1000, 50);
      }else if(sensor->isPhi()){
        if(sensor->isDownstream()){
          plot2D(x, y, 113,
                 "Particle entry point in PhiDR [cm] - XY plane",
                 -5., 5., -5., 5., 50, 50);
        }else{    
          plot2D(x, y, 114,
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
      plot(fMom.e()/Gaudi::Units::GeV, 115,
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

 
  std::vector<DeVeloSensor*>::const_iterator iter = m_veloDet->rPhiSensorsBegin();
  for (; iter != m_veloDet->rPhiSensorsEnd(); ++iter){
    double depositedEn = 0.0; bool take = false; double path = 0.0;
    for(LHCb::MCHits::iterator itHit=m_veloMCHits->begin(); itHit!=m_veloMCHits->end(); ++itHit){
      if ( m_veloDet->sensor((*itHit)->sensDetID()) == *iter){
	if ((*itHit)->pathLength() > 0.290 && (*itHit)->pathLength() < 0.31 && (*itHit)->p() > 0.005*Gaudi::Units::GeV ) take = true;
        depositedEn += (*itHit)->energy();
        path += (*itHit)->pathLength();
      }
    }

     // smear 
    using namespace Gaudi::Units;
    double smear = m_gaussGen->shoot()*sqrt(0.3*1800*keV*keV/cm);
    //info()<< "*** smear "<<smear<<" take "<<take<<endmsg;
    //info()<<"*** 1 depositedEn "<<depositedEn/Gaudi::Units::keV<<endmsg;
    if (take) plot(depositedEn/Gaudi::Units::keV, 2002,"Energy deposited in Si [eV] 2",0., 400., 800);
    depositedEn += smear;
    //info()<<"*** 2 depositedEn plus smear"<<depositedEn/Gaudi::Units::keV<<endmsg;
    if (take) {
      plot(depositedEn/Gaudi::Units::keV, 1002,"Energy deposited in Si, smeared [eV]",0., 400., 800);
    }
    if (path > 0.290 ) {
      plot(depositedEn/Gaudi::Units::keV, 1003,"Energy deposited in Si [eV]",0., 400., 800); 
      //info()<<"*** 3 depositedEn path"<<depositedEn/Gaudi::Units::keV<<" path"<<path<<endmsg;
    }
    

  }

  

  return StatusCode::SUCCESS;
}
//
StatusCode EMGaussMoni::veloPileUpMCHitMonitor()
{
  debug()<< " ==> EMGaussMoni::VeloPileUpMCHitMonitor " <<endmsg;
  //
  double size=m_veloPileUpMCHits->size();
  m_nPUMCH+=size;
  m_nPUMCH2+=size*size;
  //
  plot(size, 120,
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
    plot((*It)->energy()/Gaudi::Units::eV, 121,
         "PileUp: Energy deposited in Si [eV]",
         0., 300000., 100);
    plot2D((*It)->entry().x()/Gaudi::Units::cm,
           (*It)->entry().y()/Gaudi::Units::cm, 122,
           "PileUp: Particle entry point in Si [cm] - XY plane",
           -5., 5., -5., 5., 50, 50);
    plot2D((*It)->entry().z()/Gaudi::Units::cm,
           (*It)->entry().x()/Gaudi::Units::cm, 123,
           "PileUp: Particle entry point in Si [cm] - ZX plane",
           -40., -10., -5., 5., 1000, 50);
    plot2D((*It)->exit().x()/Gaudi::Units::cm,
           (*It)->exit().y()/Gaudi::Units::cm, 124,
           "PileUp: Particle exit point in Si [cm] - XY plane",
           -5., 5., -5., 5., 50, 50);
    plot2D((*It)->exit().z()/Gaudi::Units::cm,
           (*It)->exit().x()/Gaudi::Units::cm, 125,
           "PileUp: Particle exit point in Si [cm] - ZX plane",
           -40., -10., -5., 5., 1000, 50);
    plot((*It)->time()/Gaudi::Units::ns, 126,
         "PileUp: Time Of Flight [ns]",
         0., 50., 100);
    }
   // get access to the MCParticle which made the hit, and write out 4-mom
   const LHCb::MCParticle* myMCParticle=(*It)->mcParticle();
   if(0!=myMCParticle){
     Gaudi::LorentzVector fMom=myMCParticle->momentum();
     plot(fMom.e()/Gaudi::Units::GeV, 127,
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
      //    MCVertex* oVert = myParticle->originVertex();
      // if (oVert != 0 ** oVertex->type() == MCVertex::){
      //   SmartRefVector vec = myParticle->decayVertic
      // }

    }
  }
  return StatusCode::SUCCESS;
}
//
StatusCode EMGaussMoni::basicMonitor()
{
  debug()<< " ==> EMGaussMoni::basicMonitor " <<endmsg;
  
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
    plot(size, 200, "Number of MCHits per event" , 0., 3000., 100);
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
    plot(size, 201, "Number of PileUp MCHits per event" , 0., 3000., 100);
  }
  //
  return StatusCode::SUCCESS;
}
//
StatusCode EMGaussMoni::checkTests()
{
  if(!(m_testMCHit||m_testPileUpMCHit)){
    error()<< " ==> EMGaussMoni asked to monitor nothing! " <<endmsg;
    return StatusCode::FAILURE;
  }
  //
  return StatusCode::SUCCESS;
}   
//=============================================================================
