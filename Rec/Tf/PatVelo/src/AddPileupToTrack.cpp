// Include files

// from Gaudi
#include "GaudiKernel/IUpdateManagerSvc.h"

// local
#include "AddPileupToTrack.h"
#include "PatVeloSpaceTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloGeneric
//
// 2007-06-15 : David Hutchcroft
//-----------------------------------------------------------------------------

namespace Tf {
  DECLARE_ALGORITHM_FACTORY( AddPileupToTrack )
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

Tf::AddPileupToTrack::AddPileupToTrack( const std::string& name,
					ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_velo(NULL)
  , m_angleUtils(-Gaudi::Units::pi,Gaudi::Units::pi)
  , m_zStartState(0.)
  , m_errScale(0.)
  , m_RWindow(0.)
{
  declareProperty( "ExtendTracksLocation" ,
                   m_extendTracksLocation = LHCb::TrackLocation::Velo );
  declareProperty( "ScaleTrackError" , m_errScale = 1.5 );
  declareProperty( "RSearchWindow" , m_RWindow = 0.1*Gaudi::Units::mm );
}

//=============================================================================
// Destructor
//=============================================================================

Tf::AddPileupToTrack::~AddPileupToTrack() {}

//=============================================================================
// Initialization
//=============================================================================

StatusCode Tf::AddPileupToTrack::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();

  if ( sc.isFailure() ) return sc;
  m_isDebug   = msgLevel( MSG::DEBUG   );
  m_isVerbose = msgLevel( MSG::VERBOSE );

  if( m_isDebug ) debug() << "==> Initialize" << endmsg;

  m_velo = getDet<DeVelo>( DeVeloLocation::Default );

  for ( auto sens = m_velo->sensorsBegin() ;
	sens != m_velo->sensorsEnd() ;
	++sens ){
    m_isLeft[(*sens)->sensorNumber()] = (*sens)->isLeft();
  }

  for ( auto puSens = m_velo->pileUpSensorsBegin() ;
	puSens != m_velo->pileUpSensorsEnd() ;
	++puSens ){
    if( ! (*puSens)->isReadOut() ) continue; // skip "dead" sensors
    DeVeloRType *sensor = dynamic_cast<DeVeloRType *>(*puSens);
    if( !sensor ) {
      return Error("pile up is not R sensor");
    }
    unsigned int nSens = sensor->sensorNumber();
    if( sensor->isLeft() ) m_PULeft.push_back(nSens);
    if( sensor->isRight() ) m_PURight.push_back(nSens);

    m_clusCont.addSensor(nSens);

    m_zSens[nSens] = sensor->z();
    if( m_zStartState == 0. ){
      m_zStartState = m_zSens[nSens];
    }
    if(m_radius.empty()){
      // note ignoring alignment for now
      // FIXME do one per sensor and include alignment
      m_radius.resize(512,0.);
      for( unsigned int i = 0 ; i < 512 ; ++i ){
	m_radius[i] = sensor->rOfStrip(i);
      }
    }
    m_phiRange[nSens].resize(4,std::pair<double,double>(0.,0.));
    for( unsigned int i = 0 ; i < 4 ; ++i ){
      m_phiRange[nSens][i] = sensor->globalPhiRange(i);
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode Tf::AddPileupToTrack::execute() {

  if( m_isDebug ) debug() << "==> Execute" << endmsg;

  // load pileup hits
  StatusCode sc = loadPileupHits();
  if(!sc) return sc;

  // loop over tracks to extend
  LHCb::Tracks * tracks = getIfExists<LHCb::Tracks>(m_extendTracksLocation);
  if( !tracks ) return Error(format("Failed to find track container %s",
				    m_extendTracksLocation.c_str()),
				    StatusCode::FAILURE);
  for (auto &track : *tracks){
    if( ! track->checkFlag(LHCb::Track::Backward) ) continue;
    extendWithPUTrack(*track);
  }
  
  return StatusCode::SUCCESS;
}

StatusCode Tf::AddPileupToTrack::loadPileupHits(){
  LHCb::VeloLiteCluster::FastContainer *veloClus= getIfExists<LHCb::VeloLiteCluster::FastContainer>(LHCb::VeloLiteClusterLocation::Default);
  if( !veloClus ) {
    return Error(format("Failed to find velo lite cluster container %s",
			LHCb::VeloLiteClusterLocation::Default.c_str()),
		 StatusCode::FAILURE);
  }
  m_clusCont.clear();
  unsigned int nPUHit = 0;
  for (auto vl : *veloClus){
    if( vl.isPileUp() ){
      m_clusCont.addCluster(vl,m_radius[vl.channelID().strip()%512]);
      ++nPUHit;
    }
  }
  if( m_isDebug ) debug() << "Loaded " << nPUHit << " PU clusters" << endmsg;
  // sort clusters into ascending strip number
  for (auto &sensCont : m_clusCont.getCont() ){
    for ( unsigned int i = 0 ; i < 4 ; ++i ){
      sort(sensCont.second.getVec(i).begin() ,
	   sensCont.second.getVec(i).end() ,
	   [](const clusInPUSector::clusInfo& a, 
	      const clusInPUSector::clusInfo& b){return a.m_r < b.m_r;});
      if ( m_isVerbose ) verbose() 
			   << "sens " << sensCont.first
			   << " pos " << i 
			   << " size " << sensCont.second.getVec(i).size()
			   << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

void Tf::AddPileupToTrack::extendWithPUTrack(LHCb::Track &track){
  bool isLeft = true;
  for( auto id : track.lhcbIDs() ){
    if( id.isVelo() ){
      if( !m_isLeft[id.veloID().sensor()] ) isLeft = false;
      break;
    }
  }
  if ( m_isVerbose ) verbose() 
		       << "extending " << (isLeft ? "left" : "right")
		       << " side backward track, key=" << track.key() 
		       << endmsg;
  // extrapolate track to PU sensor(s)
  if( isLeft ){
    for(auto sensNum : m_PULeft){
      addBestClus( sensNum, track );
    }
  }else{
    for(auto sensNum : m_PURight){
      addBestClus( sensNum, track );
    }
  }
}

void Tf::AddPileupToTrack::addBestClus(const unsigned int &sensNum,
				       LHCb::Track &track){
  double z = m_zSens[sensNum];
  LHCb::State & state = track.closestState(z);
  double dZ = (z-state.z());
  double predX = state.x() + dZ*state.tx();
  double predY = state.y() + dZ*state.ty();
  double predR = sqrt(predX*predX + predY*predY);
  double predRErr = sqrt( fabs(predX)*(state.errX2() + 
				       fabs(dZ)*state.errTx2()) +
			  fabs(predY)*(state.errY2() + 
				       fabs(dZ)*state.errTy2()) / predR);
  if( predR > m_radius[511] || predR < m_radius[0] ) return;
  double predPhi = atan2(predY,predX);
  for( unsigned int p = 0 ; p < 4 ; ++p ){
    if(m_angleUtils.contains(m_phiRange[sensNum][p], predPhi)){
      if( m_clusCont.empty(sensNum,p) ) continue;
      LHCb::VeloLiteCluster vl;
      bool OK = m_clusCont.bestCluster(sensNum,p,predR,
				       (m_errScale*predRErr)+m_RWindow, vl);
      if(OK) if(m_isDebug) debug() << "Found cluster " << vl.channelID()
				   << " for track key " << track.key()
				   << endmsg;
      track.addToLhcbIDs(LHCb::LHCbID(vl.channelID()));
    }
  }
}

StatusCode Tf::AddPileupToTrack::finalize() {
  if( m_isDebug ) debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();
}

