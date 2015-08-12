// Include files 

// from Gaudi
#include "Event/Track.h"
#include "Event/StateParameters.h"
// local
#include "PrSeedingAlgorithm.h"
#include "PrSeedQuadruplet.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrSeedingAlgorithm
//
// 2012-06-18 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrSeedingAlgorithm )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PrSeedingAlgorithm::PrSeedingAlgorithm( const std::string& name,
                                          ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator ),
  m_hitManager(NULL),
  m_geoTool(NULL),
  m_debugTool(NULL)
{
  declareProperty( "InputName",           m_inputName            = LHCb::TrackLocation::Forward );
  declareProperty( "OutputName",          m_outputName           = LHCb::TrackLocation::Seed    );
  declareProperty( "HitManagerName",      m_hitManagerName       = "PrFTHitManager"  );
  declareProperty( "TolXQuad",            m_tolXQuad             =   30. * Gaudi::Units::mm );
  declareProperty( "TolVMatch",           m_tolVMatch            =    1. * Gaudi::Units::mm );
  declareProperty( "MaxChi2InTrack",      m_maxChi2InTrack       = 9.                );
  declareProperty( "DeltaSlopeQuadMatch", m_deltaSlopeQuadMatch  = 0.015           );
  declareProperty( "DeltaXQuadMatch",     m_deltaXQuadMatch      = 350.            );
  declareProperty( "DeltaArrowQuadMatch", m_deltaArrowQuadMatch  = 20.             );
  declareProperty( "MaxChi2Triplet",      m_maxChi2Triplet       = 1000.           );

  // Parameters for debugging
  declareProperty( "DebugToolName"     , m_debugToolName  = ""        );
  declareProperty( "WantedKey"         , m_wantedKey      = -100      );
}
//=============================================================================
// Destructor
//=============================================================================
PrSeedingAlgorithm::~PrSeedingAlgorithm() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrSeedingAlgorithm::initialize() {
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

  m_quadruplets.resize( 3 );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrSeedingAlgorithm::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

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
          int zoneNb = 2 * ftId.layer() + ftId.mat();
          PrHitZone* zone = m_hitManager->zone(zoneNb);
          for ( PrHits::iterator itH = zone->hits().begin(); zone->hits().end() != itH; ++itH ) {
            if ( (*itH)->id() == *itId ) (*itH)->setUsed( true );
          }
          ids.push_back( *itId );
        }
      }
      LHCb::Track* seed = new LHCb::Track;
      seed->setLhcbIDs( ids );
      seed->setType( LHCb::Track::Ttrack );
      seed->setHistory( LHCb::Track::PrSeeding );
      seed->setPatRecStatus( LHCb::Track::PatRecIDs );
      seed->addToStates( (*itT)->closestState( 9000. ) );
      result->insert( seed );
    }
  }
  m_trackCandidates.clear();


  for ( unsigned int zone = 0; 2 > zone; ++zone ) {

    findQuadruplets( zone );

    tracksFromTriplets( zone );

    tracksFromPairs( zone );
  }

  makeLHCbTracks( result );

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrSeedingAlgorithm::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//
//=========================================================================
void PrSeedingAlgorithm::findQuadruplets( int zone ) {
  for ( int station = 0 ; 3 > station ; ++station ) {
    m_quadruplets[station].clear();
    int firstZone = 8*station + zone;
    PrHitZone* x1Zone = m_hitManager->zone( firstZone     );
    PrHitZone* uZone  = m_hitManager->zone( firstZone + 2 );
    PrHitZone* vZone  = m_hitManager->zone( firstZone + 4 );
    PrHitZone* x2Zone = m_hitManager->zone( firstZone + 6 );
    PrHits& hitsX1 = x1Zone->hits();
    PrHits& hitsU  = uZone->hits();
    PrHits& hitsV  = vZone->hits();
    PrHits& hitsX2 = x2Zone->hits();
    float zx1 = x1Zone->z();
    float zu  = uZone->z();
    float zv  = vZone->z();
    float zx2 = x2Zone->z();
    float dzx = zx2-zx1;
    float zRatio = zx2 / zx1;

    //== Compute the minimal deltax for a stereo hit
    float minU = uZone->dxOnAFibre();
    float minV = vZone->dxOnAFibre();
    float tolU = fabs( minU );

    if ( 1 == zone ) {
      minU = -minU;
      minV = -minV;
    }
    if ( 0 < minU ) minU = 0;
    if ( 0 < minV ) minV = 0;

    PrHits::iterator itX1 = hitsX1.begin();
    PrHits::iterator itU  = hitsU.begin();
    PrHits::iterator itV  = hitsV.begin();
    PrHits::iterator itX2 = hitsX2.begin();
    while( itX1 != hitsX1.end() ) {
      if ( (*itX1)->isUsed() ) { ++itX1; continue; }
      float x1     = (*itX1)->x();
      float x2Pred = x1 * zRatio;

      while ( itX2 != hitsX2.end() && (*itX2)->x() < x2Pred - m_tolXQuad ) ++itX2;

      PrHits::iterator it2 = itX2;
      while ( it2 != hitsX2.end() && fabs( (*it2)->x() - x2Pred ) < m_tolXQuad ) {
        if ( (*it2)->isUsed() ) { ++it2; continue; }

        //== We have a pair. Try the u/v
        bool debug = false;
        if ( matchKey( *itX1 ) && matchKey( *it2 ) ) {
          info() << "Try X pair : " << endmsg;
          printHit( *itX1, "   X1" );
          printHit( *it2,  "   X2" );
          debug = true;
        }
        float dx = (*it2)->x() - x1;
        float xAtU = x1 + dx * (zu-zx1)/dzx;
        float uMin = xAtU + minU;
        for ( PrHits::iterator iu = itU; hitsU.end() != iu; ++iu ) {
          if ( (*iu)->x() < x1 + minU - m_tolXQuad ) itU = iu;
          if ( (*iu)->x() < uMin ) continue;
          if ( (*iu)->x() > uMin + tolU ) break;
          if ( (*iu)->isUsed() ) continue;
          float vPred = x1 + dx * (zv-zx1) / dzx + (xAtU - (*iu)->x()) * zv/zu;
          if ( debug ) {
            if ( matchKey( *iu ) ) {
              info() << format( "vPred %8.2f ", vPred );
              printHit( *iu );
            }
          }
          for ( PrHits::iterator iv = itV; hitsV.end() != iv; ++iv ) {
            if ( (*iv)->x() < x1 + minV - m_tolXQuad ) itV = iv;
            if ( (*iv)->x() - vPred < -m_tolVMatch ) continue;
            if ( (*iv)->x() - vPred >  m_tolVMatch ) break;
            if ( (*iv)->isUsed() ) continue;
            PrSeedQuadruplet cand( *itX1, *iu, *iv, *it2 );
            if ( debug ) {
              info() << " --- UV pair d = " << (*iv)->x() - vPred
                     << " x0 " << cand.x0() << " y0 " << cand.y0()
                     << " z0 " << cand.z0() << " tx " << cand.tx() << endmsg;
              printHit( *iu, "   U ");
              printHit( *iv, "   V ");
            }
            m_quadruplets[station].push_back( cand );
          }
        }
        ++it2;
        if ( it2 == hitsX2.end() ) break;
      }
      ++itX1;
    }
  }
}

