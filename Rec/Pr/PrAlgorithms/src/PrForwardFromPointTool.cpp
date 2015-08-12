// Include files 

// from Gaudi
#include "Event/StateParameters.h"
// local
#include "PrForwardFromPointTool.h"
#include "PrForwardTrack.h"
#include "PrPlaneCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrForwardFromPointTool
//
// 2012-03-20 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PrForwardFromPointTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PrForwardFromPointTool::PrForwardFromPointTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
    : IPrForwardTool( type, name , parent )
{
  declareInterface<PrForwardFromPointTool>(this);
  declareProperty( "HitManagerName",         m_hitManagerName         = "PrFTHitManager"        );
  declareProperty( "MinXHits",               m_minXHits               =   5                     );
  declareProperty( "MaxChi2XProjection",     m_maxChi2XProjection     =   9.                    );
  declareProperty( "MaxChi2ToAddExtraXHits", m_maxChi2ToAddExtraXHits = 500.                    );
  declareProperty( "MinStereoHits",          m_minStereoHits          =   5                     );
  declareProperty( "DeltaMeanDy",            m_deltaMeanDy            =   5.* Gaudi::Units::mm  );
  declareProperty( "MaxChi2Stereo",          m_maxChi2Stereo          =  16.                    );

  declareProperty( "TolXSeed",               m_tolXSeed               =   1.* Gaudi::Units::mm  );
  declareProperty( "TolXSeedSlope",          m_tolXSeedSlope          =   0.002                 );
  declareProperty( "TolUMatch",              m_tolUMatch              =   3.* Gaudi::Units::mm  );
  declareProperty( "TolUMatchSlopeX",        m_tolUMatchSlopeX        = 0.001 );
  declareProperty( "TolUMatchSlopeY",        m_tolUMatchSlopeY        = 0.05  );
  declareProperty( "TolVMatch",              m_tolVMatch              =   1.* Gaudi::Units::mm  );
  declareProperty( "MaxChi2Seed",            m_maxChi2Seed            = 10. );
  declareProperty( "MaxDistanceToAdd",       m_maxDistanceToAdd       =   3.* Gaudi::Units::mm );
  declareProperty( "MaxChi2Extra",           m_maxChi2Extra           = 100. );
}
//=============================================================================
// Destructor
//=============================================================================
PrForwardFromPointTool::~PrForwardFromPointTool() {}


