// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "DetDesc/Condition.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
// local
#include "TrackForPV.h"
#include "FastVertex.h"
#include "FastPVFinder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastPVFinder
//
// 2011-11-15 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FastPVFinder )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastPVFinder::FastPVFinder( const std::string& name,
                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputLocation",  m_inputLocation  = LHCb::TrackLocation::Velo );
  declareProperty( "OutputLocation", m_outputLocation = LHCb::RecVertexLocation::Primary );
  declareProperty( "MaxIPToBeam",    m_maxIPToBeam    = 0.300 * Gaudi::Units::mm );
  declareProperty( "MaxDeltaZ",      m_maxDeltaZ      = 2.000 * Gaudi::Units::mm );
  declareProperty( "MinTracksInPV",  m_minTracksInPV  = 7   );
  declareProperty( "MaxChi2ToAdd",   m_maxChi2ToAdd   = 60.  );
  declareProperty( "MaxChi2Fit",     m_maxChi2Fit     = 35.  );
  declareProperty( "OffsetInZ",      m_offsetInZ      = 0.000 * Gaudi::Units::mm );
}
//=============================================================================
// Destructor
//=============================================================================
FastPVFinder::~FastPVFinder() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FastPVFinder::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_xBeam = 0.0;
  m_yBeam = 0.0;

  m_beamSpotCondition = "/dd/Conditions/Online/Velo/MotionSystem";
  IUpdateManagerSvc* updMgrSvc = svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
  updMgrSvc->registerCondition(this, m_beamSpotCondition, &FastPVFinder::updateBeamSpot);
  updMgrSvc->update(this);

  m_debug = msgLevel( MSG::DEBUG );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Update the beam position when the condition change
