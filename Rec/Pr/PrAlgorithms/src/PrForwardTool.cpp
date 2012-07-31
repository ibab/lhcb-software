// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Event/StateParameters.h"
// local
#include "PrForwardTool.h"
#include "PrForwardTrack.h"
#include "PrPlaneCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrForwardTool
//
// 2012-03-20 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PrForwardTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PrForwardTool::PrForwardTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
    : IPrForwardTool ( type, name , parent )
{
  declareInterface<PrForwardTool>(this);
  declareProperty( "HitManagerName",         m_hitManagerName         = "PrFTHitManager"         );
  declareProperty( "MinPt",                  m_minPt                  =   50 * Gaudi::Units::MeV );
  declareProperty( "MinXHits",               m_minXHits               =    4                     );
  declareProperty( "TolY",                   m_tolY                   =    5.* Gaudi::Units::mm  );
  declareProperty( "TolYSlope",              m_tolYSlope              =0.002 * Gaudi::Units::mm  );
  declareProperty( "MaxXWindow",             m_maxXWindow             =    2 * Gaudi::Units::mm  );
  declareProperty( "MaxXWindowSlope",        m_maxXWindowSlope        =0.002 * Gaudi::Units::mm  );
  declareProperty( "MaxXGap",                m_maxXGap                =  1.5 * Gaudi::Units::mm  );
  declareProperty( "MaxChi2XProjection",     m_maxChi2XProjection     =    9.                    );
  declareProperty( "MaxCHi2ToAddExtraXHits", m_maxChi2ToAddExtraXHits =  500.                    );
  declareProperty( "MaxChi2PerDoF",          m_maxChi2PerDoF          =    4.                    );
  declareProperty( "MaxDxForY",              m_maxDxForY              =    3.* Gaudi::Units::mm  );
  declareProperty( "MinStereoHits",          m_minStereoHits          =    5                     );
  declareProperty( "MaxChi2Stereo",          m_maxChi2Stereo          =   16.                    );
  declareProperty( "MaxQuality",             m_maxQuality             =   15.                    );
  declareProperty( "DeltaQuality",           m_deltaQuality           =    3.                    );
  declareProperty( "YQualityWeight",         m_yQualityWeight         =    4.                    );
}
//=============================================================================
// Destructor
//=============================================================================
PrForwardTool::~PrForwardTool() {}