//=========================================================================
//  initialize
//=========================================================================
StatusCode PrForwardFromPointTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize();
  if ( !sc ) return sc;

  m_geoTool = tool<PrGeometryTool>("PrGeometryTool");
  m_hitManager = tool<PrHitManager>( m_hitManagerName );
  return StatusCode::SUCCESS;
}
//=========================================================================
//  Main method: Process a track
//=========================================================================
void PrForwardFromPointTool::extendTrack ( LHCb::Track* velo, LHCb::Tracks* result ) {

  m_debug = (velo->key() == m_veloKey) || msgLevel( MSG::DEBUG );

  PrForwardTrack track( velo, m_geoTool->zReference() );

  for ( unsigned int zone = 0 ; m_hitManager->nbZones() > zone; ++zone ) {
    for ( PrHits::iterator itH = m_hitManager->hits(zone).begin(); m_hitManager->hits(zone).end() != itH; ++itH ) {
      (*itH)->setUsed( false );
    }
  }
  PrForwardTracks all;

  for ( unsigned int side = 0 ; 2 > side ; ++side ) {
    for ( int seedStation = 2 ;  -1 < seedStation  ; seedStation -= 2 ) {
      m_trackCandidates.clear();
      findSeedsInStation( track, side, seedStation );
      if ( m_debug ) {
        info() << "** Found " << m_trackCandidates.size() << " seeds in T3" << endmsg;
        for ( PrForwardTracks::const_iterator itT = m_trackCandidates.begin();
              m_trackCandidates.end() != itT; ++itT ) {
          printTrack( *itT );
        }
      }

      //== Extend tracks
      
      extendTracks( seedStation, side );
      
      for ( PrForwardTracks::iterator itT = m_trackCandidates.begin();
            m_trackCandidates.end() != itT; ++itT ) {
        if ( (*itT).valid() && (*itT).hits().size() > 9 ) {
          all.push_back( *itT );
          for ( PrHits::iterator itH = (*itT).hits().begin(); (*itT).hits().end() != itH; ++itH ) {
            (*itH)->setUsed( true );
          }
        }
      }
    }
  }

  m_trackCandidates.clear();

  //== Cleanup of multiple solutions...
  PrForwardTracks::iterator itT;
  
  if ( all.size() > 1 ) {
    if ( m_debug ) {
      info() << "********** list of candidates before cleanup ********" << endmsg;
      for ( itT = all.begin(); all.end() != itT; ++itT ) {
        if ( (*itT).valid() ) printTrack( *itT );
      }
    }

    //== Remove clones...
    for ( PrForwardTracks::iterator itT1 = all.begin(); all.end() != itT1+1; ++itT1 ) {
      if ( !(*itT1).valid() ) continue;
      for ( PrForwardTracks::iterator itT2 = itT1+1; all.end() != itT2; ++itT2 ) {
        if ( !(*itT2).valid() ) continue;
        int nCommon = 0;
        PrHits::iterator it1 = (*itT1).hits().begin();
        PrHits::iterator it2 = (*itT2).hits().begin();
        while ( it1 != (*itT1).hits().end() &&
                it2 != (*itT2).hits().end() ) {
          if ( *it1 == *it2 ) ++nCommon;
          if ( (*it1)->planeCode() < (*it2)->planeCode() ) {
            ++it1;
          } else if ( (*it1)->planeCode() > (*it2)->planeCode() ) {
            ++it2;
          } else {
            ++it1;
            ++it2;
          }
        }
        if ( nCommon > 5 ) {
          if ( (*itT1).chi2() > (*itT2).chi2() ) {
            (*itT1).setValid( false );
            break;
          } else {
            (*itT2).setValid( false );
          }
        }        
      }
    }



    unsigned int maxSize = 0;
    for ( itT = all.begin(); all.end() != itT; ++itT ) {
      if ( !(*itT).valid() ) continue;
      if ( (*itT).hits().size() > maxSize ) maxSize = (*itT).hits().size();
    }
    maxSize -= 1;
    float minDXCoord = 1.e9;
    for ( itT = all.begin(); all.end() != itT; ++itT ) {
      if ( maxSize > (*itT).hits().size() ) (*itT).setValid( false );
      if ( (*itT).valid() ) {
        if ( (*itT).dXCoord() < minDXCoord ) minDXCoord = (*itT).dXCoord();
      }
    }
    minDXCoord += 1.;
    float maxDY = 1.e9;
    for ( itT = all.begin(); all.end() != itT; ++itT ) {
      if ( (*itT).dXCoord() > minDXCoord ) (*itT).setValid( false );
      if ( (*itT).valid() ) {
        if ( fabs( (*itT).meanDy() ) < maxDY ) maxDY = fabs( (*itT).meanDy() );
      }
    }
    maxDY += m_deltaMeanDy;
    for ( itT = all.begin(); all.end() != itT; ++itT ) {
      if ( fabs( (*itT).meanDy() ) > maxDY ) (*itT).setValid( false );
    }
  }

  for ( itT = all.begin(); all.end() != itT; ++itT ) {
    if ( (*itT).valid() ) m_trackCandidates.push_back( *itT );
  }

  if ( m_debug ) {
    info() << "********** final list of candidates ********" << endmsg;
    for ( itT = m_trackCandidates.begin(); m_trackCandidates.end() != itT; ++itT ) {
      printTrack( *itT );
    }
  }

  //=== Finalisation: Produce LHCb tracks

  makeLHCbTracks( result );
}

