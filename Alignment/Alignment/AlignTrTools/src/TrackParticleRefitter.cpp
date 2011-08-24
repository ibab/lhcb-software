// $Id: TrackToParticleRelinker.cpp,v 1.2 2010-01-05 11:43:41 wouter Exp $


/** @class TrackParticleRefitter.cpp TrackParticleRefitter.cpp.h
 *
 *  Make a subselection of a track list
 *
 *  @author Wouter Hulsbergen
 *  @date   05/01/2010
 */

#ifdef _WIN32
#pragma warning ( disable : 4355 ) // This used in initializer list, needed for ToolHandles
#endif

#include <string>
#include <boost/foreach.hpp>
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackStateProvider.h"
#include "GaudiKernel/SharedObjectsContainer.h"
#include "GaudiKernel/AlgFactory.h"
#include "Event/Track.h"
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/KalmanFitResult.h"
#include "TrackKernel/TrackStateVertex.h"
#include "LHCbMath/LorentzVectorWithError.h"

class TrackParticleRefitter: public GaudiAlgorithm {
  
public:
  
  // Constructors and destructor
  TrackParticleRefitter(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackParticleRefitter();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

private:
  std::string m_inputLocation ;
  ToolHandle<ITrackFitter> m_trackfitter ;
  ToolHandle<ITrackStateProvider> m_stateprovider ;
};


DECLARE_ALGORITHM_FACTORY( TrackParticleRefitter );

TrackParticleRefitter::TrackParticleRefitter(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator),
  m_trackfitter("TrackMasterFitter",this),
  m_stateprovider("TrackStateProvider")
{
  // constructor
  declareProperty( "ParticleLocation",  m_inputLocation ) ;
  declareProperty( "TrackFitter", m_trackfitter ) ;
}

StatusCode TrackParticleRefitter::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if(sc.isSuccess()) 
    sc = m_trackfitter.retrieve() ;
  if(sc.isSuccess()) 
    sc = m_stateprovider.retrieve() ;
  return sc ;
}

StatusCode TrackParticleRefitter::finalize()
{
  m_trackfitter.release().ignore() ;
  m_stateprovider.release().ignore() ;
  return GaudiAlgorithm::finalize() ;
}

TrackParticleRefitter::~TrackParticleRefitter()
{
  // destructor
}

namespace
{
  void addTracks( const LHCb::Particle& p,
		  std::vector<const LHCb::Track*>& tracks,
		  std::vector<double>& masshypos)
  {
    if( p.proto() && p.proto()->track() ) {
      tracks.push_back(p.proto()->track() ) ;
      masshypos.push_back(p.momentum().M()) ;
    } else {
      BOOST_FOREACH( const LHCb::Particle* dau,
		     p.daughters() )
	addTracks( *dau, tracks, masshypos ) ;
    }
  }
}

StatusCode TrackParticleRefitter::execute()
{
  LHCb::Particle::Range inputparticles  = get<LHCb::Particle::Range>(m_inputLocation) ;
  BOOST_FOREACH( const LHCb::Particle* p, inputparticles) {
    double z = p->referencePoint().z() ;
    
    std::vector< const LHCb::Track* > tracks ;
    std::vector< double > masshypos ;
    addTracks(*p, tracks, masshypos) ;
    if( tracks.size()<2 ) {
      warning()<< "Not enough tracks! .. skipping particle" << p->daughters().size() << " " << tracks.size() << endreq ;
      
    } else {
      
      std::vector< const LHCb::State* > states ;
      BOOST_FOREACH( const LHCb::Track* tr, tracks) {
	if( !dynamic_cast<const LHCb::KalmanFitResult*>(tr->fitResult())) {
	  StatusCode sc = m_trackfitter->fit( const_cast<LHCb::Track&>(*tr) ) ;
	  if(!sc.isSuccess())
	    warning() << "problem fitting track" << endreq ;
	}
	LHCb::State* state = new LHCb::State() ;
	StatusCode sc = m_stateprovider->stateFromTrajectory(*state,*tr,z) ;
	if(!sc.isSuccess()) warning() << "problem getting state from stateprovider" << endreq ;
	states.push_back(state) ;
      }
      
      LHCb::TrackStateVertex vertex( states ) ;
      vertex.fit() ;
      // now copy everything
      LHCb::Particle* ncp = const_cast<LHCb::Particle*>(p) ;
      ncp->setMomentum(  vertex.p4(masshypos) ) ;
      ncp->setReferencePoint( vertex.position() ) ;
      Gaudi::SymMatrix7x7 cov7x7 = vertex.covMatrix7x7(masshypos) ;
      ncp->setMomCovMatrix( cov7x7.Sub<Gaudi::SymMatrix4x4>(3,3) ) ;
      ncp->setPosCovMatrix( cov7x7.Sub<Gaudi::SymMatrix3x3>(0,0) ) ;
      ncp->setPosMomCovMatrix( cov7x7.Sub<Gaudi::Matrix4x3>(3,0) ) ;
      ncp->setMeasuredMass( ncp->momentum().M() ) ;
      //ncp->setMeasuredMassErr(  vertex.massErr(masshypos) ) ;
      ncp->setMeasuredMassErr( Gaudi::Math::LorentzVectorWithError( ncp->momentum(),
								    ncp->momCovMatrix() ).sigmaMass() ) ;
      LHCb::Vertex* endvertex = ncp->endVertex() ;
      if( endvertex ) {
	endvertex->setPosition( vertex.position() ) ;
	endvertex->setChi2AndDoF( vertex.chi2(), vertex.nDoF() ) ;
	endvertex->setCovMatrix( vertex.covMatrix() ) ;
      }
      
      // don't forget to delete all  states !
      BOOST_FOREACH( const LHCb::State* s, states) delete s ;
    }
  }
  
  return StatusCode::SUCCESS;
};