//=========================================================================
//  initialize
//=========================================================================
StatusCode PrForwardTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize();
  if ( !sc ) return sc;

  m_geoTool = tool<PrGeometryTool>("PrGeometryTool");
  m_hitManager = tool<PrHitManager>( m_hitManagerName );
  m_allXHits.reserve(5000);
  return StatusCode::SUCCESS;
}
//=========================================================================
//  Main method: Process a track
//=========================================================================
void PrForwardTool::extendTrack ( LHCb::Track* velo, LHCb::Tracks* result ) {

  m_debug = (velo->key() == m_veloKey) || msgLevel( MSG::DEBUG );

  PrForwardTrack track( velo, m_geoTool->zReference() );

  m_trackCandidates.clear();
  m_minXSize = m_minXHits;
  for ( unsigned int side = 0 ; 2 > side ; ++side ) {

    //== Collect all hits and project them
    collectAllXHits( track, side );
    if ( m_debug ) {
      float xWindow = m_maxXWindow + fabs( track.xFromVelo( m_geoTool->zReference() )) * m_maxXWindowSlope;
      info() << "**** Processing Velo track " << velo->key() << " zone " << side
             << " Selected " << m_allXHits.size() << " hits, window size " << xWindow << endmsg;
      for ( PrHits::const_iterator itH = m_allXHits.begin(); m_allXHits.end() != itH; ++itH ) {
        if ( matchKey( *itH ) ) printHit( *itH, " " );
      }
    }

    //== Select groups and make temporary tracks
    selectXCandidates( track );
  }

  if ( m_debug ) {
    info() << "Selected " << m_trackCandidates.size() << " candidates." << endmsg;
    for ( PrForwardTracks::const_iterator itT = m_trackCandidates.begin();
          m_trackCandidates.end() != itT; ++itT ) {
      printTrack( *itT );
    }
  }

  int nbOK = 0;

  for ( PrForwardTracks::iterator itT = m_trackCandidates.begin();
        m_trackCandidates.end() != itT; ++itT ) {
    collectStereoHits( *itT );
    if ( m_debug ) {
      info() << "**** Stereo hits for track " << itT - m_trackCandidates.begin() << " ****" << endmsg;
      for ( PrHits::const_iterator itH = m_stereoHits.begin(); m_stereoHits.end() != itH; ++itH ) {
        printHit( *itH, " " );
      }
    }
    (*itT).setValid( false );
    PrPlaneCounter pc;
    pc.set( m_stereoHits.begin(), m_stereoHits.end() );
    if ( pc.nbDifferent() + (*itT).hits().size() < m_minStereoHits + m_minXHits ) {
      if ( m_debug ) info() << "Not enough different stereo layers: " << pc.nbDifferent()
                            << " for " << m_minStereoHits << endmsg;
      continue;
    }

    if ( !fitStereoHits( *itT ) ) continue;

    (*itT).setValid( true );
    (*itT).addHits( m_stereoHits );
    (*itT).setHitsUnused();
    fitXProjection ( *itT );  //== Fit both x and stereo

    if ( (*itT).hits().size() < 10 ) {
      if ( m_debug ) {
        info() <<"*** rejected: no eneough hits in total ***" << endmsg;
        printTrack( *itT );
      }
      (*itT).setValid( false );
    } else {

      //== Compute a quality factor for later filtering

      float quality = (*itT).dXCoord() + m_yQualityWeight * (*itT).meanDy();
      if ( (*itT).hits().size() < 12 ) quality += (12-(*itT).hits().size());
      quality /= ( 1. + fabs( track.xFromVelo(m_geoTool->zReference() )-track.x(m_geoTool->zReference() ) ) * 0.001 );

      (*itT).setQuality( quality );
      if ( m_debug ) {
        info() << "*** Accepted as track " << nbOK << " ***" << endmsg;
        printTrack( *itT );
      }
      nbOK++;
    }
  }

  if ( 0 < nbOK ) {
    std::sort( m_trackCandidates.begin(), m_trackCandidates.end(), PrForwardTrack::LowerByQuality() );
    if ( m_debug ) {
      info() << "********** list of candidates ********" << endmsg;
      for ( PrForwardTracks::iterator itT = m_trackCandidates.begin();
            m_trackCandidates.end() != itT; ++itT ) {
        if ( (*itT).valid() ) printTrack( *itT );
      }
    }
    float minQuality = 1.e9;
    for ( PrForwardTracks::iterator itT = m_trackCandidates.begin();
          m_trackCandidates.end() != itT; ++itT ) {
      if ( (*itT).valid() && minQuality > 1.e8 ) {
        minQuality = (*itT).quality() + m_deltaQuality;
        if ( minQuality > m_maxQuality ) minQuality = m_maxQuality;
      }
      if ( (*itT).quality() > minQuality ) (*itT).setValid( false );
    }
  }

  if ( m_debug ) {
    info() << "********** final list of candidates ********" << endmsg;
    for ( PrForwardTracks::iterator itT = m_trackCandidates.begin();
          m_trackCandidates.end() != itT; ++itT ) {
      if ( (*itT).valid() ) printTrack( *itT );
    }
  }

  //====================================================================
  // Finalisation: Produce LHCb tracks
  //====================================================================
  makeLHCbTracks( result );
}
//=========================================================================
//  Collect all X hits, within a window defined by the minimum Pt.
//  Better restrictions possible, if we use the momentum of the input track.
//  Ask for the presence of a stereo hit in the same biLayer compatible.
//  This reduces the efficiency. X-alone hits to be re-added later in the processing
//=========================================================================
void PrForwardTool::collectAllXHits ( PrForwardTrack& track, unsigned int side ) {
  m_allXHits.clear();
  float openingAngle = 1300. * sqrt( track.slX2() + track.slY2() ) / m_minPt;
  float zMag = m_geoTool->zMagnet( track );
  m_nbXPlanes = 0;
  for ( unsigned int zoneNumber = side; m_hitManager->nbZones() > zoneNumber; zoneNumber+=2 ) {
    PrHitZone* zone = m_hitManager->zone(zoneNumber);
    if ( !zone->isX() ) continue;  // only X layers
    if ( m_hitManager->hits(zoneNumber).size() > 0 ) ++m_nbXPlanes;
    float zZone = zone->z();
    unsigned int uvZoneNumber = zoneNumber + 2;  // two zones per 'layer'
    if ( 4 < zone->number()%8 ) uvZoneNumber = zoneNumber - 2;
    if ( 0 == m_hitManager->hits( uvZoneNumber ).size() ) continue;
    PrHitZone* zoneUv = m_hitManager->zone(uvZoneNumber);
    float xInZone = track.xFromVelo( zZone );
    float yInZone = track.yFromVelo( zZone );
    if ( !zone->isInside( xInZone, yInZone ) ) continue;
    float xTol  = openingAngle * ( zZone - zMag );
    float xMin  = xInZone - xTol;
    float xMax  = xInZone + xTol;
    float dx    = yInZone * zoneUv->dxDy();
    PrHits::const_iterator itUv = m_hitManager->hits( uvZoneNumber ).begin();
    float zRatio = ( zoneUv->z() - zMag ) / ( zZone - zMag );
    float xCentral = xInZone + yInZone * zoneUv->dxDy();
    for ( PrHits::const_iterator itH = m_hitManager->hits( zoneNumber ).begin();
          m_hitManager->hits( zoneNumber ).end() != itH; ++itH ) {
      (*itH)->setUsed( false );
      if ( (*itH)->x() < xMin ) continue;
      if ( (*itH)->x() > xMax ) break;
      float xPredUv = xInZone + ( (*itH)->x() - xInZone) * zRatio - dx;
      float maxDx   = m_tolY + fabs( (*itH)->x() -xCentral ) * m_tolYSlope + 0.01 * fabs(dx);
      while ( (*itUv)->x() < xPredUv - maxDx ) {
        if ( itUv == m_hitManager->hits( uvZoneNumber ).end()-1 ) break;
        ++itUv;
      }
      if ( m_debug && matchKey( *itH ) ) {
        info() << format( "UV delta %7.3f / %7.3f ", (*itUv)->x() - xPredUv, maxDx );
        m_geoTool->xAtReferencePlane( track, *itH );
        printHit( *itH, " " );
        info() << format( "  y %8.1f  dx %7.2f   ", yInZone, dx );
        printHit( *itUv, " " );
      }
      if ( (*itUv)->x() < xPredUv - maxDx ) continue;
      if ( (*itUv)->x() > xPredUv + maxDx ) continue;

      m_geoTool->xAtReferencePlane( track, *itH );
      m_allXHits.push_back( *itH );
    }

  }

  std::sort( m_allXHits.begin(), m_allXHits.end(), PrHit::LowerByCoord() );
}