//=========================================================================
//  Find a seed, i.e. 4 layers in the same station
//=========================================================================
void PrForwardFromPointTool::findSeedsInStation ( PrForwardTrack& track, unsigned int side, int station ) {
  float zMagnet = m_geoTool->zMagnet( track );

  int firstZone = 8*station + side;
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

  float xAtX1 = track.xFromVelo( zx1 );
  float yAtX1 = track.yFromVelo( zx1 );
  if ( !x1Zone->isInside( xAtX1, yAtX1 ) ) return;
  float xAtX2  = track.xFromVelo( zx2 );

  float zRatio = (zx2-zMagnet) / (zx1-zMagnet);

  float deltaAtU = -uZone->dxDy() * track.yFromVelo( zu );
  float deltaAtV = -vZone->dxDy() * track.yFromVelo( zv );

  PrHits::iterator itX1 = hitsX1.begin();
  PrHits::iterator itU  = hitsU.begin();
  PrHits::iterator itV  = hitsV.begin();
  PrHits::iterator itX2 = hitsX2.begin();
  while( itX1 != hitsX1.end() ) {
    if ( (*itX1)->isUsed() ) { ++itX1; continue; }
    float x1     = (*itX1)->x();
    float x2Pred = xAtX2 + ( x1-xAtX1) * zRatio;

    float tolXMatch = m_tolXSeed + fabs(x1-xAtX1) * m_tolXSeedSlope;

    while ( itX2 != hitsX2.end() && (*itX2)->x() < x2Pred - tolXMatch ) ++itX2;

    PrHits::iterator it2 = itX2;
    while ( it2 != hitsX2.end() && fabs( (*it2)->x() - x2Pred ) < tolXMatch ) {
      if ( (*it2)->isUsed() ) { ++it2; continue; }

      //== We have a pair. Try the u/v
      bool fullDebug = false;
      if ( m_debug ) {
        if ( matchKey( *itX1 ) && matchKey( *it2 ) ) {
          fullDebug = true;
          info() << "Try X pair : " << endmsg;
          printHit( *itX1, "   X1" );
          printHit( *it2,  "   X2" );
        }
      }
      float dx = (*it2)->x() - x1;
      float xAtU = x1 + dx * (zu-zx1)/(zx2-zx1) + deltaAtU;
      float tolU = m_tolUMatch + m_tolUMatchSlopeX * fabs( x1-xAtX1 ) + m_tolUMatchSlopeY * fabs(deltaAtU);

      if ( fullDebug ) info() << " -- predicted u : " << xAtU << " tolU " << tolU
                              << " first " << (*itU)->x() << endmsg;
      for ( PrHits::iterator iu = itU; hitsU.end() != iu; ++iu ) {
        if ( (*iu)->x() < xAtU - tolU ) {
          itU = iu;
          continue;
        }
        if ( (*iu)->x() > xAtU + tolU ) break;
        if ( (*iu)->isUsed() ) continue;
        float vPred = x1 + dx * (zv-zx1) / (zx2-zx1) + deltaAtV + ( xAtU - (*iu)->x() );

        if ( fullDebug ) {
          if ( matchKey( *iu ) ) {
            info() << format( "vPred %8.2f first %8.2f ", vPred, (*itV)->x() );
            printHit( *iu );
          }
        }
        while ( (*itV)->x() > vPred - m_tolVMatch ) {
          if ( itV == hitsV.begin() ) break;
          --itV;
        }

        for ( PrHits::iterator iv = itV; hitsV.end() != iv; ++iv ) {
          if ( (*iv)->x() - vPred < -m_tolVMatch ) {
            itV = iv;
            continue;
          }
          if ( (*iv)->x() - vPred >  m_tolVMatch ) break;
          if ( (*iv)->isUsed() ) continue;

          PrHits seed;
          seed.push_back( *itX1 );
          seed.push_back( *iu   );
          seed.push_back( *iv   );
          seed.push_back( *it2  );
          PrForwardTrack tmp( track.track(), m_geoTool->zReference(), seed );
          m_geoTool->setTrackParameters( tmp );
          fastLinearFit( tmp );
          fastLinearFit( tmp );
          setChi2( tmp );
          if ( tmp.chi2() > m_maxChi2Seed ) {
            if ( m_debug ) info() << "Chi2 too high: " << tmp.chi2() << endmsg;
            continue;
          }
          m_trackCandidates.push_back( tmp );
        }
      }
      ++it2;
      if ( it2 == hitsX2.end() ) break;
    }
    ++itX1;
  }
}

