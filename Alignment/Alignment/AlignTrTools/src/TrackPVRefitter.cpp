#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "TrackInterfaces/ITrackVertexer.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackSelector.h"
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>
#include <algorithm>

class TrackPVRefitter : public GaudiAlgorithm
{
public:

   /** Standard construtor */
  TrackPVRefitter( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~TrackPVRefitter();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm finalize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();
  
private:
  std::string m_trackContainerName;
  std::string m_pvContainerName;
  double m_maxLongTrackChisqPerDof ;

  ToolHandle<ITrackVertexer> m_vertexer ;
  ToolHandle<ITrackFitter> m_trackfitter ;
  ToolHandle<ITrackSelector> m_trackselector ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackPVRefitter );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackPVRefitter::TrackPVRefitter( const std::string& name,
					ISvcLocator* pSvcLocator)
  : GaudiAlgorithm( name , pSvcLocator ),
    m_vertexer("TrackVertexer",this),
    m_trackfitter("",this),
    m_trackselector("",this)
{
  declareProperty( "PVContainer", m_pvContainerName = LHCb::RecVertexLocation::Primary ) ;
  declareProperty( "TrackFitter", m_trackfitter ) ;
  declareProperty( "TrackSelector", m_trackselector ) ;
  //declareProperty( "MaxLongTrackChisqPerDof", m_maxLongTrackChisqPerDof = 5 ) ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackPVRefitter::~TrackPVRefitter() 
{
}


//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackPVRefitter::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;              // error printed already by GaudiAlgorithm
  sc = m_vertexer.retrieve() ;
  if( !m_trackfitter.empty() ) sc = m_trackfitter.retrieve() ;
  if( !m_trackselector.empty() ) sc = m_trackselector.retrieve() ;
  return sc;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackPVRefitter::finalize() 
{
  m_vertexer.release().ignore() ;
  if( !m_trackfitter.empty() ) m_trackfitter.release().ignore() ;
  if( !m_trackselector.empty() ) m_trackselector.release().ignore() ;
  return GaudiAlgorithm::finalize() ;
}

namespace {
  std::vector<const LHCb::Track*> myconvert( const SmartRefVector<LHCb::Track> & tracks )
  {
    std::vector<const LHCb::Track*> rc ;
    rc.reserve( tracks.size() ) ;
    BOOST_FOREACH(const LHCb::Track* t, tracks )
      if( t ) rc.push_back( t ) ;
    return rc ;
  }
}

StatusCode TrackPVRefitter::execute()
{ 
  
  LHCb::RecVertices* pvcontainer = get<LHCb::RecVertices>( m_pvContainerName ) ;

  // for now I'll just create the track lists from the Best container
  typedef std::vector<const LHCb::Track*> TrackVector ;

  LHCb::RecVertices newcontainer ;

  for( LHCb::RecVertices::iterator ipv = pvcontainer->begin() ;
       ipv != pvcontainer->end(); ++ipv ) {
    LHCb::RecVertex* pv = *ipv ;
    TrackVector tracks = myconvert(pv->tracks()) ;
    
    // we select the tracks before we fit them, to save time
    if( !m_trackselector.empty() ) {
      TrackVector tmptracks ;
      for( TrackVector::const_iterator itr = tracks.begin() ;
	   itr != tracks.end(); ++itr)
	if( m_trackselector->accept( **itr ) ) tmptracks.push_back(*itr) ;
      tracks.swap(tmptracks) ;
    }
    
    if( !m_trackfitter.empty() ) {
      TrackVector tmptracks ;
      for( TrackVector::const_iterator itr = tracks.begin() ;
	   itr != tracks.end(); ++itr) {
	StatusCode sc = m_trackfitter->fit( const_cast<LHCb::Track&>(**itr) ) ;
	if( sc.isSuccess() ) tmptracks.push_back(*itr) ;
      }
      tracks.swap(tmptracks) ;
    }
    
    if( tracks.size() >= 2 ) {
      LHCb::RecVertex* vertex = m_vertexer->fit( tracks ) ;
      vertex->setTechnique( pv->technique() ) ;
      newcontainer.add( vertex ) ;
      *pv = *vertex ;
      delete vertex ;
    }
  }

  //*pvcontainer = newcontainer ;
  return StatusCode::SUCCESS ;
}