//=========================================================================
//  Select the zones in the allXHits array where we can have a track
//=========================================================================
void PrForwardTool::selectXCandidates( PrForwardTrack& track ) {
  float xStraight = track.xFromVelo( m_geoTool->zReference() );
  if ( m_allXHits.size() < m_minXSize ) return;
  PrHits::iterator it1 = m_allXHits.begin();
  PrHits::iterator it2 = it1 + m_minXSize;
  PrPlaneCounter planeCount;
  while( it2 <= m_allXHits.end() ) {
    float xWindow = m_maxXWindow + ( fabs( (*it1)->coord() ) + fabs( (*it1)->coord() - xStraight ) ) * m_maxXWindowSlope;
    if( (*(it2-1))->coord() - (*it1)->coord() > xWindow ) {
      ++it1;
      if ( it2-it1 < m_minXSize) ++it2;
      continue;
    }
    planeCount.set( it1, it2 );
    while( it2 < m_allXHits.end() &&
           planeCount.nbDifferent() < m_minXSize &&
           (*it2)->coord() - (*it1)->coord() < xWindow ) {
      planeCount.addHit( *it2 );
      ++it2;
    }
    if ( planeCount.nbDifferent() >= m_minXSize ) {
      //== Add next hits until a large enough gap. or no hit in empty zone
      float lastCoord = (*(it2-1))->coord();
      while( it2 < m_allXHits.end() &&
             ( (*it2)->coord() < lastCoord + m_maxXGap ||
               ( (*it2)->coord() - (*it1)->coord() < xWindow &&
                 planeCount.nbInPlane( (*it2)->planeCode() ) == 0 )
               )
             ) {
        lastCoord = (*it2)->coord();
        planeCount.addHit( *it2 );
        ++it2;
      }
      //====================================================================
      // Try to find a group of m_nbXPlanes planes with very small interval
      //====================================================================
      if ( it2 - it1 > m_nbXPlanes && planeCount.nbDifferent() == m_nbXPlanes ) {
        PrPlaneCounter pc;
        PrHits::iterator itStart = it1;
        PrHits::iterator it3 = it1 + m_nbXPlanes;
        PrHits::iterator best = it1;
        float minInterval = 1.e9;
        pc.set( it1, it3 );
        while ( it3 != it2+1 ) {
          if ( pc.nbDifferent() == m_nbXPlanes ) {
            float dist = (*(it3-1))->coord() - (*it1)->coord();
            if ( dist < minInterval ) {
              if ( m_debug ) {
                info() << format( "dist%7.3f start ", dist );
                printHit( *it1, " " );
              }
              minInterval = dist;
              best = it1;
            }
          }
          if ( it3 == it2 ) break;
          pc.removeHit( *it1 );
          pc.addHit( *it3 );
          ++it1;
          ++it3;
        }
        it1 = itStart;
        if ( minInterval < 1. ) {
          it1 = best;
          it2 = best+m_nbXPlanes;
        }
      }

      //== We have a possible candidate. Compute average x at reference
      PrHits coordToFit;
      for ( PrHits::iterator itH = it1; it2 > itH; ++itH ) {
        if ( !(*itH)->isUsed() ) coordToFit.push_back( *itH );
      }
      float xAtRef = 0.;
      for ( PrHits::iterator itH = coordToFit.begin(); coordToFit.end() != itH; ++itH ) {
        xAtRef += (*itH)->coord();
      }
      xAtRef /= coordToFit.size();

      PrForwardTrack tmp( track.track(), m_geoTool->zReference(), coordToFit );
      m_geoTool->setTrackParameters( tmp, xAtRef );

      fastLinearFit( tmp );
      addHitsOnEmptyXLayers( tmp );

      //== Fit and remove...
      bool ok = fitXProjection( tmp );
      if ( ok ) ok = tmp.chi2PerDoF() < m_maxChi2PerDoF;
      if ( ok ) ok = addHitsOnEmptyXLayers( tmp );
      if ( ok ) {
        float minXCoord = 1.e9;
        float maxXCoord = -1.e9;
        for ( PrHits::iterator itH = tmp.hits().begin(); tmp.hits().end() != itH; ++itH ) {
          (*itH)->setUsed( true );
          if ( (*itH)->coord() < minXCoord ) minXCoord = (*itH)->coord();
          if ( (*itH)->coord() > maxXCoord ) maxXCoord = (*itH)->coord();
        }
        float dXCoord = maxXCoord - minXCoord;
        tmp.setDXCoord( dXCoord );
        if ( m_debug ) {
          info() << "=== Storing track candidate " << m_trackCandidates.size() << endmsg;
          printTrack( tmp );
          info() << endmsg;
        }
        if ( tmp.hits().size() > m_minXSize+1 ) m_minXSize = tmp.hits().size()-1;
        m_trackCandidates.push_back( tmp );
      }
    }

    it1++;
    while( it1+1 < m_allXHits.end() && (*it1)->isUsed() ) ++it1;
    it2 = it1 + m_minXSize;
    if ( it2 > m_allXHits.end() ) break;
  }
}

