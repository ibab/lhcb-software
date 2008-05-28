 // $Id: TrackVertexer.cpp,v 1.2 2008-05-28 08:11:33 wouter Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackVertexer.h"

// from Event
#include "Event/TwoProngVertex.h"
#include "Event/TrackStateVertex.h"
#include "Event/RecVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackVertexer
//
// 2007-11-07 : Wouter HULSBERGEN
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackVertexer );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackVertexer::TrackVertexer( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_maxNumIter(10),
    m_maxDChisq(0.01),
    m_computeMomCov(true),
    m_discardFailedFits(false)
{
  declareInterface<ITrackVertexer>(this);
  declareProperty("MaxNumberOfIterations",m_maxNumIter) ;
  declareProperty("MaxDeltaChisqForConvergence",m_maxDChisq) ;
  declareProperty("ComputeMomentumCovariance",m_computeMomCov) ;
  declareProperty("DiscardFailedFits",m_discardFailedFits) ;
}
//=============================================================================
// Destructor
//=============================================================================
TrackVertexer::~TrackVertexer() {} 

LHCb::TwoProngVertex*
TrackVertexer::fit(const LHCb::State& stateA, const LHCb::State& stateB) const
{
  LHCb::TwoProngVertex* rc(0) ;
  StateContainer states(2) ;
  states.front() = &stateA ;
  states.back() = &stateB ;
  LHCb::TrackStateVertex vertex( states, m_maxDChisq, m_maxNumIter ) ;
  if( vertex.fitStatus() == LHCb::TrackStateVertex::FitSuccess || !m_discardFailedFits) 
    rc = vertex.createTwoProngVertex(m_computeMomCov) ;
  return rc ;
}

LHCb::RecVertex*
TrackVertexer::fit(const StateContainer& tracks) const
{
  LHCb::RecVertex* recvertex(0) ;
  
  if( tracks.size()>=2 ) {
    LHCb::TrackStateVertex vertex( tracks, m_maxDChisq, m_maxNumIter ) ;
    if( vertex.fitStatus() == LHCb::TrackStateVertex::FitSuccess || !m_discardFailedFits) {
      recvertex = new LHCb::RecVertex(vertex.position()) ;
      recvertex->setChi2AndDoF( vertex.chi2(), vertex.nDoF() ) ;
      recvertex->setCovMatrix( vertex.covMatrix() ) ;
    }
  }
  return recvertex ;
}

LHCb::RecVertex*
TrackVertexer::fit(const TrackContainer& tracks) const
{
  // NOTE: THIS IS NOT EXTRAPOLATING YET!!
  warning() << "TrackVertexer::fit(const TrackContainer& tracks) is not yet extrapolating!" << endreq ;

  // get the states. this needs to becomes smarter.
  StateContainer states ;
  states.reserve( tracks.size() ) ;
  for( TrackContainer::const_iterator itrack = tracks.begin() ;
       itrack != tracks.end(); ++itrack ) 
    states.push_back( &((*itrack)->firstState()) ) ; 
  
  // fit the states
  LHCb::RecVertex* recvertex = fit( states ) ;
  
  // add the tracks
  if( recvertex ) 
    for( TrackContainer::const_iterator itrack = tracks.begin() ;
	 itrack != tracks.end(); ++itrack ) 
      recvertex->addToTracks( *itrack ) ;
  
  return recvertex ;
}