//=========================================================================
//  Extend the candidates up or downstream
//=========================================================================
void PrForwardFromPointTool::extendTracks ( int seedStation, unsigned int side ) {
  int first = 7;
  int last  = -1;
  int step  = -1;
  if ( 0 == seedStation ) {
    first = 4;
    last  = 12;
    step = 1;
  }

  for ( PrForwardTracks::iterator itT = m_trackCandidates.begin(); m_trackCandidates.end() != itT; ++itT ) {
    if ( (*itT).zone() != side ) continue;
    int layer = first;
    int nbMiss = 0;
    while( layer != last ) {
      PrHitZone* zone = m_hitManager->zone( 2*layer + side );
      PrHit* best = 0;
      float minChi2 = 1.e9;
      for ( PrHits::iterator itH = zone->hits().begin(); zone->hits().end() != itH; ++itH ) {
        float d = (*itT).distance( *itH );
        if ( d < -m_maxDistanceToAdd ) continue;
        if ( d >  m_maxDistanceToAdd ) break;
        float chi2 = d * d * (*itH)->w();
        if ( chi2 < minChi2 ) {
          minChi2 = chi2;
          best = *itH;
        }
      }
      if ( 0 == best ) {
        ++nbMiss;
        if ( 2 < nbMiss ) {
          if ( m_debug ) info() << "=== Too many missed layers" << endmsg;
          (*itT).setValid( false );
          break;
        }
      } else {
        if ( m_debug ) {
          info() << format( " chi2 %7.2f ", minChi2 );
          printHit( best );
        }
        m_geoTool->xAtReferencePlane( *itT, best );
        (*itT).addHit( best );
        fastLinearFit( *itT );
        if ( (*itT).hits().size() > 6 ) {
          fitStereoHits( *itT );
          fitXProjection( *itT );
        }
        setChi2( *itT );
        if ( (*itT).chi2PerDoF() > m_maxChi2Extra ) {
          if ( m_debug ) info() << "=== Chi2 too high : " << (*itT).chi2PerDoF()<< endmsg;
          (*itT).setValid( false );
          break;
        }
      }
      layer += step;
    }
    //== Try to add hits on missed layers
    std::sort( (*itT).hits().begin(), (*itT).hits().end(), PrHit::LowerByZ() );
    if ( (*itT).valid() && (*itT).hits().size() != 12 ) {
      if ( m_debug ) info() << "Try to add hits on missing layers" << endmsg;
      PrHits added;
      PrHits::iterator itH = (*itT).hits().begin();
      for ( int lay = 0 ; 12 > lay ; ++lay ) {
        if ( (*itH)->planeCode() == lay ) {
          itH++;
          if ( itH == (*itT).hits().end() ) --itH;
        } else {
          if ( m_debug ) info() << "Try on layer " << lay << endmsg;
          
          PrHits& zoneHits = m_hitManager->hits( 2*lay+side );
          PrHit* best = NULL;
          float bestChi2 = m_maxChi2Extra;
          for ( PrHits::iterator itAdd = zoneHits.begin(); zoneHits.end() != itAdd ; ++itAdd ) {
            float d = (*itT).distance( *itAdd );
            if ( d < -m_maxDistanceToAdd ) continue;
            if ( d >  m_maxDistanceToAdd ) break;
            if ( (*itT).chi2( *itAdd ) < bestChi2 ) {
              bestChi2 = (*itT).chi2( *itAdd );
              best = *itAdd;
            }
          }
          if ( NULL != best ) {
            if ( m_debug ) printHit( best, "Add " );
            added.push_back( best );
          }
        }
      }
      if ( added.size() > 0 ) {
        for ( PrHits::iterator itH = added.begin(); added.end() != itH; ++itH ) {
          m_geoTool->xAtReferencePlane( *itT, *itH );
          (*itT).addHit( *itH );
        }
        std::sort( (*itT).hits().begin(), (*itT).hits().end(), PrHit::LowerByZ() );
        fitStereoHits( *itT );
        fitXProjection( *itT );
        setChi2( *itT );
      }
    }

    if ( (*itT).valid() ) {
      float dy = (*itT).y( m_geoTool->zReference() ) - (*itT).yFromVelo( m_geoTool->zReference() );
      (*itT).setMeanDy( dy );
      float minCoord = 1.e9;
      float maxCoord = -1.e9;
      for ( PrHits::iterator itH = (*itT).hits().begin(); (*itT).hits().end() != itH; ++itH ) {
        if ( (*itH)->coord() > maxCoord ) maxCoord = (*itH)->coord();
        if ( (*itH)->coord() < minCoord ) minCoord = (*itH)->coord();
      }
      (*itT).setDXCoord( maxCoord - minCoord );
    }
  }
}
//=========================================================================
//  Fit the X projection of a track, return OK if fit sucecssfull
//=========================================================================
bool PrForwardFromPointTool::fitXProjection ( PrForwardTrack& track ) {
  if ( m_debug ) {
    info() << "  -- Entering fitXProjection with:" << endmsg;
    printTrack( track );
  }
  if ( track.hits().size() < m_minXHits ) {
    if ( m_debug ) info() << "  == Not enough layers with hits" << endmsg;
    return false;
  }
  float maxChi2 = 1.e9;
  while ( maxChi2 > m_maxChi2XProjection ) {
    //== Fit a cubic
    float s0   = 0.;
    float sz   = 0.;
    float sz2  = 0.;
    float sz3  = 0.;
    float sz4  = 0.;
    float sd   = 0.;
    float sdz  = 0.;
    float sdz2 = 0.;
    for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
      if ( (*itH)->isUsed() ) continue;
      float d = track.distance( *itH );
      float w = (*itH)->w();
      float z = .001 * ( (*itH)->z() - m_geoTool->zReference() );
      s0   += w;
      sz   += w * z;
      sz2  += w * z * z;
      sz3  += w * z * z * z;
      sz4  += w * z * z * z * z;
      sd   += w * d;
      sdz  += w * d * z;
      sdz2 += w * d * z * z;
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
    float da  = ( sd - db * sz - dc * sz2) / s0;
    
    db = 1.e-3 * db;
    dc = 1.e-6 * dc;
    track.updateParameters( da, db, dc );

    PrHits::iterator worst = track.hits().end();
    maxChi2 = 0.;
    for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
      float chi2 = track.chi2( *itH );
      if ( chi2 > maxChi2 ) {
        maxChi2 = chi2;
        worst   = itH;
      }
    }
    if ( m_debug ) {
      setChi2( track );
      info() << "  -- In fitXProjection, maxChi2 = " << maxChi2 << endmsg;
      printTrack( track );
    }

    if ( maxChi2 > m_maxChi2XProjection ) {
      if ( track.hits().size() == m_minXHits ) {
        if ( m_debug ) info() << "  == Not enough layers with hits" << endmsg;
        return false;
      }
      track.hits().erase( worst );
    }
  }
  setChi2( track );
  if ( m_debug ) {
    info() << "  -- End fitXProjection, maxChi2 = " << maxChi2 << endmsg;
    printTrack( track );
  }
  return true;
}