//=========================================================================
//  Fit the X projection of a track, return OK if fit sucecssfull
//=========================================================================
bool PrForwardTool::fitXProjection ( PrForwardTrack& track ) {
  if ( m_debug ) {
    info() << "  -- Entering fitXProjection with:" << endmsg;
    printTrack( track );
  }
  PrPlaneCounter pc;
  pc.set( track.hits().begin(), track.hits().end() );
  if ( pc.nbDifferent() < m_minXHits ) {
    if ( m_debug ) info() << "  == Not enough layers ( "<< pc.nbDifferent() << " ) with hits" << endmsg;
    return false;
  }
  bool doFit = true;
  while ( doFit ) {
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
    float maxChi2 = 0.;
    float totChi2 = 0.;
    int   nDoF = -3; // fitted 3 parameters
    bool hasStereo = false;
    bool notMultiple = pc.nbDifferent() == track.hits().size();
    for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
      if ( !(*itH)->isX() ) hasStereo = true;
      float chi2 = track.chi2( *itH );
      totChi2 += chi2;
      ++nDoF;
      if ( chi2 > maxChi2 && ( notMultiple || pc.nbInPlane( (*itH)->planeCode() ) > 1 ) ) {
        maxChi2 = chi2;
        worst   = itH;
      }
    }
    if ( hasStereo ) nDoF -= 3; // Fitted a parabola...
    track.setChi2( totChi2, nDoF );
    if ( m_debug ) {
      info() << "  -- In fitXProjection, maxChi2 = " << maxChi2 << endmsg;
      printTrack( track );
    }
    if ( worst == track.hits().end() ) {
      info() << "No worst hit ! nbDiff = " << pc.nbDifferent() << " nbHits " << track.hits().size() << endmsg;
      printTrack( track );
      return false;
    }

    doFit = false;
    if ( totChi2/nDoF > m_maxChi2PerDoF  ||
         maxChi2 > m_maxChi2XProjection ) {
      pc.removeHit( *worst );
      track.hits().erase( worst );
      if ( pc.nbDifferent() < m_minXHits ) {
        if ( m_debug ) {
          info() << "  == Not enough layers with hits" << endmsg;
          printTrack( track );
        }
        return false;
      }
      doFit = true;
    }
  }
  if ( m_debug ) {
    info() << "  -- End fitXProjection -- " << endmsg;
    printTrack( track );
  }

  return true;
}

