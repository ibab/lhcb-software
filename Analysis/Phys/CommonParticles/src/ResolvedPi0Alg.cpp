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
  declareProperty( "HistogramFlag", m_produceHistogram = false );
  declareProperty( "Pi0MassWindow", m_pi0MassWin = 35. * MeV);
  declareProperty( "GammaPtCut", m_gPtCut = 200. * MeV);
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
StatusCode
ResolvedPi0Alg::initialize() 
{
  MsgStream  logbk( msgSvc(), name() );
  logbk << MSG::INFO <<  " == > Initialize " << endreq;
  
  StatusCode sc=StatusCode::SUCCESS;
  
  //  sc=loadTools(); 
  
  //  if( sc.isFailure() ) {
  //    logbk << MSG::ERROR << "   Unable to load tools" << endreq;
  //    return StatusCode::FAILURE;
  //  }
  
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
      book("/stat/simple/1", "Reconstructed Pi0 Mass",
	   100,m_pi0Mass-m_pi0MassWin,m_pi0Mass+m_pi0MassWin);
    if( 0 == m_hMassPi0 ) {
      logbk << MSG::ERROR << "    Cannot register histogram 1" << endreq;
      return StatusCode::FAILURE;
    }
  }    

  /*
  logbk << MSG::DEBUG << ">>> Retriving PhysDesktop" << endreq;
  sc = toolSvc()->retrieveTool("PhysDesktop", m_pDesktop, this);
  if( sc.isFailure() ) {
    logbk << MSG::ERROR << ">>> DVAlgorithm[PhysDesktop] not found" << endreq;
    return StatusCode::FAILURE;
  }
  
  logbk << MSG::DEBUG << ">>> Retriving ParticleStuffer" << endreq;
  sc = toolSvc()->retrieveTool("ParticleStuffer", m_pStuffer);
  if( sc.isFailure() ) {
    logbk << MSG::ERROR << ">>> DVAlgorithm[ParticleStuffer] not found" 
	  << endreq;
    return StatusCode::FAILURE;
  }
  */

  ///
  return StatusCode::SUCCESS;  
};
// ============================================================================

// ============================================================================
/**  standard Algorithm finalization
 *   @return status code 
 */
// ============================================================================
StatusCode 
ResolvedPi0Alg::finalize() 
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
StatusCode 
ResolvedPi0Alg::execute() 
{
  MsgStream  logbk( msgSvc(), name() );

  setFilterPassed(false);	
  // Counter of events processed
  logbk << MSG::DEBUG << "    processing event number " << ++m_nEvents << endreq;
  
  // Retrieve informations about event
  SmartDataPtr<EventHeader> evt(eventSvc(), EventHeaderLocation::Default );
  
  if ( evt ) {   
    logbk << MSG::DEBUG << "    retrieved EVENT: " << evt->evtNum()
	  << " RUN: " << evt->runNum() << endreq;
  }
  else {
    logbk << MSG::ERROR << "    not able to retrieve event" << endreq;
    return StatusCode::FAILURE;
  }
  
  StatusCode scDesktop = desktop()->getInput();
  if (!scDesktop) {
    logbk << MSG::WARNING<< "    not able to fill PhysDesktop " << endreq;
    return StatusCode::SUCCESS;
  }
  
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

  ParticleVector::const_iterator ip,ip1,ip2;

  ParticleVector ptSelectedParts;

  for( ip = selectedParts.begin() ; ip != selectedParts.end() ; ++ip ) {
    unsigned int ind=ip-selectedParts.begin();
    double pt=(*ip)->momentum().et();
    if (pt>m_gPtCut) {
      ptSelectedParts.push_back(*ip);
      logbk << MSG::DEBUG << " Photon Candidate "<<
	ind<<" ["<<(*ip)->confLevel()<<"] : pt= "<< 
	pt << "MeV" << endreq;
    }
  }

  logbk<<MSG::DEBUG<<"Number of Particles passing Pt Cut :"<<ptSelectedParts.size()<<endreq;

  for( ip1 = ptSelectedParts.begin() ; ip1 != ptSelectedParts.end() ; ++ip1 ) {
    if( (*ip1)->particleID() == m_gammaID ) {
      for( ip2 = ip1+1 ; ip2 != ptSelectedParts.end() ; ++ip2 ) {
	if( (*ip2)->particleID() == m_gammaID ) {
	  // 
	  unsigned int ind1=ip1-ptSelectedParts.begin();
	  unsigned int ind2=ip2-ptSelectedParts.begin();
	  
	  HepLorentzVector ggComb(0.0, 0.0, 0.0, 0.0);
	  ggComb = (*ip1)->momentum() + (*ip2)->momentum() ;
	  double mass=ggComb.m();
	  logbk << MSG::DEBUG << " Combinaison "<<
	    ind1<<" ["<<(*ip1)->confLevel()<<"] + "<<
	    ind2<<" ["<<(*ip2)->confLevel()<<"]"<<
	    "  -> Pi0 hypo Mass = " << mass << " MeV" << endreq;
	  if (fabs(mass-m_pi0Mass)<m_pi0MassWin) {
	    logbk << MSG::DEBUG << " * Kept Pi0 candidate - (m=" << 
	      m_pi0Mass << "+/-" <<
	      m_pi0MassWin<<"MeV)"<<
	      endreq;
	    Vertex vtx;
      HepPoint3D coord(0.,0.,0.);
      vtx.setPosition( coord );
	    vtx.addToProducts(*ip1);
	    vtx.addToProducts(*ip2);
	    vtx.setType(Vertex::Decay);
	    
	    Particle candPi0;
	    ParticleID pi0PID( m_pi0ID );
	    StatusCode scStuff = 
	      particleStuffer()->fillParticle( vtx, candPi0, pi0PID );
	    if ( !scStuff )  {
	      logbk << MSG::ERROR << ">>> Particle stuffing failed" 
		    << endreq;
	      return StatusCode::FAILURE;
	    }      

	    Particle* pInDesktop  = desktop()->createParticle(&candPi0);

	    pi0countperevt++;

	    if( pInDesktop ) {
	      if( m_produceHistogram ) {
		m_hMassPi0->fill( mass/MeV, 1. );
	      }
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
		    }
		    else {
	      logbk <<MSG::DEBUG<< "not able to save Pi0 in desktop"
		    << endreq;  
	    }
	  }
	}
      }
    }
  }
	  
  // Log number of Combinations found by previous step
  logbk << MSG::INFO << "  Number of Pi0 found  = " 
	<< pi0countperevt<<endreq;
  
  logbk << MSG::INFO << "  Number of particles in desktop = " << parts.size() 
	<< endreq;

  scDesktop = desktop()->saveDesktop(); // save all new particles
  if (scDesktop) {
    logbk << MSG::INFO << " PhysDeskTop Saved to TES"<<endreq;
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


// ============================================================================
// The End 
// ============================================================================