//=========================================================================
//  Fit a linear form, remove the external worst as long as chi2 is big...
//=========================================================================
void PrForwardFromPointTool::fastLinearFit ( PrForwardTrack& track ) {

  //== Fit the X hits only.
  float s0   = 0.;
  float sd   = 0.;
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
    if ( !(*itH)->isX() ) continue;
    float d = track.distance( *itH );
    float w = (*itH)->w();
    s0   += w;
    sd   += w * d;
  }
  float dax1 = sd/s0;
  track.updateParameters( dax1, 0., 0., 0.);

  //== Fit Y hits only;
  s0   = 0.;
  sd   = 0.;
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
    if ( !(*itH)->isX() ) {
      float d = -track.deltaY( *itH );
      float w = (*itH)->w();
      s0   += w;
      sd   += w * d;
    }
  }
  float day = sd/s0;
  track.updateParameters( 0., 0., 0., 0., day );

  s0   = 0.;
  float sz   = 0.;
  float sz2  = 0.;
  sd   = 0.;
  float sdz  = 0.;
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
    float d = track.distance( *itH );
    float w = (*itH)->w();
    float z = (*itH)->z() - m_geoTool->zReference();
    s0   += w;
    sz   += w * z;
    sz2  += w * z * z;
    sd   += w * d;
    sdz  += w * d * z;
  }
  float denX = (sz*sz-s0*sz2);
  float dax  = (sdz * sz - sd * sz2) / denX;
  float dbx  = (sd *  sz - s0 * sdz) / denX;
  track.updateParameters( dax, dbx, 0., 0.);


  if ( m_debug ) {
    info() << format( "-- In fastLinearFit: changed x by %8.3f then y by %8.3f then x %7.3f + slx %8.6f",
                      dax1, day, dax, dbx ) << endmsg;
    printTrack( track );
  }
}
//=========================================================================
//  Fit the stereo hits
//=========================================================================
void PrForwardFromPointTool::fitStereoHits( PrForwardTrack& track ) {

  //== Fit a line
  float w     = 0.01;  // 10 mm constrait for y=0 at z=zMagnet
  float zMag  = m_geoTool->zMagnet( track );
  float dyMag = track.y( zMag ) - track.yFromVelo( zMag );
  zMag -= m_geoTool->zReference();
  float s0  = w;
  float sz  = w * zMag;
  float sz2 = w * zMag * zMag;
  float sd  = w * dyMag;
  float sdz = w * dyMag * zMag;
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
    if ( (*itH)->isX() ) continue;
    float d = -track.deltaY( *itH );
    float w = (*itH)->w();
    float z = (*itH)->z() - m_geoTool->zReference();
    s0   += w;
    sz   += w * z;
    sz2  += w * z * z;
    sd   += w * d;
    sdz  += w * d * z;
  }
  float den = (s0 * sz2 - sz * sz );
  float da  = (sd  * sz2 - sdz * sz ) / den;
  float db  = (sdz * s0  - sd  * sz ) / den;
  track.updateParameters( 0., 0., 0., 0., da, db, 0. );
}