//=========================================================================
//  Fit a linear form, remove the external worst as long as chi2 is big...
//=========================================================================
void PrForwardTool::fastLinearFit ( PrForwardTrack& track ) {
  PrPlaneCounter pc;
  pc.set( track.hits().begin(), track.hits().end() );

  bool fit = true;
  while ( fit ) {
    //== Fit a line
    float s0   = 0.;
    float sz   = 0.;
    float sz2  = 0.;
    float sd   = 0.;
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
    float den = (sz*sz-s0*sz2);
    float da = (sdz * sz - sd * sz2) / den;
    float db = (sd *  sz - s0 * sdz) / den;
    track.updateParameters( da, db, 0.);
    fit = false;
    if ( m_debug ) {
      info() << "Linear fit, current status : " << endmsg;
      printTrack( track );
    }
    if ( track.hits().size() < m_minXHits ) return;

    PrHits::iterator worst = track.hits().end();
    float maxChi2 = 0.;
    bool notMultiple = pc.nbDifferent() == track.hits().size();
    for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
      float chi2 = track.chi2( *itH );
      if ( chi2 > maxChi2 && ( notMultiple || pc.nbInPlane( (*itH)->planeCode() ) > 1 ) ) {
        maxChi2 = chi2;
        worst   = itH;
      }
    }

    //== Remove grossly out hit

    if ( maxChi2 > 100. ) {
      if ( m_debug ) {
        info() << "Remove hit ";
        printHit( *worst );
      }
      pc.removeHit( *worst );
      track.hits().erase( worst );
      fit = true;
    }
  }
}