//=========================================================================
//  Make a track from 3 quadruplets, one on each station
//=========================================================================
void PrSeedingAlgorithm::tracksFromTriplets ( int zone) {
  std::sort( m_quadruplets[0].begin(), m_quadruplets[0].end(), PrSeedQuadruplet::LowerByTy() );
  std::sort( m_quadruplets[1].begin(), m_quadruplets[1].end(), PrSeedQuadruplet::LowerByTy() );
  std::sort( m_quadruplets[2].begin(), m_quadruplets[2].end(), PrSeedQuadruplet::LowerByTy() );
  PrSeedQuadruplets::iterator itQ1Start = m_quadruplets[1].begin();
  PrSeedQuadruplets::iterator itQ2Start = m_quadruplets[2].begin();

  for ( PrSeedQuadruplets::iterator itQ0 = m_quadruplets[0].begin(); m_quadruplets[0].end() != itQ0; ++itQ0 ) {
    float ty0 = (*itQ0).ty();
    float tx0 = (*itQ0).tx();
    float x00 = (*itQ0).x0();
    for ( PrSeedQuadruplets::iterator itQ1 = itQ1Start; m_quadruplets[1].end() != itQ1; ++itQ1 ) {
      float ty1 = (*itQ1).ty();
      if ( ty1 < ty0 - m_deltaSlopeQuadMatch ) {
        ++itQ1Start;
        continue;
      }
      bool debug = false;
      if ( ty1 > ty0 + m_deltaSlopeQuadMatch ) break;
      float tx1 = (*itQ1).tx();
      if ( fabs( tx1 - tx0 ) > m_deltaSlopeQuadMatch ) continue;
      float x01 = (*itQ1).x0();
      if ( fabs( x01 - x00 ) > m_deltaXQuadMatch ) continue;
      
      for ( PrSeedQuadruplets::iterator itQ2 = itQ2Start; m_quadruplets[2].end() != itQ2; ++itQ2 ) {
        float ty2 = (*itQ2).ty();
        if ( ty2 < ty0 - m_deltaSlopeQuadMatch ) { ++itQ2Start; continue; }
        if ( ty2 > ty0 + m_deltaSlopeQuadMatch ) break;
        float tx2 = (*itQ2).tx();
        if ( fabs( tx2 - tx1 ) > m_deltaSlopeQuadMatch ) continue;
        float x02 = (*itQ2).x0();
        if ( fabs( x02 - x01 ) > m_deltaXQuadMatch ) continue;
        if ( fabs( x00 + x02 - 2 * x01 ) > m_deltaArrowQuadMatch ) continue;  // consistent slopes
        if ( fabs( ( x00 - x02 ) / ( (*itQ0).z0()-(*itQ2).z0() ) - tx1 ) > m_deltaSlopeQuadMatch ) continue;

        PrSeedTrack temp( zone, m_geoTool->zReference(), (*itQ0).hits() );
        temp.addHits( (*itQ1).hits() );
        temp.addHits( (*itQ2).hits() );
        float xSl = tx1;
        float xTr = x01 + tx1 * (  m_geoTool->zReference() - (*itQ1).z0() );
        float ySl = ( (*itQ2).y0() - (*itQ0).y0() ) / ( (*itQ2).z0() - (*itQ0).z0() );
        float yTr = (*itQ1).y0() + ySl * (  m_geoTool->zReference() - (*itQ1).z0() );
        temp.setParameters( xTr, xSl, 0., yTr, ySl );
        if ( debug ) printTrack( temp );
        
        if ( fitTrack( temp ) ) {
          (*itQ0).setUsed( true );
          (*itQ1).setUsed( true );
          (*itQ2).setUsed( true );
          m_trackCandidates.push_back( temp );
          if ( matchKey( temp ) ) {
            info() << "=== Good candidate ===" << endmsg;
            printTrack( temp );
          }
        } else {
          if ( matchKey( *itQ0 ) && matchKey( *itQ1 ) && matchKey( *itQ2 ) ) {
            info() << "Fit failed" << endmsg;
            printTrack( temp );
          }
        }
      }
    }
  }
  //====================================================================
  // Remove clones, i.e. share 10/12 hits....
  //====================================================================
  for ( PrSeedTracks::iterator itT1 = m_trackCandidates.begin(); m_trackCandidates.end() !=itT1; ++itT1 ) {
    if ( !(*itT1).valid() ) continue;
    for ( PrSeedTracks::iterator itT2 = itT1 + 1; m_trackCandidates.end() !=itT2; ++itT2 ) {
      if ( !(*itT2).valid() ) continue;
      int nCommon = 0;
      for ( int j = 0; 12 > j ; ++j ) {
        if ( (*itT1).hits()[j] == (*itT2).hits()[j] ) nCommon++;
      }
      if ( nCommon > 9 ) {
        if ( (*itT1).chi2PerDoF() < (*itT2).chi2PerDoF() ) {
          (*itT2).setValid( false );
        } else {
          (*itT1).setValid( false );
        }
      }
    }
  }
  // Tag all hits from used quadruplets, and then quadruplets with all used hits

  for ( unsigned int station = 0; 3 > station; ++station ) {
    for ( PrSeedQuadruplets::iterator itQ = m_quadruplets[station].begin();
          m_quadruplets[station].end() != itQ; ++itQ ) {
      if ( (*itQ).isUsed()  ) {
        for ( PrHits::iterator itH = (*itQ).hits().begin(); (*itQ).hits().end() != itH; ++itH ) {
          (*itH)->setUsed( true );
        }
      }
    }

    for ( PrSeedQuadruplets::iterator itQ = m_quadruplets[station].begin();
          m_quadruplets[station].end() != itQ; ++itQ ) {
      if ( (*itQ).isUsed() ) continue;
      int nbUsed = 0;
      for ( PrHits::iterator itH = (*itQ).hits().begin(); (*itQ).hits().end() != itH; ++itH ) {
        if ( (*itH)->isUsed( ) ) nbUsed++;
      }
      if ( 2 < nbUsed ) (*itQ).setUsed( true );
    }
  }
}

