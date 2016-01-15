// $Id: MakeMuonTracks.cpp,v 1.1 2010-06-04 13:02:57 svecchi Exp $
// Include files

// from STD
#include <vector>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from LHCbKernel
#include "Kernel/LHCbID.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/StateVector.h"
#include "Event/State.h"

// from MuonTrackRec
#include "MuonInterfaces/MuonTrack.h"
#include "MuonInterfaces/MuonHit.h"

//tools
#include "TrackInterfaces/ITrackMomentumEstimate.h"
// local
#include "MakeMuonTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MakeMuonTracks based on CopyNNetTracks by Jan  Amoraal
//
// 2009-12-3: Stefania Vecchi:
//-----------------------------------------------------------------------------

using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MakeMuonTracks )

MakeMuonTracks::MakeMuonTracks( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "TracksOutputLocation", m_trackOutputLoc  = TrackLocation::Muon );
  declareProperty( "MuonRecTool"         , m_muonRecToolName = "MuonNNetRec"       );
  declareProperty( "MaxNTiles"           , m_MaxNTiles = 6       );
  declareProperty( "SkipBigClusters"     , m_skipBigClusters = false       );
  declareProperty( "BField"              , m_Bfield = false );
}

MakeMuonTracks::~MakeMuonTracks() {}

StatusCode MakeMuonTracks::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_muonRecTool = tool<IMuonTrackRec>( m_muonRecToolName, "MuonRecTool", this );

  debug()<<" MakeMuonTracks will make the tracks with "<<m_muonRecToolName<<" Tool "<<endmsg;

  //calculate the transverse momentum
  m_fCalcMomentum = tool<ITrackMomentumEstimate>("TrackPtKick");
  if ( msgLevel(MSG::DEBUG) ) debug() << "In init, PTKick from geometry " << endreq;

  m_muonDetector =getDet<DeMuonDetector>(DeMuonLocation::Default);

  return StatusCode::SUCCESS;
}

StatusCode MakeMuonTracks::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  if (exist<LHCb::Tracks>(m_trackOutputLoc)) {    //already called with this output location, exit
    return StatusCode::SUCCESS;
  }
  Tracks* tracks = new Tracks();
  put( tracks, m_trackOutputLoc );

  typedef std::vector< MuonTrack* > MTracks;
  typedef std::vector< const MuonHit*   > MHits  ;
  typedef std::vector<LHCb::MuonTileID> MTileIDs;
  const MTracks* mTracks = m_muonRecTool->tracks();

  if(mTracks->size() == 0 && m_muonRecToolName == "MuonCombRec" ){
    m_muonRecTool->setSeedStation(3); // Try starting with M4
    mTracks = m_muonRecTool->tracks();
  }

  debug()<<"found "<<mTracks->size()<<" MuonTracks "<<endmsg;
  for ( MTracks::const_iterator t = mTracks->begin(), tEnd = mTracks->end(); t != tEnd; ++t ) {
    /*
    if((*t)->isClone()) {
      debug()<<"Skip this track: it is a CLONE! "<<endmsg;
      continue;
    }
    */

    /// New track
    Track* track = new Track();
    /// Get the hits
    MHits hits   = (*t)->getHits();

    Gaudi::XYZPoint trackPos((*t)->bx() + (*t)->sx() * m_muonDetector->getStationZ(0),
                             (*t)->by() + (*t)->sy() * m_muonDetector->getStationZ(0),
                             m_muonDetector->getStationZ(0));
    LHCb::State state( LHCb::StateVector( trackPos,
                                          Gaudi::XYZVector( (*t)->sx(), (*t)->sy(), 1.0 ), 1./10000.));

    if(m_Bfield){
      double qOverP(0.), sigmaQOverP(0.);
      m_fCalcMomentum->calculate(&state, qOverP, sigmaQOverP);
      state.setQOverP(qOverP);
    }

    state.setLocation( State::Muon );
    /// add state to new track
    debug() << "Muon state = " << state << endmsg;
    track->addToStates( state );

    debug()<< " MakeMuonTracks "<<(*t)->getHits().size() <<" MuonHits to the track "<<endmsg;
    int ntile=0;
    bool flagBIGcluster = false;                                //flag to identify tracks with too big clusters
    for ( MHits::const_iterator h = hits.begin(); h != hits.end(); ++h ){
      const MTileIDs Tiles = (*h)->getLogPadTiles();
      debug()<< " // "<< (*h)->getLogPadTiles().size()<<" tiles in station "<< (*h)->station() ;

      if(Tiles.size()>m_MaxNTiles) flagBIGcluster = true;

      for (MTileIDs::const_iterator it = Tiles.begin(); it!= Tiles.end(); ++it){
        track->addToLhcbIDs( LHCbID( *it ) );
        ntile++;
      }
    }
    debug()<<endmsg;
    debug()<< " in total "<<ntile<<" tiles"<<endmsg;

    if(m_skipBigClusters && flagBIGcluster) {

      debug()<< " track not saved due to BIG cluster rejection"<< endmsg;
      delete track;

    } else {

      track->setPatRecStatus( Track::PatRecIDs );
      track->setType( Track::Muon );
      tracks->insert( track );
      debug()<< " track  saved in the container"<<m_trackOutputLoc << endmsg;

    }
  }
  debug()<<" end MakeMuonTrack"<<endmsg;

  return StatusCode::SUCCESS;
}
