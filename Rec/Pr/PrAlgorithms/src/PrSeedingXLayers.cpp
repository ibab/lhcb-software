// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/Track.h"
#include "Event/StateParameters.h"
// local
#include "PrSeedingXLayers.h"
#include "PrPlaneCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrSeedingXLayers
//
// 2013-02-14 : Olivier Callot
// 2013-03-21 : Yasmine Amhis Modification 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrSeedingXLayers )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrSeedingXLayers::PrSeedingXLayers( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName",           m_inputName            = LHCb::TrackLocation::Forward );
  declareProperty( "OutputName",          m_outputName           = LHCb::TrackLocation::Seed    );
  declareProperty( "HitManagerName",      m_hitManagerName       = "PrFTHitManager"  );
  declareProperty( "MaxChi2InTrack",      m_maxChi2InTrack       = 5.                );
  declareProperty( "TolXInf",             m_tolXInf              = 0.5 * Gaudi::Units::mm );
  declareProperty( "TolXSup",             m_tolXSup              = 3.0 * Gaudi::Units::mm );
  declareProperty( "MinXPlanes",          m_minXPlanes           = 5                      );
  declareProperty( "MaxChi2PerDoF",       m_maxChi2PerDoF        = 3.0                    );
  declareProperty( "XOnly",               m_xOnly                = false                  );
  
  declareProperty( "MaxIpAtZero",         m_maxIpAtZero          = 2500.           );
  //declareProperty( "MaxIpAtZero",         m_maxIpAtZero          = 1000.           );
  

  // Parameters for debugging
  declareProperty( "DebugToolName"     , m_debugToolName  = ""        );
  declareProperty( "WantedKey"         , m_wantedKey      = -100      );
  declareProperty( "TimingMeasurement" , m_doTiming       = false     );
}
//=============================================================================
// Destructor
//=============================================================================
PrSeedingXLayers::~PrSeedingXLayers() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrSeedingXLayers::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hitManager = tool<PrHitManager>( m_hitManagerName );
  m_hitManager->buildGeometry();
  m_geoTool = tool<PrGeometryTool>("PrGeometryTool");

  m_debugTool   = 0;
  if ( "" != m_debugToolName ) {
    m_debugTool = tool<IPrDebugTool>( m_debugToolName );
  } else {
    m_wantedKey = -100;  // no debug
  }

  if ( m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool/Timer", this );
    m_timeTotal   = m_timerTool->addTimer( "PrSeeding total" );
    m_timerTool->increaseIndent();
    m_timeFromForward = m_timerTool->addTimer( "Convert Forward" );
    m_timeXProjection = m_timerTool->addTimer( "X Projection" );
    m_timeStereo      = m_timerTool->addTimer( "Add stereo" );
    m_timeFinal       = m_timerTool->addTimer( "Convert tracks" );
    m_timerTool->decreaseIndent();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrSeedingXLayers::execute() {
  //always () << "Welcome to quick and dirty fix from Olivier " << endmsg; 
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  if ( m_doTiming ) {
    m_timerTool->start( m_timeTotal );
    m_timerTool->start( m_timeFromForward );
  }

  LHCb::Tracks* result = new LHCb::Tracks();
  put( result, m_outputName );

  // m_hitManager->decodeData();   //=== Should be done only if needed !!!

  for ( unsigned int zone = 0; m_hitManager->nbZones() > zone; ++zone ) {
    for ( PrHits::const_iterator itH = m_hitManager->hits( zone ).begin();
          m_hitManager->hits( zone ).end() != itH; ++itH ) {
      (*itH)->setUsed( false );
    }
  }

  //== If needed, debug the cluster associated to the requested MC particle.
  if ( 0 <= m_wantedKey ) {
    info() << "--- Looking for MCParticle " << m_wantedKey << endmsg;
    for ( unsigned int zone = 0; m_hitManager->nbZones() > zone; ++zone ) {
      for ( PrHits::const_iterator itH = m_hitManager->hits( zone ).begin();
            m_hitManager->hits( zone ).end() != itH; ++itH ) {
        if ( matchKey( *itH ) ) printHit( *itH, " " );
      }
    }
  }
  //====================================================================
  // Extract the seed part from the forward tracks.
  //====================================================================
  if ( "" != m_inputName ) {
    LHCb::Tracks* forward = get<LHCb::Tracks>( m_inputName );
    for ( LHCb::Tracks::iterator itT = forward->begin(); forward->end() != itT; ++itT ) {
      std::vector<LHCb::LHCbID> ids;
      for ( std::vector<LHCb::LHCbID>::const_iterator itId = (*itT)->lhcbIDs().begin();
            (*itT)->lhcbIDs().end() != itId; ++itId ) {
        if ( (*itId).isFT() ) {
          LHCb::FTChannelID ftId =(*itId).ftID();
          int zoneNb = 2 * ftId.layer() + ftId.quarter()/2;
          PrHitZone* zone = m_hitManager->zone(zoneNb);
	  // for next dev, think of using binary search (that takes lhcb ids)
          for ( PrHits::iterator itH = zone->hits().begin(); zone->hits().end() != itH; ++itH ) {
            if ( (*itH)->id() == *itId ) (*itH)->setUsed( true );
          }
          ids.push_back( *itId );
        }
      }
      LHCb::Track* seed = new LHCb::Track;
      seed->setLhcbIDs( ids );
      seed->setType( LHCb::Track::Ttrack );
      seed->setHistory( LHCb::Track::PatSeeding );
      seed->setPatRecStatus( LHCb::Track::PatRecIDs );
      seed->addToStates( (*itT)->closestState( 9000. ) );
      result->insert( seed );
    }
  }
  m_trackCandidates.clear();
  if ( m_doTiming ) {
    m_timerTool->stop( m_timeFromForward );
  }

  for ( unsigned int zone = 0; 2 > zone; ++zone ) {
    if ( m_doTiming ) {
      m_timerTool->start( m_timeXProjection);
    }
    findXProjections( zone );

    if ( m_doTiming ) {
      m_timerTool->stop( m_timeXProjection);
      m_timerTool->start( m_timeStereo);
    }

    if ( ! m_xOnly ) addStereo( zone );
    if ( m_doTiming ) {
      m_timerTool->stop( m_timeStereo);
    }
  }

  if ( m_doTiming ) {
    m_timerTool->start( m_timeFinal);
  }

  makeLHCbTracks( result );

  if ( m_doTiming ) {
    m_timerTool->stop( m_timeFinal);
    float tot = m_timerTool->stop( m_timeTotal );
    info() << format( "                                            Time %8.3f ms", tot )
           << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrSeedingXLayers::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//
//=========================================================================
void PrSeedingXLayers::findXProjections( unsigned int zone ){
  m_xCandidates.clear();
  for ( unsigned int iCase = 0 ; 3 > iCase ; ++iCase ) {
    int firstZone = zone;
    int lastZone  = 22 + zone;
    if ( 1 == iCase ) firstZone = zone + 6;
    if ( 2 == iCase ) lastZone  = 16 + zone;
    
    PrHitZone* fZone = m_hitManager->zone( firstZone );
    PrHitZone* lZone = m_hitManager->zone( lastZone  );
    PrHits& fHits = fZone->hits();
    PrHits& lHits = lZone->hits();
    
        
    float zRatio =  lZone->z(0.) / fZone->z(0.);
    
  

    
    std::vector<PrHitZone*> xZones;
    for ( int kk = firstZone+2; lastZone > kk ; kk += 2 ) {
      if ( m_hitManager->zone( kk )->isX() ) xZones.push_back( m_hitManager->zone(kk) );
    }
    
    PrHits::iterator itLBeg = lHits.begin();
    for ( PrHits::iterator itF = fHits.begin(); fHits.end() != itF; ++itF ) {
      if ( 0 != iCase && (*itF)->isUsed() ) continue;
      float minXl = (*itF)->x() * zRatio - m_maxIpAtZero * ( zRatio - 1 );
      float maxXl = (*itF)->x() * zRatio + m_maxIpAtZero * ( zRatio - 1 );
  

      if ( matchKey( *itF ) ) info() << "Search from " << minXl << " to " << maxXl << endmsg;
      while ( itLBeg != lHits.end() && (*itLBeg)->x() < minXl ) {
        ++itLBeg;
        if ( lHits.end() == itLBeg ) break;
      }
      PrHits::iterator itL = itLBeg;
      while ( itL != lHits.end() && (*itL)->x() < maxXl ) {
        if ( 0 != iCase && (*itL)->isUsed() ) {
          ++itL;
          continue;
        }
        float tx = ((*itL)->x() - (*itF)->x()) / (lZone->z() - fZone->z() );
        float x0 = (*itF)->x() - (*itF)->z() * tx;
        bool debug = matchKey( *itF ) || matchKey( *itL );
        PrHits xCandidate;
        xCandidate.push_back( *itF );
        int nMiss = 0;
        if ( 0 != iCase ) nMiss = 1;
        

	//loop over ALL x zones 
        for ( std::vector<PrHitZone*>::iterator itZ = xZones.begin(); xZones.end() != itZ; ++itZ ) {
          float xP   = x0 + (*itZ)->z() * tx;
          float xMin = xP - m_tolXInf;
          float xMax = xP + m_tolXSup;
          if ( x0 < 0 ) {
            xMin = xP - m_tolXSup;
            xMax = xP + m_tolXInf;
          }
          PrHit* best = NULL;
          for ( PrHits::iterator itH = (*itZ)->hits().begin(); (*itZ)->hits().end() != itH; ++itH ) {
            if ( (*itH)->x() < xMin ) continue;
            if ( (*itH)->x() > xMax ) break;
            best = *itH;
            xCandidate.push_back( best );
          }
          if ( NULL == best ) {
            nMiss++;
            if ( 1 < nMiss ) break;
          }
        }
        if ( nMiss < 2 ) {
          xCandidate.push_back( *itL );
          PrSeedTrack temp( zone, m_geoTool->zReference(), xCandidate );
          
          bool OK = fitTrack( temp );
          if ( debug ) {
            info() << "=== before fit === OK = " << OK << endmsg;
            printTrack( temp );
          }
          
          while ( !OK ) {
            OK = removeWorstAndRefit( temp );
          }
          setChi2( temp );
          if ( debug ) {
            info() << "=== after fit === chi2/dof = " << temp.chi2PerDoF() << endmsg;
            printTrack( temp );
          }
          if ( OK && 
               temp.hits().size() >= m_minXPlanes &&
               temp.chi2PerDoF()  < m_maxChi2PerDoF   ) {
            if ( temp.hits().size() == 6 ) {
              for ( PrHits::iterator itH = temp.hits().begin(); temp.hits().end() != itH; ++ itH) {
                (*itH)->setUsed( true );
              }
            }
            
            m_xCandidates.push_back( temp );
            if ( debug ) {
              info() << "Candidate chi2PerDoF " << temp.chi2PerDoF() << endmsg;
              printTrack( temp );
            }
          } else {
            if ( debug ) info() << "OK " << OK << " size " << temp.hits().size() << " chi2 " << temp.chi2() << endmsg;
          }
        }
        ++itL;
      }
    }
  }

  std::sort( m_xCandidates.begin(), m_xCandidates.end(), PrSeedTrack::GreaterBySize() );

  //====================================================================
  // Remove clones, i.e. share more than 2 hits
  //====================================================================
  for ( PrSeedTracks::iterator itT1 = m_xCandidates.begin(); m_xCandidates.end() !=itT1; ++itT1 ) {
    if ( !(*itT1).valid() ) continue;
    if ( (*itT1).hits().size() != 6 ) {
      int nUsed = 0;
      for ( PrHits::iterator itH = (*itT1).hits().begin(); (*itT1).hits().end() != itH; ++ itH) {
        if ( (*itH)->isUsed() ) ++nUsed;
      }
      if ( 1 < nUsed ) {
        (*itT1).setValid( false );
        continue;
      }
    }    

    for ( PrSeedTracks::iterator itT2 = itT1 + 1; m_xCandidates.end() !=itT2; ++itT2 ) {
      if ( !(*itT2).valid() ) continue;
      int nCommon = 0;
      PrHits::iterator itH1 = (*itT1).hits().begin();
      PrHits::iterator itH2 = (*itT2).hits().begin();
      while ( itH1 != (*itT1).hits().end() && itH2 != (*itT2).hits().end() ) {
        if ( (*itH1)->id() == (*itH2)->id() ) {
          nCommon++;
          ++itH1;
          ++itH2;
        } else if ( (*itH1)->id() < (*itH2)->id() ) {
          ++itH1;
        } else {
          ++itH2;
        }
      }
      if ( nCommon > 2 ) {
        if ( (*itT1).hits().size() > (*itT2).hits().size() ) {
          (*itT2).setValid( false );
        } else if ( (*itT1).hits().size() < (*itT2).hits().size() ) {
          (*itT1).setValid( false );
        } else if ( (*itT1).chi2PerDoF() < (*itT2).chi2PerDoF() ) {
          (*itT2).setValid( false );
        } else {
          (*itT1).setValid( false );
        }
      }
    }
    if ( m_xOnly ) m_trackCandidates.push_back( *itT1 );
  }
}

//=========================================================================
//  
//=========================================================================
void PrSeedingXLayers::addStereo( unsigned int zone ) {
  PrSeedTracks xProjections;
  for ( PrSeedTracks::iterator itT1 = m_xCandidates.begin(); m_xCandidates.end() !=itT1; ++itT1 ) {
    if ( !(*itT1).valid() ) continue;
    xProjections.push_back( *itT1 );
  }

  unsigned int firstZone = zone + 2;
  unsigned int lastZone  = zone + 22;
  for ( PrSeedTracks::iterator itT = xProjections.begin(); xProjections.end() !=itT; ++itT ) {
    bool debug = false;
    int nMatch = 0;
    for ( PrHits::iterator itH = (*itT).hits().begin(); (*itT).hits().end() != itH; ++itH ) {
      if (  matchKey( *itH ) ) ++nMatch;
    }
    debug = nMatch > 3;
    if ( debug ) {
      info() << "Processing track " << endmsg;
      printTrack( *itT );
    }
    
    PrHits myStereo;
    for ( unsigned int kk = firstZone; lastZone > kk ; kk+= 2 ) {
      if ( m_hitManager->zone(kk)->isX() ) continue;
      float dxDy = m_hitManager->zone(kk)->dxDy();
      float zPlane = m_hitManager->zone(kk)->z();
      
      float xPred = (*itT).x( m_hitManager->zone(kk)->z() );
      float xMin = xPred + 2700. * dxDy;
      float xMax = xPred - 2700. * dxDy;
      if ( xMin > xMax ) {
        float tmp = xMax;
        xMax = xMin;
        xMin = tmp;
      }
      for ( PrHits::iterator itH = m_hitManager->zone(kk)->hits().begin();
            m_hitManager->zone(kk)->hits().end() != itH; ++itH ) {
        if ( (*itH)->x() < xMin ) continue;
        if ( (*itH)->x() > xMax ) break;
        (*itH)->setCoord( ((*itH)->x() - xPred) / dxDy / zPlane );
        if ( debug ) {
          if ( matchKey( *itH ) ) printHit( *itH, "" );
        }
        if ( 0 == zone && (*itH)->coord() < -0.005 ) continue;
        if ( 1 == zone && (*itH)->coord() >  0.005 ) continue;
        myStereo.push_back( *itH );
      }
    }
    std::sort( myStereo.begin(), myStereo.end(), PrHit::LowerByCoord() );

    PrPlaneCounter plCount;
    unsigned int firstSpace = m_trackCandidates.size();

    PrHits::iterator itBeg = myStereo.begin();
    PrHits::iterator itEnd = itBeg + 5;
    
    while ( itEnd < myStereo.end() ) {
      float tolTy = 0.002 + .02 * fabs( (*itBeg)->coord() );
      if ( (*(itEnd-1))->coord() - (*itBeg)->coord() < tolTy ) {
        while( itEnd+1 < myStereo.end() &&
               (*itEnd)->coord() - (*itBeg)->coord() < tolTy ) {
          ++itEnd;
        }
        plCount.set( itBeg, itEnd );
        if ( 4 < plCount.nbDifferent() ) {
          PrSeedTrack temp( *itT );
          for ( PrHits::iterator itH = itBeg; itEnd != itH; ++itH ) temp.addHit( *itH );
          bool ok = fitTrack( temp );
          ok = fitTrack( temp );
          ok = fitTrack( temp );
          if ( debug ) {
            info() << "Before fit-and-remove" << endmsg;
            printTrack( temp );
          }
          while ( !ok && temp.hits().size() > 10 ) {
            ok = removeWorstAndRefit( temp );
            if ( debug ) {
              info() << "    after removing worse" << endmsg;
              printTrack( temp );
            }
          }
          if ( ok ) {
            setChi2( temp );
            if ( debug ) {
              info() << "=== Candidate chi2 " << temp.chi2PerDoF() << endmsg;
              printTrack( temp );
            }
            if ( temp.hits().size() > 9 || 
                 temp.chi2PerDoF() < m_maxChi2PerDoF ) {
              m_trackCandidates.push_back( temp );
            }
            itBeg += 4;
          }
        }
      }
      ++itBeg;
      itEnd = itBeg + 5;
    }
    //=== Remove bad candidates: Keep the best for this input track
    if ( m_trackCandidates.size() > firstSpace+1 ) {
      for ( unsigned int kk = firstSpace; m_trackCandidates.size()-1 > kk ; ++kk ) {
        if ( !m_trackCandidates[kk].valid() ) continue;
        for ( unsigned int ll = kk + 1; m_trackCandidates.size() > ll; ++ll ) {
          if ( !m_trackCandidates[ll].valid() ) continue;
          if ( m_trackCandidates[ll].hits().size() < m_trackCandidates[kk].hits().size() ) {
            m_trackCandidates[ll].setValid( false );
          } else if ( m_trackCandidates[ll].hits().size() > m_trackCandidates[kk].hits().size() ) {
            m_trackCandidates[kk].setValid( false );
          } else if ( m_trackCandidates[kk].chi2() < m_trackCandidates[ll].chi2() ) {
            m_trackCandidates[ll].setValid( false );
          } else {
            m_trackCandidates[kk].setValid( false );
          }  
        }   
      }
    }
  }
}

//=========================================================================
//  Fit the track, return OK if fit sucecssfull
//=========================================================================
bool PrSeedingXLayers::fitTrack( PrSeedTrack& track ) {

  for ( int loop = 0; 3 > loop ; ++loop ) {
    //== Fit a parabola
    float s0   = 0.;
    float sz   = 0.;
    float sz2  = 0.;
    float sz3  = 0.;
    float sz4  = 0.;
    float sd   = 0.;
    float sdz  = 0.;
    float sdz2 = 0.;
    float sdz3 = 0.;

    float t0  = 0.;
    float tz  = 0.;
    float tz2 = 0.;
    float td  = 0.;
    float tdz = 0.;

    for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
      float w = (*itH)->w();
      float z = (*itH)->z() - m_geoTool->zReference();
      if ( (*itH)->dxDy() != 0 ) {
        if ( 0 == loop ) continue;
        float dy = track.deltaY( *itH );
        t0   += w;
        tz   += w * z;
        tz2  += w * z * z;
        td   += w * dy;
        tdz  += w * dy * z;
      }
      float d = track.distance( *itH );
      s0   += w;
      sz   += w * z;
      sz2  += w * z * z;
      sz3  += w * z * z * z;
      sz4  += w * z * z * z * z;
      sd   += w * d;
      sdz  += w * d * z;
      sdz2 += w * d * z * z;
      sdz3 += w * d * z * z;
    }
    float b1 = sz  * sz  - s0  * sz2;
    float c1 = sz2 * sz  - s0  * sz3;
    float d1 = sd  * sz  - s0  * sdz;
    float b2 = sz2 * sz2 - sz * sz3;
    float c2 = sz3 * sz2 - sz * sz4;
    float d2 = sdz * sz2 - sz * sdz2;

    float den = (b1 * c2 - b2 * c1 );
    float db  = (d1 * c2 - d2 * c1 ) / den;
    float dc  = (d2 * b1 - d1 * b2 ) / den;
    float da  = ( sd - db * sz - dc * sz2 ) / s0;

    float day = 0.;
    float dby = 0.;
    if ( t0 > 0. ) {
      float deny = (tz  * tz - t0 * tz2);
      day = -(tdz * tz - td * tz2) / deny;
      dby = -(td  * tz - t0 * tdz) / deny;
    }

    track.updateParameters( da, db, dc, day, dby );
    float maxChi2 = 0.;
    for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
      float chi2 = track.chi2( *itH );
      if ( chi2 > maxChi2 ) {
        maxChi2 = chi2;
      }
    }
    if ( m_maxChi2InTrack > maxChi2 ) return true;
  }
  return false;
}

//=========================================================================
//  Remove the worst hit and refit.
//=========================================================================
bool PrSeedingXLayers::removeWorstAndRefit ( PrSeedTrack& track ) {
  float maxChi2 = 0.;
  PrHits::iterator worst = track.hits().begin();
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
    float chi2 = track.chi2( *itH );
    if ( chi2 > maxChi2 ) {
      maxChi2 = chi2;
      worst = itH;
    }
  }
  track.hits().erase( worst );
  return fitTrack( track );
}
//=========================================================================
//  Set the chi2 of the track
//=========================================================================
void PrSeedingXLayers::setChi2 ( PrSeedTrack& track ) {
  float chi2 = 0.;
  int   nDoF = -3;  // Fitted a parabola
  bool hasStereo = false;
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
    float d = track.distance( *itH );
    if ( (*itH)->dxDy() != 0 ) hasStereo = true;
    float w = (*itH)->w();
    chi2 += w * d * d;
    nDoF += 1;
  }
  if ( hasStereo ) nDoF -= 2;
  track.setChi2( chi2, nDoF );
}