//=========================================================================
//  Make tracks from a pair of quadruplets, and some other layers
//=========================================================================
void PrSeedingAlgorithm::tracksFromPairs ( int zoneNumber ) {
  for ( unsigned int first = 0 ; 2 > first; ++first ) {
    for ( unsigned int second = first + 1; 3 > second; ++second ) {
      unsigned missedStation = 3 - first - second;
      PrSeedQuadruplets::iterator itSecondStart = m_quadruplets[second].begin();
      for ( PrSeedQuadruplets::iterator itF = m_quadruplets[first].begin(); m_quadruplets[first].end() != itF; ++itF ) {
        if ( (*itF).isUsed() ) continue;
        float txf = (*itF).tx();
        float tyf = (*itF).ty();
        float x0f = (*itF).x0();
        for ( PrSeedQuadruplets::iterator itS = itSecondStart; m_quadruplets[second].end() != itS; ++itS ) {
          if ( (*itS).isUsed() ) continue;
          float tys = (*itS).ty();
          float x0s = (*itS).x0();
          if ( tys < tyf - m_deltaSlopeQuadMatch ) { ++itSecondStart; continue; }
          if ( tys > tyf + m_deltaSlopeQuadMatch ) break;
          float txs = (*itS).tx();
          if ( fabs( txf - txs ) > m_deltaSlopeQuadMatch ) continue;
          if ( fabs( x0f - x0s ) > m_deltaXQuadMatch ) continue;

          PrSeedTrack temp( zoneNumber, m_geoTool->zReference(), (*itF).hits() );
          temp.addHits( (*itS).hits() );

          float xSlope = (x0s - x0f) / ( (*itS).z0() - (*itF).z0() );
          float xTr = x0s + xSlope * ( m_geoTool->zReference() - (*itS).z0() );
          float yTr = (*itS).y0() + (*itS).ty() * (  m_geoTool->zReference() - (*itS).z0() );
          temp.setParameters( xTr, xSlope, 0., yTr, tys );

          if ( fitTrack( temp ) ) {
            int nMiss = 0;
            if ( matchKey( temp ) ) {
              info() << "=== Good candidate, before adding extra hits ===" << endmsg;
              printTrack( temp );
            }
            //== Get hits on other stations
            for ( unsigned int zoneNb = 8*missedStation+zoneNumber; 8*missedStation+zoneNumber+8 > zoneNb; zoneNb+= 2 ) {
              float minChi2 = m_maxChi2Triplet;
              PrHit* best = NULL;
              for ( PrHits::iterator itH = m_hitManager->zone(zoneNb)->hits().begin();
                    m_hitManager->zone(zoneNb)->hits().end() != itH; ++itH ) {
                float chi2 = temp.chi2( *itH );
                if ( chi2 < minChi2 ) {
                  minChi2 = chi2;
                  best = *itH;
                } else {
                  if ( temp.distance( *itH ) > 0 ) break;
                }
              }
              if ( minChi2 < m_maxChi2Triplet ) {
                temp.addHit( best );
              } else {
                ++nMiss;
                if ( nMiss > 2 ) break;
              }
            }
            if ( matchKey( temp ) ) {
              info() << "   --- after adding hits: nMiss " << nMiss << endmsg;
              printTrack( temp );
            }

            if ( nMiss < 3 ) {
              bool ok = fitTrack( temp );
              while ( !ok && temp.hits().size() > 10 ) {
                ok = removeWorstAndRefit( temp );
                if ( matchKey( temp ) ) {
                  info() << "  ... after remove worst===" << endmsg;
                  printTrack( temp );
                }
              }
              if ( ok ) {
                m_trackCandidates.push_back( temp );
                (*itF).setUsed( true );
                (*itS).setUsed( true );
                if ( matchKey( temp ) ) {
                  info() << "=== Store accepted track ===" << endmsg;
                  printTrack( temp );
                }
              } else {
                if ( matchKey( temp ) ) info() << "   --- not enough good hits in the last station ---" << endmsg;
              }
            } else {
              if ( matchKey( temp ) ) info() << "   --- not enough hits in the last station ---" << endmsg;
            }
          }
        }
      }
    }
  }
}
//=========================================================================
//  Fit the track, return OK if fit sucecssfull
//=========================================================================
bool PrSeedingAlgorithm::fitTrack( PrSeedTrack& track ) {

  for ( int loop = 0; 2 > loop ; ++loop ) {
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
bool PrSeedingAlgorithm::removeWorstAndRefit ( PrSeedTrack& track ) {
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
void PrSeedingAlgorithm::setChi2 ( PrSeedTrack& track ) {
  float chi2 = 0.;
  int   nDoF = -5;  // Fitted a parabola + line in Y = 5 degree of freedom
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
    float d = track.distance( *itH );
    float w = (*itH)->w();
    chi2 += w * d * d;
    nDoF += 1;
  }
  track.setChi2( chi2, nDoF );
}

//=========================================================================
//  Convert to LHCb tracks
//=========================================================================
void PrSeedingAlgorithm::makeLHCbTracks ( LHCb::Tracks* result ) {
  for ( PrSeedTracks::iterator itT = m_trackCandidates.begin();
        m_trackCandidates.end() != itT; ++itT ) {
    if ( !(*itT).valid() ) continue;
    LHCb::Track* tmp = new LHCb::Track;
    tmp->setType( LHCb::Track::Ttrack );
    tmp->setHistory( LHCb::Track::PrSeeding );
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
void PrSeedingAlgorithm::printHit ( const PrHit* hit, std::string title ) {
  info() << "  " << title
         << format( "Plane%3d zone%2d z0 %8.2f x0 %8.2f  size%2d charge%3d used%2d ",
                    hit->planeCode(), hit->zone(), hit->z(), hit->x(),
                    hit->size(), hit->charge(), hit->isUsed() );
  if ( m_debugTool ) m_debugTool->printKey( info(), hit->id() );
  if ( matchKey( hit ) ) info() << " ***";
  info() << endmsg;
}

//=========================================================================
//  Print the whole track
//=========================================================================
void PrSeedingAlgorithm::printTrack ( PrSeedTrack& track ) {
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
    info() << format( "dist %7.3f dy %7.2f chi2 %7.2f ", track.distance( *itH ), track.deltaY( *itH ), track.chi2( *itH ) );
    printHit( *itH );
  }
}
//=============================================================================
