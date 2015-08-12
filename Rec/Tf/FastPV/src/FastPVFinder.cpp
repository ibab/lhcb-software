// Include files

// from Gaudi
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
  // declareProperty( "InputLocation",  m_inputLocation  = LHCb::TrackLocation::VP );
  declareProperty( "OutputLocation", m_outputLocation = LHCb::RecVertexLocation::Primary );
  declareProperty( "MaxIPToBeam",    m_maxIPToBeam    = 0.200 * Gaudi::Units::mm );
  declareProperty( "MaxDeltaZ",      m_maxDeltaZ      = 3.000 * Gaudi::Units::mm );
  declareProperty( "MinTracksInPV",  m_minTracksInPV  = 5    );
  declareProperty( "MaxChi2ToAdd",   m_maxChi2ToAdd   = 50.  );
  declareProperty( "MaxChi2Fit",     m_maxChi2Fit     = 12.  );
  declareProperty( "MaxChi2PerDoF",  m_maxChi2PerDoF  = 3.   );
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

  //== Select tracks with a cut on their R at beam

  std::vector<TrackForPV> pvTracks;
  for ( LHCb::Tracks::iterator itT = tracks->begin(); tracks->end() != itT; ++itT ) {
    TrackForPV temp( *itT, m_xBeam, m_yBeam );
    if ( temp.rAtBeam() < m_maxIPToBeam ) {
      pvTracks.push_back( temp );
    }
  }
  //== Create a vector of pointer to these tracks, and sort them by zBeam
  std::vector<TrackForPV*> myTracks;
  for ( std::vector<TrackForPV>::iterator itTr = pvTracks.begin(); pvTracks.end() != itTr; ++itTr ) {
    myTracks.push_back( &(*itTr) );
  }
  std::sort( myTracks.begin(), myTracks.end(), TrackForPV::LowerByZ() );
  std::vector<TrackForPV*>::iterator itT;
  if ( m_debug ) {
    for ( itT = myTracks.begin(); myTracks.end() != itT; ++itT ) {
      info() << format( "Track %3d IP %7.3f at z = %7.3f ",
                        (*itT)->track()->key(), (*itT)->rAtBeam(), (*itT)->zAtBeam() )
             << endmsg;
    }
  }

  //== At least enough tracks for a vertex
  if ( myTracks.size() < m_minTracksInPV ) return StatusCode::SUCCESS;
  unsigned int minTracks = 0.3 * myTracks.size();  // Search first for large accumulation
  int nLoop = 2;
  if ( minTracks < m_minTracksInPV ) {
    minTracks = m_minTracksInPV;
    nLoop = 1;
  }

  //== Collect all tracks with compatible zAtBeam, and then by chi2.
  std::vector<FastVertex> myVertices;
  std::vector<TrackForPV*>::iterator itT1;
  while ( 0 < nLoop ) {
    std::vector<TrackForPV*> unusedTracks;
    for ( itT1 = myTracks.begin(); myTracks.end() != itT1; ++itT1 ) {
      if ( !(*itT1)->used() ) unusedTracks.push_back( *itT1 );
    }
    if ( m_debug ) info() << "== Loop " << nLoop << " minTracks " << minTracks 
                          << " size " << unusedTracks.size() << endmsg;
    --nLoop;
    if ( unusedTracks.size() < minTracks ) continue;
    for ( itT1 = unusedTracks.begin(); unusedTracks.end() - minTracks > itT1; ++itT1 ) {
      std::vector<TrackForPV*>::iterator itT2 = itT1 +  minTracks - 1;
      if ( (*itT2)->zAtBeam() > (*itT1)->zAtBeam() + m_maxDeltaZ ) continue;  // consecutive zBeam close enough
      //== extend the range until maxDeltaZ
      while ( itT2+1 < unusedTracks.end() ) {
        if ( (*itT2+1)->zAtBeam() > (*itT1)->zAtBeam() + m_maxDeltaZ ) break;
        ++itT2;
      }
      FastVertex temp( itT1, itT2 );
      temp.removeWorsts( m_maxChi2Fit );

      //== add tracks before if they have a good chi2.
      while ( itT1 >  unusedTracks.begin() ) {
        -- itT1;
        double chi2 = (*itT1)->chi2( temp.vertex() );
        if ( chi2 < m_maxChi2ToAdd ) temp.addTrack( *itT1 );
      }
      itT1 = itT2;

      //== Add tracks after, and define the next starting point...
      bool isLast = true;
      while ( ++itT2 < unusedTracks.end() ) {
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
      if ( m_debug ) info() << "tentative vertex at z " << temp.vertex().z() << " n track " <<  temp.nTracks()
                            << " chi2/DoF " << temp.chi2PerDoF() << endmsg;

      if ( temp.nTracks() < minTracks ) continue;
      if ( temp.nbUsed() > 0.9 * temp.tracks().size() ) continue;  // avoid duplicates
      if ( temp.chi2PerDoF() > m_maxChi2PerDoF ) continue;
      myVertices.push_back( temp );
      temp.setTracksUsed( true );
      if ( m_debug ) info() << "    stored" << endmsg;
    }
    minTracks = m_minTracksInPV;
  }

  //== Try with all (unused) tracks, without zBeam grouping, iteratively.
  bool found = true;
  while ( found ) {
    std::vector<TrackForPV*> unusedTracks;
    for ( itT1 = myTracks.begin(); myTracks.end() != itT1; ++itT1 ) {
      if ( !(*itT1)->used() ) unusedTracks.push_back( *itT1 );
    }
    if ( m_debug ) {
      if ( m_debug ) info() << "-- Try unused tracks, size = " << unusedTracks.size() << endmsg;
      for ( itT = unusedTracks.begin(); unusedTracks.end() != itT; ++itT ) {
        info() << format( "Track %3d beamIP %7.3f at z = %8.3f ",
                          (*itT)->track()->key(), (*itT)->rAtBeam(), (*itT)->zAtBeam() );
        for ( std::vector<FastVertex>::iterator itV = myVertices.begin(); myVertices.end() != itV; ++itV ) {
          info() << format( " %9.1f", (*itT)->chi2( (*itV).vertex() ) );
        }
        info() << endmsg;
      }
    }
    found = false;
    if ( m_minTracksInPV > unusedTracks.size() ) continue;
    FastVertex temp( unusedTracks.begin(), unusedTracks.end() -1 );
    temp.removeWorsts( m_maxChi2Fit );
    if ( temp.nTracks()    >= m_minTracksInPV &&
         temp.chi2PerDoF() <  m_maxChi2PerDoF ) {
      myVertices.push_back( temp );
      temp.setTracksUsed( true );
      found = true;
      if ( m_debug ) {
        info() << format( "Unused Vertex at x %7.3f y%7.3f z%7.3f  Ntr %3d chi2/DoF%7.3f",
                          temp.vertex().x(), temp.vertex().y(), temp.vertex().z(),
                          temp.nTracks(), temp.chi2PerDoF() )
               << endmsg;
        for ( std::vector<TrackForPV*>::iterator itT = temp.tracks().begin();
              temp.tracks().end() != itT; ++itT ) {
          info() << format( "     Track %3d IP %7.3f at z = %7.3f chi2 %7.3f ",
                            (*itT)->track()->key(), (*itT)->rAtBeam(), (*itT)->zAtBeam(),
                            (*itT)->chi2(temp.vertex()) )
                 << endmsg;
        }
      }
    }
  }

  std::vector<FastVertex>::iterator itV, itV2;
  //== Summary
  if ( m_debug ) {
    info() << "Number of reconstructed vertices " << myVertices.size() << endmsg;
    for ( itV = myVertices.begin(); myVertices.end() != itV; ++itV ) {
      info() << format( "Vertex at x %7.3f y%7.3f z%7.3f  Ntr %3d ",
                        (*itV).vertex().x(), (*itV).vertex().y(), (*itV).vertex().z(), (*itV).nTracks() )
             << endmsg;
      for ( std::vector<TrackForPV*>::iterator itT = (*itV).tracks().begin();
            (*itV).tracks().end() != itT; ++itT ) {
        info() << format( "     Track %3d IP %7.3f at z = %7.3f chi2 %7.3f ",
                          (*itT)->track()->key(), (*itT)->rAtBeam(), (*itT)->zAtBeam(),
                          (*itT)->chi2((*itV).vertex()) )
               << endmsg;
      }
    }
  }

  //== Store the vertices in the appropriate container
  for ( itV = myVertices.begin(); myVertices.end() != itV; ++itV ) {
    LHCb::RecVertex* tmp = new LHCb::RecVertex;
    tmp->setTechnique( LHCb::RecVertex::Primary );
    tmp->setPosition( (*itV).vertex() );
    for ( std::vector<TrackForPV*>::iterator itT = (*itV).tracks().begin();
          (*itV).tracks().end() != itT; ++itT ) {
      tmp->addToTracks( (*itT)->track() );
    }
    tmp->setChi2AndDoF( (*itV).chi2(), 2*(*itV).nTracks()-3 );
    tmp->setCovMatrix( (*itV).cov() );
    out->insert( tmp );
  }

  setFilterPassed( !out->empty() );

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