//=========================================================================
//  Compute and store the Chi2 and DoF of the track
//=========================================================================
void PrForwardFromPointTool::setChi2 ( PrForwardTrack& track ) {
  float chi2 = 0.;
  int   nDoF = -2;  // Fitted a line
  bool hasStereo = false;
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
    float d = track.distance( *itH );
    float w = (*itH)->w();
    if ( !(*itH)->isX()) hasStereo = true;
    chi2 += w * d * d;
    nDoF += 1;
  }
  if ( hasStereo ) nDoF -= 1;  // Fitted as a value
  track.setChi2( chi2, nDoF );
}

//=========================================================================
//  Convert the local track to the LHCb representation
//=========================================================================
void PrForwardFromPointTool::makeLHCbTracks ( LHCb::Tracks* result ) {
  for ( PrForwardTracks::iterator itT = m_trackCandidates.begin();
        m_trackCandidates.end() != itT; ++itT ) {
    if ( !(*itT).valid() ) continue;
    LHCb::Track* tmp = (*itT).track()->clone();
    tmp->setType( LHCb::Track::Long );
    tmp->setHistory( LHCb::Track::PrForward );
    tmp->addToAncestors( (*itT).track() );
    double qOverP  = m_geoTool->qOverP( *itT );
    double errQop2 = 0.1 * 0.1 * qOverP * qOverP;

    for ( std::vector<LHCb::State*>::const_iterator iState = tmp->states().begin();
          iState != tmp->states().end() ; ++iState ){
      (*iState)->setQOverP( qOverP );
      (*iState)->setErrQOverP2( errQop2 );
    }

    LHCb::State tState;
    double z = StateParameters::ZEndT;
    tState.setLocation( LHCb::State::AtT );
    tState.setState( (*itT).x( z ), (*itT).y( z ), z, (*itT).xSlope( z ), (*itT).ySlope( z ), qOverP );

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
//=============================================================================