//=========================================================================
//  Add hits on empty X layers, and refit if something was added
//=========================================================================
bool PrForwardTool::addHitsOnEmptyXLayers ( PrForwardTrack& track ) {
  PrPlaneCounter planeCount;
  planeCount.set( track.hits().begin(), track.hits().end() );
  bool added = false;
  float x1 = track.hits().front()->coord();
  float xStraight = track.xFromVelo( m_geoTool->zReference() );
  float xWindow = m_maxXWindow + ( fabs( x1 ) + fabs( x1 - xStraight ) ) * m_maxXWindowSlope;

  for ( unsigned int zoneNumber = track.zone(); m_hitManager->nbZones() > zoneNumber; zoneNumber+=2 ) {
    PrHitZone* zone = m_hitManager->zone(zoneNumber);
    if ( !zone->isX() ) continue;  // only X zones
    if ( planeCount.nbInPlane( zoneNumber/2 ) != 0 ) continue;
    float zZone = zone->z();
    float xPred  = track.x( zZone );
    PrHit* best = NULL;
    float bestChi2 = m_maxChi2ToAddExtraXHits;
    float minX = xPred - xWindow;
    float maxX = xPred + xWindow;
    for ( PrHits::iterator itH = zone->hits().begin(); zone->hits().end() != itH; ++itH ) {
      if ( (*itH)->x() < minX ) continue;
      if ( (*itH)->x() > maxX ) break;
      float chi2 = track.chi2( *itH );
      if ( chi2 < bestChi2 ) {
        bestChi2 = chi2;
        best = *itH;
      }
    }
    if ( NULL != best ) {
      if ( m_debug ) {
        info() << format( "AddHitOnEmptyXLayer:    chi2%8.2f", bestChi2 );
        printHit( best, " ");
      }
      m_geoTool->xAtReferencePlane( track, best );
      best->setUsed( false );
      track.addHit( best );
      added = true;
    }
  }
  if ( added ) return fitXProjection( track );
  return true;
}
//=========================================================================
//  Collect all hits in the stereo planes compatible with the track
//=========================================================================
void PrForwardTool::collectStereoHits ( PrForwardTrack& track ) {
  if ( m_debug ) info() << "== Collecte stereo hits. wanted ones: " << endmsg;
  m_stereoHits.clear();
  for ( unsigned int zoneNumber = track.zone(); m_hitManager->nbZones() > zoneNumber; zoneNumber += 2 ) {
    PrHitZone* zone = m_hitManager->zone(zoneNumber);
    if ( zone->isX() ) continue;  // exclude X zones
    if ( zone->hits().empty()) continue;
    float zZone = zone->z();
    float yZone = track.y( zZone );
    zZone = zone->z( yZone );  // Correct for dzDy
    float xPred  = track.x( zZone );
    if ( m_debug ) {
      for ( PrHits::iterator itH = zone->hits().begin(); zone->hits().end() != itH; ++itH ) {
        float dx = (*itH)->x( yZone ) - xPred;
        (*itH)->setCoord( dx );
        if ( matchKey( *itH ) ) printHit( *itH, "-- " );
      }
    }

    float dxTol = m_tolY + m_tolYSlope * fabs( xPred - track.xFromVelo( zZone ) );
    // dxTol += 0.002 * fabs( yZone );

    for ( PrHits::iterator itH = zone->hits().begin(); zone->hits().end() != itH; ++itH ) {
      float dx = (*itH)->x( yZone ) - xPred ;
      if ( dx < - dxTol ) continue;
      if ( dx >   dxTol ) break;
      if ( zone->dxDy() < 0 ) dx = -dx;
      (*itH)->setCoord( dx );
      (*itH)->setUsed( false );
      m_stereoHits.push_back( *itH );
    }
  }
  std::sort( m_stereoHits.begin(), m_stereoHits.end(), PrHit::LowerByCoord() );
}

