// ============================================================================
// Include files
// STD & STL 
#include <algorithm>
#include <functional>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
// from Event 
#include "Event/EventHeader.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"
// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Geometry/Point3D.h"
//DaVinci
#include "DaVinciTools/IPhysDesktop.h"
#include "DaVinciTools/IParticleStuffer.h"

#include "ResolvedPi0Alg.h"

static const  AlgFactory<ResolvedPi0Alg>         s_Factory ;
const        IAlgFactory&ResolvedPi0AlgFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param name algorithm name 
 *  @param pSvc service locator 
 */
// ============================================================================
ResolvedPi0Alg::ResolvedPi0Alg
( const std::string& name ,
  ISvcLocator*       pSvc )
  : DVAlgorithm ( name , pSvc ),
    m_nEvents(0),
    m_pi0Count(0) 
{
  declareProperty( "PhotonMinLikelihood" , m_photonLhMin = 0. );
  declareProperty( "SinglePhotonUse"     , m_singlePhotonUse  = false);
  declareProperty( "MassWindow"          , m_pi0MassWin = 35. * MeV);
  declareProperty( "GammaPtCut"          , m_gPtCut     = 200. * MeV);
  declareProperty( "Pi0PtCut"            , m_pi0PtCut   = -1000. * MeV);
  declareProperty( "HistogramFlag"       , m_produceHistogram = false );
};
// ============================================================================

// ============================================================================
/// destructor (protected and virtual)
// ============================================================================
ResolvedPi0Alg::~ResolvedPi0Alg() {};
// ============================================================================

// ============================================================================
/**  standard Algorithm initialization
 *   @return status code
 */