//=========================================================================
StatusCode FastPVFinder::updateBeamSpot ( ) {
  if ( !exist<Condition>(detSvc(), m_beamSpotCondition ) ){ 
    Warning( "Unable to locate beam spot condition" ) ;
    return StatusCode::FAILURE;
  }
  Condition *myCond =  get<Condition>(detSvc(), m_beamSpotCondition );

  const double xRC = myCond -> paramAsDouble ( "ResolPosRC" ) ;
  const double xLA = myCond -> paramAsDouble ( "ResolPosLA" ) ;
  const double   Y = myCond -> paramAsDouble ( "ResolPosY"  ) ;

  m_xBeam = ( xRC + xLA ) / 2;
  m_yBeam = Y ;

  //m_xBeam += 0.050;

  info() << "*** Update beam spot, x " << m_xBeam << " y " << m_yBeam << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode FastPVFinder::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  LHCb::RecVertices* out = new LHCb::RecVertices();
  put( out, m_outputLocation );

  LHCb::Tracks* tracks = get<LHCb::Tracks>( m_inputLocation );

  //== Select tracks with a cut on their r at beam

  std::vector<TrackForPV> pvTracks;
  for ( LHCb::Tracks::iterator itT = tracks->begin(); tracks->end() != itT; ++itT ) {
    TrackForPV temp( *itT, m_xBeam, m_yBeam );
    if ( temp.rAtBeam() < m_maxIPToBeam ) {
      pvTracks.push_back( temp );
    }
  }
  //== Create a vector of pointe rto them, for faster sorting
  std::vector<TrackForPV*> myTracks;
  for ( std::vector<TrackForPV>::iterator itTr = pvTracks.begin(); pvTracks.end() != itTr; ++itTr ) {
    myTracks.push_back( &(*itTr) );
  }     
  std::sort( myTracks.begin(), myTracks.end(), TrackForPV::LowerByZ() );
  std::vector<TrackForPV*>::iterator itT;
  if ( m_debug ) {
    for ( itT = myTracks.begin(); myTracks.end() != itT; ++itT ) {
      info() << format( "Track %3d IP %7.3f at z = %7.3f", 
                        (*itT)->track()->key(), (*itT)->rAtBeam(), (*itT)->zAtBeam() ) 
             << endmsg;   
    }
  }

  //== At least enough tracks for a vertex
  if ( myTracks.size() < m_minTracksInPV ) return StatusCode::SUCCESS;

  //== Collect all tracks with compatible zAtBeam, and then by chi2.
  std::vector<FastVertex> myVertices;  
  std::vector<TrackForPV*>::iterator itT1;
  for ( itT1 = myTracks.begin(); myTracks.end() - m_minTracksInPV > itT1; ++itT1 ) {
    std::vector<TrackForPV*>::iterator itT2 = itT1 +  m_minTracksInPV - 1;
    if ( (*itT2)->zAtBeam() > (*itT1)->zAtBeam() + m_maxDeltaZ ) continue;  // consecutive zBeam close enough
    //== extend the range
    while ( itT2+1 < myTracks.end() ) {
      if ( (*itT2+1)->zAtBeam() > (*itT1)->zAtBeam() + m_maxDeltaZ ) break;
      ++itT2;
    }
    FastVertex temp( itT1, itT2 );
    temp.removeWorsts( m_maxChi2Fit );

    //== add tracks before if they have a good chi2.
    while ( itT1 >  myTracks.begin() ) {
      -- itT1;
      double chi2 = (*itT1)->chi2( temp.vertex() );
      if ( chi2 > m_maxChi2ToAdd ) break;
      temp.addTrack( *itT1 );
    }
    itT1 = itT2;

    //== Add tracks after, and define teh next starting point...
    bool isLast = true;
    while ( ++itT2 < myTracks.end() ) {
      double chi2 = (*itT2)->chi2( temp.vertex() );
      if ( chi2 < m_maxChi2ToAdd ) {
        temp.addTrack( *itT2 );
        if ( (*itT2)->zAtBeam() - temp.vertex().z() < m_maxDeltaZ ) isLast = true;
      } else {
        if ( isLast ) itT1 = itT2-1;
        isLast = false;
      }
    }

    //== Final fit, and checks
    temp.removeWorsts( m_maxChi2Fit );
    if ( temp.nTracks() < m_minTracksInPV ) continue;
    if ( temp.nBack() == 0 ) continue;
    myVertices.push_back( temp );
  }
  if ( m_debug ) info() << "Number of seed vertices " << myVertices.size() << endmsg;

  std::vector<FastVertex>::iterator itV, itV2;
  //== Summary
  if ( m_debug ) {
    for ( itV = myVertices.begin(); myVertices.end() != itV; ++itV ) {
      info() << format( "Vertex at x %7.3f y%7.3f z%7.3f  Ntr %3d nBack%3d",
                        (*itV).vertex().x(), (*itV).vertex().y(), (*itV).vertex().z(), 
                        (*itV).nTracks(), (*itV).nBack() )
             << endmsg;
      for ( std::vector<TrackForPV*>::iterator itT = (*itV).tracks().begin(); 
            (*itV).tracks().end() != itT; ++itT ) {
        info() << format( "     Track %3d IP %7.3f at z = %7.3f chi2 %7.3f", 
                          (*itT)->track()->key(), (*itT)->rAtBeam(), (*itT)->zAtBeam(), 
                          (*itT)->chi2((*itV).vertex())  ) 
               << endmsg;   
      }
    }
  }

  //== Store the vertices in the appropriate contained
  for ( itV = myVertices.begin(); myVertices.end() != itV; ++itV ) {
    if ( (*itV).nTracks() < m_minTracksInPV ) continue;
    if ( 0 == (*itV).nBack() ) continue;
    LHCb::RecVertex* tmp = new LHCb::RecVertex;
    tmp->setTechnique( LHCb::RecVertex::Primary );
    tmp->setPosition( (*itV).vertex() );
    double chi2 = 0.;
    for ( std::vector<TrackForPV*>::iterator itT = (*itV).tracks().begin();
          (*itV).tracks().end() != itT; ++itT ) {
      tmp->addToTracks( (*itT)->track() );
      chi2 += (*itT)->chi2( (*itV).vertex() );
    }
    tmp->setChi2AndDoF( chi2, (*itV).nTracks()-3 );
    tmp->setCovMatrix( (*itV).cov() );
    out->insert( tmp );
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FastPVFinder::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
