/** @class VertexListRefiner VertexListRefiner.h
 *
 *  Make a subselection of a track list
 *
 *  @author Wouter Hulsbergen
 *  @date   05/01/2010
 */

#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>
#include "Event/RecVertex.h"
#include "TrackKernel/TrackPredicates.h"
#include <boost/foreach.hpp>

class VertexListRefiner: public GaudiAlgorithm {

public:

  // Constructors and destructor
  VertexListRefiner(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~VertexListRefiner();
  virtual StatusCode execute();

private:
  std::string m_inputLocation;
  std::string m_outputLocation;
  int m_minNumTracks ;
  int m_minNumBackwardTracks ;
  int m_minNumForwardTracks ;
  int m_minNumLongTracks ;
  double m_maxChi2PerDoF ;
  double m_minX ;
  double m_maxX ;
  double m_minY ;
  double m_maxY ;
  double m_minZ ;
  double m_maxZ ;
  bool m_deepCopy ;
};


DECLARE_ALGORITHM_FACTORY( VertexListRefiner )

VertexListRefiner::VertexListRefiner(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  declareProperty( "InputLocation",  m_inputLocation ) ;
  declareProperty( "OutputLocation", m_outputLocation ) ;
  declareProperty( "MinNumBackwardTracks", m_minNumBackwardTracks = 0) ;
  declareProperty( "MinNumForwardTracks", m_minNumForwardTracks = 0) ;
  declareProperty( "MinNumLongTracks", m_minNumLongTracks = 0) ;
  declareProperty( "MinNumTracks", m_minNumTracks = 0) ;
  declareProperty( "MaxChi2PerDoF", m_maxChi2PerDoF = -1 ) ;
  declareProperty( "DeepCopy", m_deepCopy = false ) ;
  declareProperty( "MinX", m_minX = 1) ;
  declareProperty( "MaxX", m_maxX = -1) ;
  declareProperty( "MinY", m_minY = 1) ;
  declareProperty( "MaxY", m_maxY = -1) ;
  declareProperty( "MinZ", m_minZ = 1) ;
  declareProperty( "MaxZ", m_maxZ = -1) ;
}

VertexListRefiner::~VertexListRefiner()
{
  // destructor
}

StatusCode VertexListRefiner::execute()
{
  LHCb::RecVertex::Range verticesin  = get<LHCb::RecVertex::Range>(m_inputLocation) ;
  std::vector< const LHCb::RecVertex* > verticesout ;
  
  // loop 
  BOOST_FOREACH( const LHCb::RecVertex* vertex , verticesin ) {

    bool accept = true ;

    // unfortunately stl doesn't work with the smartrefs in
    // vertex. furthermore, when reading a dst, track pointers can be
    // zero.

    std::vector<const LHCb::Track*> tracks ;
    tracks.reserve( vertex->tracks().size() ) ;
    BOOST_FOREACH( const LHCb::Track* track, vertex->tracks() ) 
      if(track) tracks.push_back(track) ;
    
    accept = accept && (m_maxChi2PerDoF<0 || vertex->chi2PerDoF() < m_maxChi2PerDoF) ;
    
    accept = accept && int(tracks.size()) >= m_minNumTracks ;

    accept = accept && (m_minNumLongTracks == 0 ||
      std::count_if( tracks.begin(), tracks.end(),
		     TrackPredicates::Type(LHCb::Track::Long) ) >= m_minNumLongTracks) ;
    
    if( accept && m_minX < m_maxX ) 
      accept = m_minX < vertex->position().x() &&  vertex->position().x() < m_maxX ;

    if( accept && m_minY < m_maxY ) 
      accept = m_minY < vertex->position().y() &&  vertex->position().y() < m_maxY ;

    if( accept && m_minZ < m_maxZ ) 
      accept = m_minZ < vertex->position().z() &&  vertex->position().z() < m_maxZ ;
    
    
    if( accept && (m_minNumBackwardTracks > 0 || m_minNumForwardTracks>0 ) ) {
      int numback = std::count_if( tracks.begin(), tracks.end(),
				   TrackPredicates::Flag(LHCb::Track::Backward) ) ;
      int numforward = tracks.size() - numback ;
      accept = numback>=m_minNumBackwardTracks && numforward>=m_minNumForwardTracks ;
    }
    
    if( accept ) verticesout.push_back( vertex ) ;
  }

  if( m_deepCopy ) {
    LHCb::RecVertices* copies = new LHCb::RecVertices() ;
    put( copies, m_outputLocation) ;
    BOOST_FOREACH( const LHCb::RecVertex* vertex, verticesout ) 
      copies->insert( vertex->clone() ) ;
  } else {
    LHCb::RecVertex::Selection* selection = new LHCb::RecVertex::Selection() ;
    put( selection, m_outputLocation) ;
    BOOST_FOREACH( const LHCb::RecVertex* vertex, verticesout ) 
      selection->insert( vertex->clone() ) ;
  }
  
  return StatusCode::SUCCESS;
}