// ============================================================================
StatusCode ResolvedPi0Alg::initialize()
{
  MsgStream  logbk( msgSvc(), name() );
  logbk << MSG::INFO <<  " == > Initialize " << endreq;

  StatusCode sc=StatusCode::SUCCESS;

  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  logbk << MSG::INFO << "    Looking for Particle Property Service." << endreq;

  IParticlePropertySvc* ppSvc = 0;
  sc = service("ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    logbk << MSG::FATAL << "    Unable to locate Particle Property Service"
	  << endreq;
    return sc;
  }

  ParticleProperty* partProp;

  partProp = ppSvc->find( "pi0" );
  m_pi0ID = (*partProp).jetsetID();
  m_pi0Mass = (*partProp).mass();

  partProp = ppSvc->find( "gamma" );
  m_gammaID = (*partProp).jetsetID();

  // If histograms are required initialize them
  if( m_produceHistogram )   {
    logbk << MSG::INFO << "    histograms will be produced" << endreq;

    // Create the histograms
    m_hMassPi0 = histoSvc()->
      book("/stat/pi0/1", "Reconstructed Pi0 Mass",
	   100,m_pi0Mass-m_pi0MassWin,m_pi0Mass+m_pi0MassWin);
    if( 0 == m_hMassPi0 ) {
      logbk << MSG::ERROR << "    Cannot register histogram 1" << endreq;
      return StatusCode::FAILURE;
    }
  }

  logbk<<MSG::WARNING<<"setFilterPassed is systematically FALSE"<<endreq;

  ///
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/**  standard Algorithm finalization
 *   @return status code
 */
// ============================================================================
StatusCode ResolvedPi0Alg::finalize()
{
  MsgStream  logbk( msgSvc(), name() );
  logbk << MSG::INFO <<  " == > Finalize " << endreq;

  // Print out counters
  logbk << MSG::INFO << "    Number of events processed      = "
	<< m_nEvents << endreq;

  logbk << MSG::INFO << "    Number of selected Pi0          = "
	<< m_pi0Count << endreq;
  // End of finalization step

  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/**  standard Algorithm execution
 *   @return status code
 */
// ============================================================================
StatusCode ResolvedPi0Alg::execute()
{
  MsgStream  logbk( msgSvc(), name() );

  // Counter of events processed
  logbk << MSG::DEBUG << "    processing event number " << ++m_nEvents << endreq;

  // Retrieve informations about event
  SmartDataPtr<EventHeader> evt(eventSvc(), EventHeaderLocation::Default );

  setFilterPassed(true);

  if ( evt ) {
    logbk << MSG::DEBUG << "    retrieved EVENT: " << evt->evtNum()
	  << " RUN: " << evt->runNum() << endreq;
  }
  else {
    logbk << MSG::ERROR << "    not able to retrieve event" << endreq;
    return StatusCode::FAILURE;
  }


  StatusCode scDesktop;

  // Retrieve the particles  from PhysDesktop
  const VertexVector& verts = desktop()->vertices();
  const ParticleVector& parts = desktop()->particles();

  logbk<<MSG::DEBUG<<"Number of Vertices  :"<<verts.size()<<endreq;
  logbk<<MSG::DEBUG<<"Number of Particles :"<<parts.size()<<endreq;

  ParticleVector selectedParts;
  StatusCode scFilter = particleFilter()->filter( parts, selectedParts );

  if ( !scFilter ) {
    logbk<< MSG::ERROR << "Could not perform correct particle filtering" << endreq;
  }

  logbk<<MSG::DEBUG<<"Number of Filtered Particles :"<<selectedParts.size()<<endreq;

  int pi0countperevt=0;

  ParticleVector::iterator ip;

  ParticleVector selectedPtConf;

  for( ip = selectedParts.begin() ; ip != selectedParts.end() ; ++ip ) {
    //unsigned int ind=ip-selectedParts.begin();
    double pt=(*ip)->momentum().et();
    if (pt>m_gPtCut &&
        (*ip)->confLevel()>m_photonLhMin &&
        (*ip)->particleID()==m_gammaID
        ) {
      selectedPtConf.push_back(*ip);
      /*
      logbk << MSG::DEBUG << "Photon Candidate "<<
        ind<<" [ConfLevel="<<(*ip)->confLevel()<<"], pt= "<<
        pt << "MeV -> selected"<< endreq;
*/
    }
  }

  logbk<<MSG::DEBUG<<"Number of Particles passing Pt Cut :"
       <<selectedPtConf.size()<<endreq;

  if (0==selectedPtConf.size()) {return StatusCode::SUCCESS;}

  if (m_singlePhotonUse){

    // Single Photon use
    std::vector< std::pair< SmartRef<Particle> ,bool> > orderedSelectedPtConf;
    unsigned int isize=selectedPtConf.size();

    // sort photon candidates according to Pt reconstructed
    for ( unsigned int i=0 ; i < isize ; ++i ){
      double ptmax=-1.;
      ParticleVector::iterator part;
      for ( ip = selectedPtConf.begin() ;
            ip != selectedPtConf.end() ;
            ++ip ){
        double pt=(*ip)->momentum().et();
        if (pt>=ptmax){
          ptmax=pt;
          part=ip;
        }
      }
      orderedSelectedPtConf.push_back(
        std::pair< SmartRef<Particle> , bool > (*part,true));
      selectedPtConf.erase(part);
    }
    if ( 0 != selectedPtConf.size() ) {
      logbk<<MSG::WARNING<<"Photon candidate sorting was wrong..."<<endreq;
      return StatusCode::FAILURE;
    }

    // Make Pi0 candidates from sorted list
    std::vector< std::pair< SmartRef<Particle> ,bool> >::iterator ip1, ip2;
    for( ip1 = orderedSelectedPtConf.begin() ;
         ip1 != orderedSelectedPtConf.end()-1 ;
         ++ip1 ) {
      if ((*ip1).second) {
        for( ip2 = ip1+1 ;
             ip2 != orderedSelectedPtConf.end() ;
             ++ip2 ) {
          if ( (*ip1).second && (*ip2).second ) {
            if (goodComb((*ip1).first,(*ip2).first)) {
  //            unsigned int ind1=ip1-orderedSelectedPtConf.begin();
  //            unsigned int ind2=ip2-orderedSelectedPtConf.begin();
  //            logbk<<MSG::DEBUG<<"Pi0 <--"<<ind1<<" + "<<ind2<<endreq;
              makePi0((*ip1).first,(*ip2).first);
              pi0countperevt++;
              (*ip1).second = false;
              (*ip2).second = false;
            }
          }
        }
      }
    }
  }
  else {

    // Multiple Photon use
    ParticleVector::const_iterator ip1,ip2;

    // Make Pi0 candidates
    for( ip1 = selectedPtConf.begin() ; ip1 != selectedPtConf.end()-1 ; ++ip1 ){
      for( ip2 = ip1+1 ; ip2 != selectedPtConf.end() ; ++ip2 ) {
        if (goodComb(*ip1,*ip2)) {
          makePi0(*ip1,*ip2);
          pi0countperevt++;
        }
      }
    }
  }

  // Log number of Combinations found by previous step
  logbk << MSG::DEBUG << "  Number of Pi0 found  = "
	<< pi0countperevt<<endreq;

  logbk << MSG::DEBUG << "  Number of particles in desktop = " << parts.size()
	<< endreq;

  scDesktop = desktop()->saveDesktop(); // save all new particles
  if (scDesktop) {
    logbk << MSG::DEBUG << " PhysDeskTop Saved to TES"<<endreq;
  }
  else {
    logbk << MSG::ERROR << "not able to save desktop in TES"
	  << endreq;
    return StatusCode::FAILURE;
  }

  m_pi0Count+=pi0countperevt;

  return StatusCode::SUCCESS;
};
// ============================================================================

StatusCode ResolvedPi0Alg::makePi0(SmartRef<Particle> p1,
                                   SmartRef<Particle> p2){
  MsgStream  logbk( msgSvc(), name() );
  //
  Vertex vtx;
  HepPoint3D coord(0.,0.,0.);
  vtx.setPosition( coord );
  vtx.addToProducts(p1);
  vtx.addToProducts(p2);
  vtx.setType(Vertex::Decay);

  Particle candPi0;
  ParticleID pi0PID( m_pi0ID );
  StatusCode scStuff =
        particleStuffer()->fillParticle( vtx, candPi0, pi0PID );
  if ( !scStuff )  {
    logbk << MSG::ERROR << ">>> Particle stuffing failed"<< endreq;
    return StatusCode::FAILURE;
  }

  Particle* pInDesktop  = desktop()->createParticle(&candPi0);

  if( pInDesktop ) {
    if( m_produceHistogram ) {m_hMassPi0->fill( candPi0.mass()/MeV, 1. );}
/*
      logbk << MSG::DEBUG << "Pi0 added to PhysDesktop "
        << "Pid and momentum = "
        << pInDesktop->particleID().pid() << ", "
        << pInDesktop->momentum().px() << ", "
        << pInDesktop->momentum().py() << ", "
        << pInDesktop->momentum().pz() << endreq;
      logbk << MSG::DEBUG << "Its decay vertex position = "
        <<(pInDesktop->endVertex())->position().x()<< ", "
        <<(pInDesktop->endVertex())->position().y()<< ", "
        <<(pInDesktop->endVertex())->position().z()
        << endreq;
*/
  }
  else {
    logbk <<MSG::DEBUG<< "not able to save Pi0 in desktop"<< endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
};

// ============================================================================

bool ResolvedPi0Alg::goodComb(SmartRef<Particle> p1,
                                       SmartRef<Particle> p2){
  MsgStream  logbk( msgSvc(), name() );
  //
  bool isGood=false;

  HepLorentzVector ggComb(0.0, 0.0, 0.0, 0.0);
  ggComb = (p1)->momentum() + (p2)->momentum() ;
  double mass=ggComb.m();
  double pt=ggComb.perp();

//  logbk << MSG::DEBUG << " Combinaison  Mass = " << mass << " MeV" << endreq;

  if (fabs(mass-m_pi0Mass)<m_pi0MassWin && pt>m_pi0PtCut ) {
    isGood=true;
    logbk << MSG::DEBUG << " Pi0 candidate  mass="<<mass<<" pt="<<pt
          <<" (m=" <<m_pi0Mass << "+/-" <<m_pi0MassWin<<"MeV)"<<endreq;
  }
  return isGood;
};

// ============================================================================
// The End
// ============================================================================