//=========================================================================
//  Fit the stereo hits
//=========================================================================
bool PrForwardTool::fitStereoHits( PrForwardTrack& track ) {
  PrHits::iterator beginRange = m_stereoHits.begin() - 1;
  while ( beginRange < m_stereoHits.end() - m_minStereoHits ) {
    beginRange++;
    if ( m_debug ) {
      info() << " stereo start at ";
      printHit( *beginRange );
    }
    
    PrPlaneCounter pc;
    PrHits::iterator endRange   = beginRange;
    float sumCoord = 0.;
    while( pc.nbDifferent() < m_minStereoHits ||
           (*endRange)->coord() < (*(endRange-1))->coord() + 0.25 ) {
      pc.addHit( *endRange );
      sumCoord += (*endRange)->coord();
      ++endRange;
      if ( endRange == m_stereoHits.end() ) break;
    }
    bool changed = true;
    while( changed ) {
      float averageCoord = sumCoord / (endRange-beginRange);
      changed = false;
      if ( endRange != m_stereoHits.end() ) {
        if ( pc.nbInPlane( (*endRange)->planeCode() ) == 0 ) {  // next is new -> add
          pc.addHit( *endRange );
          sumCoord += (*endRange)->coord();
          ++endRange;
          changed = true;
        } else if ( averageCoord - (*beginRange)->coord() >  // next decreases the range size
                    (*(endRange))->coord() - averageCoord ) {
          if ( (*endRange)->planeCode() == (*beginRange)->planeCode() ||
               pc.nbInPlane( (*beginRange)->planeCode() ) >1 ) {
            pc.addHit( *endRange );
            pc.removeHit( *beginRange );
            sumCoord = sumCoord + (*endRange)->coord() - (*beginRange)->coord();
            ++beginRange;
            ++endRange;
            changed = true;
          } else {
            pc.addHit( *endRange );
            sumCoord += (*endRange)->coord();
            ++endRange;
            changed = true;
          }
        }
      }
      if ( pc.nbInPlane( (*beginRange)->planeCode() ) > 1 ) {
        pc.removeHit( *beginRange );
        sumCoord -= (*beginRange)->coord();
        ++beginRange;
        changed = true;
      }
    }

    PrHits stereoHits( beginRange, endRange );
    float maxChi2 = 1.e9;
    bool parabola = false;
    while ( maxChi2 > m_maxChi2Stereo ) {
      //== Fit a line
      float w     = 0.01;  // 10 mm constrait for y=0 at z=zMagnet
      float zMag  = m_geoTool->zMagnet( track );
      float dyMag = track.y( zMag ) - track.yFromVelo( zMag );
      zMag -= m_geoTool->zReference();
      float s0  = w;
      float sz  = w * zMag;
      float sz2 = w * zMag * zMag;
      float sz3  = 0.;
      float sz4  = 0.;
      float sd  = w * dyMag;
      float sdz = w * dyMag * zMag;
      float sdz2 = 0.;
      for ( PrHits::iterator itH = stereoHits.begin(); stereoHits.end() != itH; ++itH ) {
        float d = -track.deltaY( *itH );
        float w = (*itH)->w();
        float z = (*itH)->z() - m_geoTool->zReference();
        s0   += w;
        sz   += w * z;
        sz2  += w * z * z;
        sz3  += w * z * z * z;
        sz4  += w * z * z * z * z;
        sd   += w * d;
        sdz  += w * d * z;
        sdz2 += w * d * z * z;
      }
      if ( parabola ) {
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

        track.updateParameters( 0., 0., 0., 0., da, db, dc );
      } else {
        float den = (s0 * sz2 - sz * sz );
        float da  = (sd * sz2 - sdz * sz ) / den;
        float db  = (sdz * s0 - sd  * sz ) / den;
        track.updateParameters( 0., 0., 0., 0., da, db);
      }

      PrHits::iterator worst = stereoHits.end();
      maxChi2 = 0.;
      for ( PrHits::iterator itH = stereoHits.begin(); stereoHits.end() != itH; ++itH ) {
        float chi2 = track.chi2( *itH );
        if ( chi2 > maxChi2 ) {
          maxChi2 = chi2;
          worst   = itH;
        }
      }

      if ( maxChi2 > m_maxChi2Stereo ) {
        if ( m_debug ) printHitsForTrack( stereoHits, track );
        pc.removeHit( *worst );
        if ( pc.nbDifferent() < m_minStereoHits ) {
          if ( m_debug ) info() << "-- not enough different planes --" << endmsg;
          break;
        }
        stereoHits.erase( worst );
        parabola = true;

        continue;
      } else if ( !parabola ) {
        parabola = true;
        maxChi2 = 1.e9;
        continue;
      }
      
      //== Store average dy
      float meanDy = 0.;
      m_stereoHits = stereoHits;
      for ( PrHits::iterator itH = m_stereoHits.begin(); m_stereoHits.end() != itH; ++itH ) {
        meanDy += (*itH)->coord();
      }
      meanDy = fabs( meanDy / m_stereoHits.size() );

      float dX = track.xFromVelo( m_geoTool->zReference() ) - track.x( m_geoTool->zReference() );

      meanDy = meanDy / ( 1. + m_tolYSlope / m_tolY * fabs( dX ) );

      track.setMeanDy( meanDy );

      return true;
    }
  }
  return false;
}

//=========================================================================
//  Convert the local track to the LHCb representation
//=========================================================================
void PrForwardTool::makeLHCbTracks ( LHCb::Tracks* result ) {
  for ( PrForwardTracks::iterator itT = m_trackCandidates.begin();
        m_trackCandidates.end() != itT; ++itT ) {
    if ( !(*itT).valid() ) continue;
    LHCb::Track* tmp = (*itT).track()->clone();
    tmp->setType( LHCb::Track::Long );
    tmp->setHistory( LHCb::Track::PatForward );
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
    tmp->addInfo( LHCb::Track::PatQuality, (*itT).quality() );
    result->insert( tmp );
    if ( m_debug ) info() << "Store track " << (*(result->end()-1))->key() 
                          << " quality " << (*(result->end()-1))->info( LHCb::Track::PatQuality, 0. ) << endmsg;
  }
}
//=============================================================================