//=========================================================================
//  Convert to LHCb tracks
//=========================================================================
void PrSeedingXLayers::makeLHCbTracks ( LHCb::Tracks* result ) {
  for ( PrSeedTracks::iterator itT = m_trackCandidates.begin();
        m_trackCandidates.end() != itT; ++itT ) {
    if ( !(*itT).valid() ) continue;

    //info() << "==== Store track ==== chi2/dof " << (*itT).chi2PerDoF() << endmsg;
    //printTrack( *itT );
    
    LHCb::Track* tmp = new LHCb::Track;
    //tmp->setType( LHCb::Track::Long );
    //tmp->setHistory( LHCb::Track::PatForward );
    tmp->setType( LHCb::Track::Ttrack );
    tmp->setHistory( LHCb::Track::PatSeeding );
    double qOverP = m_geoTool->qOverP( *itT );

    LHCb::State tState;
    double z = StateParameters::ZEndT;
    tState.setLocation( LHCb::State::AtT );
    tState.setState( (*itT).x( z ), (*itT).y( z ), z, (*itT).xSlope( z ), (*itT).ySlope( ), qOverP );

    //== overestimated covariance matrix, as input to the Kalman fit

    tState.setCovariance( m_geoTool->covariance( qOverP ) );
    tmp->addToStates( tState );

    //== LHCb ids.

    tmp->setPatRecStatus( LHCb::Track::PatRecIDs );
    for ( PrHits::iterator itH = (*itT).hits().begin(); (*itT).hits().end() != itH; ++itH ) {
      tmp->addToLhcbIDs( (*itH)->id() );
    }
    tmp->setChi2PerDoF( (*itT).chi2PerDoF() );
    tmp->setNDoF(       (*itT).nDoF() );
    result->insert( tmp );
  }
}

//=========================================================================
//  Print the information of the selected hit
//=========================================================================
void PrSeedingXLayers::printHit ( const PrHit* hit, std::string title ) {
  info() << "  " << title
         << format( "Plane%3d zone%2d z0 %8.2f x0 %8.2f  size%2d charge%3d coord %8.3f used%2d ",
                    hit->planeCode(), hit->zone(), hit->z(), hit->x(),
                    hit->size(), hit->charge(), hit->coord(), hit->isUsed() );
  if ( m_debugTool ) m_debugTool->printKey( info(), hit->id() );
  if ( matchKey( hit ) ) info() << " ***";
  info() << endmsg;
}

//=========================================================================
//  Print the whole track
//=========================================================================
void PrSeedingXLayers::printTrack ( PrSeedTrack& track ) {
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
    info() << format( "dist %7.3f dy %7.2f chi2 %7.2f ", track.distance( *itH ), track.deltaY( *itH ), track.chi2( *itH ) );
    printHit( *itH );
  }
}
//=============================================================================
