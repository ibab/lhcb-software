// $Id: ParticleToTrackContainer.cpp,v 1.2 2010-01-05 11:43:41 wouter Exp $


/** @class ParticleToTrackContainer ParticleToTrackContainer.h
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

class ParticleToTrackContainer: public GaudiAlgorithm {
  
public:
  
  // Constructors and destructor
  ParticleToTrackContainer(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~ParticleToTrackContainer();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode execute();

private:
  std::string m_inputLocation;
  std::string m_outputLocation;
  double m_minMass ;
  double m_maxMass ;
  ToolHandle<ITrackSelector> m_selector;
};


DECLARE_ALGORITHM_FACTORY( ParticleToTrackContainer );

ParticleToTrackContainer::ParticleToTrackContainer(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator),
  m_selector("",this)
{
  // constructor
  declareProperty( "ParticleLocation",  m_inputLocation ) ;
  declareProperty( "TrackLocation", m_outputLocation ) ;
  declareProperty( "Selector", m_selector );
}

ParticleToTrackContainer::~ParticleToTrackContainer()
{
  // destructor
}

StatusCode ParticleToTrackContainer::initialize()
{
  // Initializes TsaInitialization at the begin of program execution.
  
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize");
  }
  
  // retrieve the selector if it is set
  if ( !m_selector.empty() ) {
    sc = m_selector.retrieve() ;
    if(sc.isFailure())
      error() << "Failed to retrieve selector." << endmsg ;
  }
  
  return sc ;
}

StatusCode ParticleToTrackContainer::finalize()
{
  if ( !m_selector.empty() ) m_selector.release().ignore() ;
  return GaudiAlgorithm::finalize() ;
}

namespace {
  void extractTracks( const LHCb::Particle& p,
		      std::vector<const LHCb::Track*>& tracks)
  {
    if( p.proto() && p.proto()->track() ) 
      tracks.push_back(p.proto()->track() ) ;
    BOOST_FOREACH( const LHCb::Particle* dau,
		   p.daughters() )
      extractTracks( *dau, tracks ) ;
  }
}

StatusCode ParticleToTrackContainer::execute()
{
  LHCb::Particle::Range particles  = get<LHCb::Particle::Range>(m_inputLocation) ;
  LHCb::Track::Selection* tracks = new LHCb::Track::Selection() ;
  put( tracks, m_outputLocation) ;

  // get all the tracks
  std::vector<const LHCb::Track*> alltracks ;
  BOOST_FOREACH( const LHCb::Particle* p, particles) 
    extractTracks( *p, alltracks) ;
  
  // make sure all tracks are unique
  std::sort( alltracks.begin(), alltracks.end() ) ;
  std::vector<const LHCb::Track*>::iterator it = std::unique( alltracks.begin(), alltracks.end() ) ;
  alltracks.erase( it, alltracks.end()) ;
  
  BOOST_FOREACH( const LHCb::Track* tr, alltracks) {
    bool accept = true ;
    if ( !m_selector.empty() )
      accept = m_selector->accept( *tr ) ;
    if( accept) tracks->insert( tr ) ;
  }
  
  debug() << "candidates, tracks, selected tracks: " << particles.size() << " " << alltracks.size() << " " << tracks->size() << endreq ;
  
  return StatusCode::SUCCESS;
};


