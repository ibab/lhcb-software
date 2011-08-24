// $Id: TrackToParticleRelinker.cpp,v 1.2 2010-01-05 11:43:41 wouter Exp $


/** @class TrackToParticleRelinker TrackToParticleRelinker.h
 *
 *  Make a subselection of a track list
 *
 *  @author Wouter Hulsbergen
 *  @date   05/01/2010
 */

#ifdef _WIN32
#pragma warning ( disable : 4355 ) // This used in initializer list, needed for ToolHandles
#endif

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackSelector.h"
#include "GaudiKernel/SharedObjectsContainer.h"
#include <string>
#include "GaudiKernel/AlgFactory.h"
#include "Event/Track.h"
#include "Event/Particle.h"
#include <boost/foreach.hpp>

class TrackToParticleRelinker: public GaudiAlgorithm {
  
public:
  
  // Constructors and destructor
  TrackToParticleRelinker(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackToParticleRelinker();

  //virtual StatusCode initialize();
  //virtual StatusCode finalize();

  virtual StatusCode execute();

private:
  std::string m_inputLocation ;
  std::string m_outputLocation ;
  std::string m_trackLocation ;
};


DECLARE_ALGORITHM_FACTORY( TrackToParticleRelinker );

TrackToParticleRelinker::TrackToParticleRelinker(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  declareProperty( "InputLocation",  m_inputLocation ) ;
  declareProperty( "OutputLocation",  m_outputLocation ) ;
  declareProperty( "TrackLocation", m_trackLocation ) ;
}

TrackToParticleRelinker::~TrackToParticleRelinker()
{
  // destructor
}

namespace {
  
  struct TrackClonePredicate
  {
    const LHCb::Track* lhs ;
    TrackClonePredicate( const LHCb::Track* tr ) : lhs(tr) {}
    bool operator()(const LHCb::Track* rhs) const {
      // either it is the same tracks, or all LHCbIDs of rhs appear in lhs or vice versa
      return rhs == lhs ||
	lhs->nCommonLhcbIDs(*rhs) == std::min(lhs->lhcbIDs().size(),rhs->lhcbIDs().size()) ;
    }
  } ;
  
  // very ugly: replaces pointers to track in Particle with those in track container
  // returns true only if all tracks are found
  template<class TrackContainer>
  bool replaceTracks( const LHCb::Particle& p,
		      const TrackContainer& tracks) {
    bool success = true ;
    if( p.proto() && p.proto()->track() ) {
      // locate the track in the input container
      typename TrackContainer::iterator itrack = 
	std::find_if( tracks.begin(), tracks.end(), TrackClonePredicate(p.proto()->track()) ) ;
      if( (success = (itrack != tracks.end()) ) ) {
	LHCb::ProtoParticle* proto = const_cast<LHCb::ProtoParticle*>(p.proto()) ;
	proto->setTrack( *itrack ) ;
      }
    } else {
      BOOST_FOREACH( const LHCb::Particle* dau,p.daughters() )
	success = replaceTracks(*dau,tracks) && success ;
    }
    return success ;
  }
}

StatusCode TrackToParticleRelinker::execute()
{
  LHCb::Particle::Range inputparticles  = get<LHCb::Particle::Range>(m_inputLocation) ;
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_trackLocation) ;
  LHCb::Particle::Selection* outputparticles = new LHCb::Particle::Selection() ;
  put( outputparticles, m_outputLocation) ;
  
  BOOST_FOREACH( const LHCb::Particle* p, inputparticles) {
    if( replaceTracks( *p, tracks ) )
      outputparticles->insert( p ) ;
  }
  
  return StatusCode::SUCCESS;
};
