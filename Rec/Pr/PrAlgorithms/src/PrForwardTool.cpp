// Include files 

// from Gaudi
#include "Event/StateParameters.h"
// local
#include "PrForwardTool.h"
#include "PrForwardTrack.h"
#include "PrPlaneCounter.h"
#include "PrLineFitter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrForwardTool
//
// 2012-03-20 : Olivier Callot
// 2013-03-15 : Thomas Nikodem
// 2015-02-13 : Sevda Esen [additional search in the triangles by Marian Stahl]
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
  declareProperty( "MaxXGap",                m_maxXGap                =  0.8 * Gaudi::Units::mm  );
  declareProperty( "MaxChi2LinearFit",       m_maxChi2LinearFit       =  100.                    );
  declareProperty( "MaxChi2XProjection",     m_maxChi2XProjection     =    9.                    );
  declareProperty( "MaxChi2PerDoF",          m_maxChi2PerDoF          =    4.                    );
  declareProperty( "MinYGap",                m_minYGap                =  0.5 * Gaudi::Units::mm  );
  declareProperty( "TolYMag",                m_tolYMag                =   10.* Gaudi::Units::mm  );
  declareProperty( "TolYMagSlope",           m_tolYMagSlope           =    0.015                 );
  declareProperty( "MaxYChi2AtMagnet",       m_maxYChi2AtMagnet       =   20.                    );
  declareProperty( "MinTotalHits",           m_minTotalHits           =   10                     );
  declareProperty( "MaxChi2StereoLinear",    m_maxChi2StereoLinear    =   50.                    );
  declareProperty( "MaxChi2Stereo",          m_maxChi2Stereo          =    4.                    );
  declareProperty( "MaxQuality",             m_maxQuality             =   15.                    );
  declareProperty( "DeltaQuality",           m_deltaQuality           =    3.                    );
  declareProperty( "YQualityWeight",         m_yQualityWeight         =    4.                    );


  declareProperty( "AddUTHitsToolName",      m_addUTHitsToolName      = "PrAddUTHitsTool"        );
  declareProperty( "YToleranceUVsearch",     m_yTol_UVsearch          =   10.* Gaudi::Units::mm  );          
  declareProperty( "UseTriangleFix",         m_useTFix                =    true                  );

  declareProperty( "UseMomentumEstimate",    m_useMomentumEstimate    = false                    ); 
  declareProperty( "Preselection",           m_Preselection           = false                    );
  declareProperty( "PreselectionPT",         m_PreselectionPT         = 400.* Gaudi::Units::MeV  );
  declareProperty( "UseWrongSignWindow",     m_useWrongSignWindow     = false                    );
  declareProperty( "WrongSignPT",            m_wrongSignPT            = 2000.* Gaudi::Units::MeV );
  
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

  if ( "" != m_addUTHitsToolName  ) {
    m_addUTHitsTool = tool<IPrAddUTHitsTool>( m_addUTHitsToolName, this );
  } else {
    m_addUTHitsTool = NULL;
  }
  
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

    // -- > Debug --------
    if ( m_debug ) {
      float xWindow = m_maxXWindow + fabs( track.xFromVelo( m_geoTool->zReference() )) * m_maxXWindowSlope;
      info() << "**** Processing Velo track " << velo->key() << " zone " << side
             << " Selected " << m_allXHits.size() << " hits, window size " << xWindow << endmsg;
      for ( PrHits::const_iterator itH = m_allXHits.begin(); m_allXHits.end() != itH; ++itH ) {
        if ( matchKey( *itH ) ) printHit( *itH, " " );
      }
    }
    // -- < Debug --------

    //== Select groups and make temporary tracks
    selectXCandidates( track );
  }

  // -- > Debug --------
  if ( m_debug ) {
    info() << endmsg
           << "=============== Selected " << m_trackCandidates.size() << " candidates."
           << endmsg
           << endmsg;
    for ( PrForwardTracks::const_iterator itT = m_trackCandidates.begin();
          m_trackCandidates.end() != itT; ++itT ) {
      if ( (*itT).valid() ) printTrack( *itT );
    }
  }
  // -- < Debug --------

  int nbOK = 0;

  for ( PrForwardTracks::iterator itT = m_trackCandidates.begin();
        m_trackCandidates.end() != itT; ++itT ) {
    if ( !(*itT).valid() ) continue;
    collectStereoHits( *itT );
    
    // -- > Debug --------
    if ( m_debug ) {
      info() << "**** Stereo hits for track " << itT - m_trackCandidates.begin() << " ****" << endmsg;
      for ( PrHits::const_iterator itH = m_stereoHits.begin(); m_stereoHits.end() != itH; ++itH ) {
        printHit( *itH, " " );
      }
    }
    // -- < Debug --------

    (*itT).setValid( false );
    PrPlaneCounter pc;
    pc.set( m_stereoHits.begin(), m_stereoHits.end(), false );
    if ( pc.nbDifferent() + (*itT).hits().size()  < m_minTotalHits  || pc.nbDifferent() < 4 ) {
      
      // -- > Debug --------
      if ( m_debug ) info() << "Not enough different layers: " << pc.nbDifferent() + (*itT).hits().size()
                            << " for " << m_minTotalHits << endmsg;
      // -- < Debug --------
      continue;
    }
    
    m_minStereoHits =std::max( static_cast<unsigned int>(m_minTotalHits - (*itT).hits().size()),4u);//10

    if ( !fitStereoHits( *itT ) ) continue;

    (*itT).setValid( true );
    (*itT).addHits( m_stereoHits );
    (*itT).setHitsUnused();
    fitXProjection ( *itT );  //== Fit both x and stereo

    if ( (*itT).hits().size() < m_minTotalHits ) {
      
      // -- > Debug --------
      if ( m_debug ) {
        info() <<"*** rejected: no eneough hits in total ***" << endmsg;
        printTrack( *itT );
      }
      // -- < Debug --------

      (*itT).setValid( false );
    } else {

      //== Compute a quality factor for later filtering

      float quality = (*itT).dXCoord() + m_yQualityWeight * (*itT).meanDy();
      if ( (*itT).hits().size() < 12 ) quality += (12-(*itT).hits().size());
      quality /= ( 1. + fabs( track.xFromVelo(m_geoTool->zReference() )-track.x(m_geoTool->zReference() ) ) * 0.001 );

      (*itT).setQuality( quality );
      
      // -- > Debug --------
      if ( m_debug ) {
        info() << "*** Accepted as track " << nbOK << " ***" << endmsg;
        printTrack( *itT );
      }
      // -- < Debug --------

      ++nbOK;
    }
  }

  if ( 0 < nbOK ) {
    std::sort( m_trackCandidates.begin(), m_trackCandidates.end(), PrForwardTrack::LowerByQuality() );
    
    // -- > Debug --------
    if ( m_debug ) {
      info() << "********** list of candidates ********" << endmsg;
      for ( PrForwardTracks::iterator itT = m_trackCandidates.begin();
            m_trackCandidates.end() != itT; ++itT ) {
        if ( (*itT).valid() ) printTrack( *itT );
      }
    }
    // -- < Debug --------

    float minQuality = 1.e9;
    nbOK = 0;
    for ( PrForwardTracks::iterator itT = m_trackCandidates.begin();
          m_trackCandidates.end() != itT; ++itT ) {
      if ( (*itT).valid() && minQuality > 1.e8 ) {
        minQuality = (*itT).quality() + m_deltaQuality;
        if ( minQuality > m_maxQuality ) minQuality = m_maxQuality;
      }
      if ( (*itT).quality() > minQuality ) (*itT).setValid( false );
      if ( (*itT).valid() ) ++nbOK;
    }
  }

  // -- > Debug --------
  if ( m_debug ) {
    info() << "********** final list of candidates, Velo = " << velo->key() << " ********" << endmsg;
    for ( PrForwardTracks::iterator itT = m_trackCandidates.begin();
          m_trackCandidates.end() != itT; ++itT ) {
      if ( (*itT).valid() ) printTrack( *itT );
    }
  }
  // -- < Debug --------

  //====================================================================
  // Finalisation: Produce LHCb tracks (and add UT hits)
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
  //== Compute the size of the search window in the reference plane
  float dxRef = 3973000. * sqrt( track.slX2() + track.slY2() ) / m_minPt - 2200. * track.slY2() - 1000. * track.slX2();
  dxRef *= 1.10; //== 10% tolerance
  float zMag = m_geoTool->zMagnet( track );
  m_nbXPlanes = 0;
  
  double q = track.qOverP() > 0 ? 1. : -1.;
  float pt = track.track()->pt();
  double magscalefactor = m_geoTool->magscalefactor();
  float dir = q*magscalefactor*(-1);

  for ( unsigned int zoneNumber = side; m_hitManager->nbZones() > zoneNumber; zoneNumber+=2 ) {
    PrHitZone* zone = m_hitManager->zone(zoneNumber);
    if ( !zone->isX() ) continue;  // only X layers
    if ( m_hitManager->hits(zoneNumber).size() > 0 ) ++m_nbXPlanes;
    float zZone = zone->z();

    //the next UV zone in the same half of the detector  
    unsigned int uvZoneNumber = zoneNumber + 2;  // two zones per 'layer'
    //the next UV zone in the same half of the detector  
    if ( 4 < zone->number()%8 ) uvZoneNumber = zoneNumber - 2;
    //UV zone in opposite half of the detector --> gets hits in overlapping triangles
    unsigned int triangleZone = uvZoneNumber + 1;
    if ( zoneNumber%2 ) triangleZone = uvZoneNumber - 1;
    
    float xInZone = track.xFromVelo( zZone );
    float yInZone = track.yFromVelo( zZone );
    if ( !zone->isInside( xInZone, yInZone ) ) continue;
    float xTol  = dxRef * zZone / m_geoTool->zReference();
    if ( zZone > m_geoTool->zReference() ) xTol = dxRef * (zZone - zMag) / ( m_geoTool->zReference() - zMag );
    float xMin  = xInZone - xTol;
    float xMax  = xInZone + xTol;

    // -- Use momentum estimate from VeloUT tracks
    if(m_useMomentumEstimate && 0 != track.qOverP() ){
          
      // - Preselect the VeloUT tracks
      if(m_Preselection && pt<m_PreselectionPT) continue;

      // -- Extra window to catch wrong sign tracks
      float xTolWS = 0;
      if(m_useWrongSignWindow && pt>m_wrongSignPT){
        float dxRefWS = 
          3973000. * sqrt( track.slX2() + track.slY2() ) / m_wrongSignPT - 2200. * track.slY2() - 1000. * track.slX2();
        dxRefWS *= 1.10; //== 10% tolerance
	
        xTolWS  = dxRefWS * zZone / m_geoTool->zReference();
        if ( zZone > m_geoTool->zReference() ) xTolWS = dxRefWS * (zZone - zMag) / ( m_geoTool->zReference() - zMag );
      }
      
      if(dir > 0){
        xMin = xInZone - xTolWS;
        xMax = xInZone + xTol;
      }else{
        xMin = xInZone - xTol;
        xMax = xInZone + xTolWS;
      }
    }
    
    // -- Use a binary search to find the lower bound of the range of x values
    PrHits::iterator itH = std::lower_bound(  m_hitManager->hits( zoneNumber ).begin(),  
                                              m_hitManager->hits( zoneNumber ).end(), 
                                              xMin, 
                                              lowerBoundX() );
    

    PrHits::iterator itEnd =  m_hitManager->hits( zoneNumber ).end();
    for ( ; itEnd != itH; ++itH ) {
      if ( (*itH)->x() > xMax ) break;
      (*itH)->setUsed( false );
      
      bool hasUVHit = false;
      //search for same side UV hit
      hasUVHit = matchStereoHit(track,itH,uvZoneNumber);
      //if no matching UV hit was found, search in other side, to cover triangles
      if(m_useTFix)
        hasUVHit ? hasUVHit : hasUVHit = matchStereoHit(track,itH,triangleZone);
      
      if(hasUVHit){
        
        m_geoTool->xAtReferencePlane( track, *itH );
        m_allXHits.push_back( *itH );
      }
    }
  }

  std::sort( m_allXHits.begin(), m_allXHits.end(), PrHit::LowerByCoord() );
}

//=========================================================================
//  Select the zones in the allXHits array where we can have a track
//=========================================================================
void PrForwardTool::selectXCandidates( PrForwardTrack& track ) {
  if ( m_allXHits.size() < m_minXSize ) return;
  float xStraight = track.xFromVelo( m_geoTool->zReference() );
  PrHits::iterator it1 = m_allXHits.begin();
  PrHits::iterator it2 = it1 + m_minXSize;
  PrPlaneCounter planeCount;
  
  while( it2 <= m_allXHits.end() ) {
    float xWindow = m_maxXWindow + ( fabs( (*it1)->coord() ) + fabs( (*it1)->coord() - xStraight ) ) * m_maxXWindowSlope;
    if( (*(it2-1))->coord() - (*it1)->coord() > xWindow ) {  // Too wide range -> increase start
      ++it1;
      if ( it2-it1 < m_minXSize) ++it2;
      continue;
    }
  
    planeCount.set( it1, it2 );
    
    while( it2 < m_allXHits.end() &&
           planeCount.nbDifferent() < m_minXSize &&
           (*it2)->coord() - (*it1)->coord() < xWindow ) {  
      // Add hit if not enough different planes and range small enough
      planeCount.addHit( *it2 );
      ++it2;
    }
    
    if ( planeCount.nbDifferent() >= m_minXSize ) {
      //== Add next hits until a large enough gap or the hit is in a no yet hit plane
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
      // Select from single plane layers if enough single plane layers.
      //====================================================================
      PrHits coordToFit;
      float xAtRef = 0.;
      
      if ( planeCount.nbSingle() > 3 ) {
        
        // -- > Debug --------
        if ( m_debug ) {
          info() << "--- " <<   planeCount.nbSingle() 
                 << " planes with a single hit. Select best chi2 in other planes ---" << endmsg;
        }
        // -- < Debug --------
        
        PrLineFitter line( m_geoTool->zReference() );
        std::array<PrHits,12> otherHits;
        PrHits::iterator itH;
        for ( itH = it1; it2 > itH; ++itH ) {
          if ( (*itH)->isUsed() ) continue;
          
          // -- > Debug --------
          if ( m_debug ) printHit( *itH );
          // -- < Debug --------
          
          if ( planeCount.nbInPlane( (*itH)->planeCode() ) == 1 ) {
            line.addHit( *itH );
          } else {
            otherHits[ (*itH)->planeCode() ].push_back( *itH );
          }
        }
        
        // -- > Debug --------
        if ( m_debug ) {
          info() << "Start from " << line.hits().size() << " and add other." << endmsg;
          for ( itH = line.hits().begin(); line.hits().end() != itH; ++itH ) {
            printHit( *itH );
          }
        }
        // -- < Debug --------
        
        for ( int pl = 0 ; 12 > pl ; ++pl ) {
          float bestChi2 = 1.e9;
          PrHit* best = NULL;
          for ( PrHits::iterator itH = otherHits[pl].begin(); otherHits[pl].end() > itH; ++itH ) {
            float chi2 = line.chi2( *itH );
            if ( chi2 < bestChi2 ) {
              bestChi2 = chi2;
              best = *itH;
            }
          }
          if ( NULL != best ) {
            line.addHit( best );
            if ( m_debug ) {
              printHit( best );
            }
          }
        }
        
        coordToFit = line.hits();
        xAtRef = line.coordAtRef();
      } else {
        
        //====================================================================
        // Try to find a group of different planes with very small interval
        //====================================================================
        unsigned int itdiff = it2-it1;
        
        if ( itdiff > m_nbXPlanes && planeCount.nbDifferent() == m_nbXPlanes ) {
          PrPlaneCounter pc;
          PrHits::iterator itStart = it1;
          PrHits::iterator it3 = it1 + m_nbXPlanes;
          PrHits::iterator best = it1;
          float minInterval = 1.e9;
          pc.set( it1, it3, false );
        
          while ( it3 != it2+1 ) {
            if ( pc.nbDifferent() == m_nbXPlanes ) {
              float dist = (*(it3-1))->coord() - (*it1)->coord();
              if ( dist < minInterval ) {
                
                // -- > Debug --------
                if ( m_debug ) {
                  info() << format( "dist%7.3f start ", dist );
                  printHit( *it1, " " );
                }
                // -- < Debug --------
                
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
        for ( PrHits::iterator itH = it1; it2 > itH; ++itH ) {
          if ( !(*itH)->isUsed() ) {
            coordToFit.push_back( *itH );
            xAtRef += (*itH)->coord();
          }
        }
        xAtRef /= coordToFit.size();
      }

      //=== We have a candidate. Create a temporary track, fit and keep if OK.

      PrForwardTrack tmp( track.track(), m_geoTool->zReference(), coordToFit );
      m_geoTool->setTrackParameters( tmp, xAtRef );

      fastLinearFit( tmp );
      addHitsOnEmptyXLayers( tmp, false );

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

        // -- > Debug --------
        if ( m_debug ) {
          info() << "=== Storing track candidate " << m_trackCandidates.size() << endmsg;
          printTrack( tmp );
          info() << endmsg;
        }
        // -- < Debug --------

        if ( tmp.hits().size() > m_minXSize+1 ) {
          m_minXSize = tmp.hits().size()-1;
          for ( PrForwardTracks::iterator itT = m_trackCandidates.begin();
                m_trackCandidates.end() != itT; ++itT ) {
            if ( (*itT).hits().size() < m_minXSize ) {
              // -- > Debug --------
              
              if ( m_debug ) info() << "=== Invalidate track " << itT - m_trackCandidates.begin()
                                    << " as the minimum size is now " << m_minXSize << endmsg;
              // -- < Debug --------

              (*itT).setValid( false );
            }
          }
        }
        m_trackCandidates.push_back( tmp );
      }
    }

    ++it1;
    while( it1+1 < m_allXHits.end() && (*it1)->isUsed() ) ++it1;
    it2 = it1 + m_minXSize;
    if ( it2 > m_allXHits.end() ) break;
  }
}
//=========================================================================
//  Fit a linear form, remove the external worst as long as chi2 is big...
//=========================================================================
void PrForwardTool::fastLinearFit ( PrForwardTrack& track ) {
  
  PrPlaneCounter pc;
  pc.set( track.hits().begin(), track.hits().end(), false );

  bool fit = true;
  while ( fit ) {
    //== Fit a line
    float s0   = 0.;
    float sz   = 0.;
    float sz2  = 0.;
    float sd   = 0.;
    float sdz  = 0.;
    
    PrHits::const_iterator itEnd = track.hits().end();
    for ( PrHits::iterator itH = track.hits().begin(); itEnd != itH; ++itH ) {
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
    float da  = (sdz * sz - sd * sz2) / den;
    float db  = (sd *  sz - s0 * sdz) / den;
    track.updateParameters( da, db, 0.);
    fit = false;
  
    // -- > Debug --------
    if ( m_debug ) {
      info() << "Linear fit, current status : " << endmsg;
      printTrack( track );
    }
    // -- < Debug --------
    
    if ( track.hits().size() < m_minXSize ) return;

    PrHits::iterator worst = track.hits().end();
    float maxChi2 = 0.;
    bool notMultiple = pc.nbDifferent() == track.hits().size();
    
    for ( PrHits::iterator itH = track.hits().begin(); itEnd != itH; ++itH ) {
      float chi2 = track.chi2( *itH );
      if ( chi2 > maxChi2 && ( notMultiple || pc.nbInPlane( (*itH)->planeCode() ) > 1 ) ) {
        maxChi2 = chi2;
        worst   = itH;
      }
    }

    //== Remove grossly out hit, or worst in multiple layers

    if ( maxChi2 > m_maxChi2LinearFit || ( !notMultiple && maxChi2 > 4. ) ) {
    
      // -- > Debug --------
      if ( m_debug ) {
        info() << "Remove hit ";
        printHit( *worst );
      }
      // -- < Debug --------
      
      pc.removeHit( *worst );
      track.hits().erase( worst );
      fit = true;
    }
  }
}
//=========================================================================
//  Fit the X projection of a track, return OK if fit sucecssfull
//=========================================================================
bool PrForwardTool::fitXProjection ( PrForwardTrack& track ) {
  
  // -- > Debug --------
  if ( m_debug ) {
    info() << "  -- Entering fitXProjection with:" << endmsg;
    printTrack( track );
  }
  // -- < Debug --------

  PrPlaneCounter pc;
  pc.set( track.hits().begin(), track.hits().end(),false );
  if ( pc.nbDifferent() < m_minXSize ) {
    
    // -- > Debug --------
    if ( m_debug ) info() << "  == Not enough layers ( "<< pc.nbDifferent() << " ) with hits" << endmsg;
    // -- < Debug --------

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

    PrHits::const_iterator itEnd = track.hits().end();
    for ( PrHits::iterator itH = track.hits().begin(); itEnd != itH; ++itH ) {
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
    if(den == 0)return false;
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
    
    for ( PrHits::iterator itH = track.hits().begin(); itEnd != itH; ++itH ) {
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
    if ( nDoF < 1 )return false;
    track.setChi2( totChi2, nDoF );
  
    // -- > Debug --------
    if ( m_debug ) {
      info() << "  -- In fitXProjection, maxChi2 = " << maxChi2 << " totCHi2/nDof " << totChi2/nDoF << endmsg;
      printTrack( track );
    }
    // -- < Debug --------

    if ( worst == itEnd ) {
      //info() << "No worst hit ! nbDiff = " << pc.nbDifferent() << " nbHits " << track.hits().size() << endmsg;
      //printTrack( track );
      return true;
    }
    doFit = false;
    if ( totChi2/nDoF > m_maxChi2PerDoF  ||
         maxChi2 > m_maxChi2XProjection ) {
      pc.removeHit( *worst );
      track.hits().erase( worst );
      if ( pc.nbDifferent() < m_minXSize || (hasStereo && pc.nbDifferent() < 7)  ) {
        
        // -- > Debug --------
        if ( m_debug ) {
          info() << "  == Not enough layers with hits" << endmsg;
          printTrack( track );
        }
        // -- < Debug --------

        return false;
      }
      doFit = true;
    }
  }

  // -- > Debug --------
  if ( m_debug ) {
    info() << "  -- End fitXProjection -- " << endmsg;
    printTrack( track );
  }
  // -- < Debug --------

  return true;
}

//=========================================================================
//  Add hits on empty X layers, and refit if something was added
//=========================================================================
bool PrForwardTool::addHitsOnEmptyXLayers ( PrForwardTrack& track, bool fullFit  ) {
  
  PrPlaneCounter planeCount;
  planeCount.set( track.hits().begin(), track.hits().end(), false );
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
    float bestChi2 = 1.e9;
    float minX = xPred - xWindow;
    float maxX = xPred + xWindow;
  
    // -- Use a binary search to find the lower bound of the range of x values
    PrHits::const_iterator itH = std::lower_bound(  zone->hits().begin(),  
                                                    zone->hits().end(), 
                                                    minX, 
                                                    lowerBoundX() );
    

    PrHits::const_iterator itEnd = zone->hits().end();
    for ( ; itEnd != itH; ++itH ) {
      if ( (*itH)->x() > maxX ) break;
      float chi2 = track.chi2( *itH );
      if ( chi2 < bestChi2 ) {
        bestChi2 = chi2;
        best = *itH;
      }
    }
    if ( NULL != best ) {
      
      // -- > Debug --------
      if ( m_debug ) {
        info() << format( "AddHitOnEmptyXLayer:    chi2%8.2f", bestChi2 );
        printHit( best, " ");
      }
      // -- < Debug --------

      m_geoTool->xAtReferencePlane( track, best );
      best->setUsed( false );
      track.addHit( best );
      added = true;
    }
  }
  if ( !added ) return true;
  if ( fullFit ) {
    return fitXProjection( track );
  }
  fastLinearFit( track );
  return true;
}
//=========================================================================
//  Collect all hits in the stereo planes compatible with the track
//=========================================================================
void PrForwardTool::collectStereoHits ( PrForwardTrack& track ) {
  
  // -- > Debug --------
  if ( m_debug ) info() << "== Collecte stereo hits. wanted ones: " << endmsg;
  // -- < Debug --------

  m_stereoHits.clear();
  unsigned int step=2;

  if(m_useTFix) step=1;
  for ( unsigned int zoneNumber = track.zone(); m_hitManager->nbZones() > zoneNumber; zoneNumber += step ) {
    PrHitZone* zone = m_hitManager->zone(zoneNumber);
    if ( zone->isX() ) continue;  // exclude X zones
    if ( zone->hits().empty()) continue;
    float zZone = zone->z();
    float yZone = track.y( zZone );
    zZone = zone->z( yZone );  // Correct for dzDy
    float xPred  = track.x( zZone );
    

    // -- > Debug --------
    if ( m_debug ) {
      for ( PrHits::iterator itH = zone->hits().begin(); zone->hits().end() != itH; ++itH ) {
        if ( matchKey( *itH ) ) {
          float tmp = (*itH)->coord();
          float dx = (*itH)->x( yZone ) - xPred;
          (*itH)->setCoord( dx );
          printHit( *itH, "-- " );
          (*itH)->setCoord( tmp );
        }
      }
    }
    // -- < Debug --------

    float dxTol = m_tolY + m_tolYSlope * ( fabs( xPred - track.xFromVelo( zZone ) ) + fabs( yZone ) );

    // -- Use a binary search to find the lower bound of the range of x values
    // -- This takes the y value into account
    PrHits::const_iterator itH = std::lower_bound(  zone->hits().begin(),  
                                                    zone->hits().end(), 
                                                    -dxTol + xPred, 
                                                    lowerBoundXYZone(yZone) );


    PrHits::const_iterator itEnd = zone->hits().end();
    for ( ; itEnd != itH; ++itH ) {
      
      float dx = (*itH)->x( yZone ) - xPred ;
      if ( dx >   dxTol ) break;
      
      if (yZone < (*itH)->yMin() - m_yTol_UVsearch || yZone > (*itH)->yMax() + m_yTol_UVsearch)continue;

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
  
  float lastMeanDy = 1.e9;
  PrHits bestStereoHits;
  PrForwardTrack original( track );
  PrForwardTrack bestTrack( track );

  PrHits::iterator beginRange = m_stereoHits.begin() - 1;
  while ( beginRange < m_stereoHits.end() - m_minStereoHits ) {
    ++beginRange;
  
    // -- > Debug --------
    if ( m_debug ) {
      info() << " stereo start at ";
      printHit( *beginRange );
    }
    // -- > Debug --------

    PrPlaneCounter pc;
    PrHits::iterator endRange   = beginRange;
    float sumCoord = 0.;
    
    while( pc.nbDifferent() < m_minStereoHits ||
           (*endRange)->coord() < (*(endRange-1))->coord() + m_minYGap ) {
      pc.addHit( *endRange );
      sumCoord += (*endRange)->coord();
      ++endRange;
      if ( endRange == m_stereoHits.end() ) break;
    }
    
    bool changed = true;
    
    while( changed ) {
      float averageCoord = sumCoord / (endRange-beginRange);
      changed = false;
      if ( pc.nbInPlane( (*beginRange)->planeCode() ) > 1 &&   // remove first if not single and farest from mean
           (averageCoord-(*beginRange)->coord() > (*(endRange-1))->coord() - averageCoord ) ) {
        pc.removeHit( *beginRange );
        sumCoord -= (*(beginRange++))->coord();
        changed = true;
      } else if ( endRange != m_stereoHits.end() ) {
        if ( pc.nbInPlane( (*endRange)->planeCode() ) == 0 ) {  // next is new -> add
          pc.addHit( *endRange );
          sumCoord += (*(endRange++))->coord();
          changed = true;
        } else if ( averageCoord - (*beginRange)->coord() >  // next decreases the range size
                    (*endRange)->coord() - averageCoord ) {
          if ( (*endRange)->planeCode() == (*beginRange)->planeCode() ||
               pc.nbInPlane( (*beginRange)->planeCode() ) >1 ) {
            pc.addHit( *endRange );
            pc.removeHit( *beginRange );
            sumCoord = sumCoord + (*(endRange++))->coord() - (*(beginRange++))->coord();
            changed = true;
          } else {
            pc.addHit( *endRange );
            sumCoord += (*(endRange++))->coord();
            changed = true;
          }
        }
      }
    }
    
    // -- > Debug --------
    if ( m_debug ) {
      info() << "Selected stereo range from " << endmsg;
      printHit( *beginRange );
      printHit( *(endRange-1) );
    }
    // -- < Debug --------
    
    PrHits stereoHits( beginRange, endRange );
    if ( stereoHits.size() < bestStereoHits.size() ) continue;  // fit only if it can be better!

    track = original;
    float maxChi2 = 1.e9;
    bool parabola = false;
    //== Fit a line
    float zRef = m_geoTool->zReference();

    float tolYMag = m_tolYMag + m_tolYMagSlope * fabs( track.xStraight( zRef )-track.xFromVelo( zRef ) );
    float wMag   = 1./(tolYMag * tolYMag );

    while ( maxChi2 > m_maxChi2Stereo ) {
      float zMag  = m_geoTool->zMagnet( track );
      float dyMag = track.yStraight( zMag ) - track.yFromVelo( zMag );
      zMag -= m_geoTool->zReference();
      float s0   = wMag;
      float sz   = wMag * zMag;
      float sz2  = wMag * zMag * zMag;
      float sd   = wMag * dyMag;
      float sdz  = wMag * dyMag * zMag;
      
      PrHits::const_iterator itEnd = stereoHits.end();

      if ( parabola ) {
        float sz2m = 0.;
        float sz3  = 0.;
        float sz4  = 0.;
        float sdz2 = 0.;
        
        for ( PrHits::iterator itH = stereoHits.begin(); itEnd != itH; ++itH ) {
          float d = -track.deltaY( *itH );
          float w = (*itH)->w();
          float z = (*itH)->z() - m_geoTool->zReference();
          s0   += w;
          sz   += w * z;
          sz2m += w * z * z;
          sz2  += w * z * z;
          sz3  += w * z * z * z;
          sz4  += w * z * z * z * z;
          sd   += w * d;
          sdz  += w * d * z;
          sdz2 += w * d * z * z;
        }
        float b1 = sz  * sz   - s0  * sz2;
        float c1 = sz2m* sz   - s0  * sz3;
        float d1 = sd  * sz   - s0  * sdz;
        float b2 = sz2 * sz2m - sz * sz3;
        float c2 = sz3 * sz2m - sz * sz4;
        float d2 = sdz * sz2m - sz * sdz2;

        float den = (b1 * c2 - b2 * c1 );
        if(den == 0)return false;
        float db  = (d1 * c2 - d2 * c1 ) / den;
        float dc  = (d2 * b1 - d1 * b2 ) / den;
        float da  = ( sd - db * sz - dc * sz2 ) / s0;

        track.updateParameters( 0., 0., 0., 0., da, db, dc );
        
        // -- > Debug --------
        if ( m_debug ) info() << "Parabola: " << da << " " << db << " " << dc << endmsg;
        // -- < Debug --------

      } else {

        
        for ( PrHits::iterator itH = stereoHits.begin(); itEnd != itH; ++itH ) {
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
        if(den == 0)return false;
        float da  = (sd * sz2 - sdz * sz ) / den;
        float db  = (sdz * s0 - sd  * sz ) / den;
        track.updateParameters( 0., 0., 0., 0., da, db);

        // -- > Debug --------
        if ( m_debug ) info() << "Linear: " << da << " " << db  << endmsg;
        // -- < Debug --------
      }

      PrHits::iterator worst = stereoHits.end();
      maxChi2 = 0.;
      for ( PrHits::iterator itH = stereoHits.begin(); itEnd != itH; ++itH ) {
        float chi2 = track.chi2( *itH );
        if ( chi2 > maxChi2 ) {
          maxChi2 = chi2;
          worst   = itH;
        }
      }
      
      // -- > Debug --------
      if ( m_debug ) printHitsForTrack( stereoHits, track );
      // -- < Debug --------

      if ( maxChi2 < m_maxChi2StereoLinear && !parabola ) {
        parabola = true;
        maxChi2 = 1.e9;
        continue;
      }

      if ( maxChi2 > m_maxChi2Stereo ) {
        pc.removeHit( *worst );
        if ( pc.nbDifferent() < m_minStereoHits ) {
          
          // -- > Debug --------
          if ( m_debug ) info() << "-- not enough different planes after removing worst: " << pc.nbDifferent()
                                << " for " << m_minStereoHits << " --" << endmsg;
          // -- < Debug --------

          break;
        }
        stereoHits.erase( worst );
        continue;
      }

      //== Store average dy
      float meanDy = 0.;
      itEnd = stereoHits.end();
      for ( PrHits::iterator itH = stereoHits.begin(); itEnd != itH; ++itH ) {
        meanDy += (*itH)->coord();
      }
      meanDy = fabs( meanDy / stereoHits.size() );

      float dX = track.xFromVelo( m_geoTool->zReference() ) - track.x( m_geoTool->zReference() );

      meanDy = meanDy / ( 1. + m_tolYSlope / m_tolY * fabs( dX ) );

      zMag  = m_geoTool->zMagnet( track );
      dyMag = track.yStraight( zMag ) - track.yFromVelo( zMag );
      float chi2AtMagnet = dyMag * dyMag * wMag;
      track.setMeanDy( meanDy );
      track.setChi2AtMagnet( chi2AtMagnet );

      if ( meanDy < lastMeanDy &&
           stereoHits.size() >= bestStereoHits.size() &&
           chi2AtMagnet < m_maxYChi2AtMagnet ) {
        
        // -- > Debug --------
        if ( m_debug ) {
          info() << "************ Store candidate, nStereo " << stereoHits.size() << " meanDy " << meanDy
                 << " chi2AtMagnet " << chi2AtMagnet << endmsg;
        }
        // -- < Debug --------
        
        lastMeanDy = meanDy;
        bestStereoHits = stereoHits;
        bestTrack = track;
      }
    }
  }
  if ( bestStereoHits.size() > 0 ) {
    m_stereoHits = bestStereoHits;
    track = bestTrack;
    return true;
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
    tmp->setHistory( LHCb::Track::PrForward );
    tmp->addToAncestors( (*itT).track() );
    double qOverP  = m_geoTool->qOverP( *itT );
    double errQop2 = 0.1 * 0.1 * qOverP * qOverP;

    std::vector<LHCb::State*>::const_iterator iStateEnd =  tmp->states().end();
    for ( std::vector<LHCb::State*>::const_iterator iState = tmp->states().begin();
          iState != iStateEnd ; ++iState ){
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

    PrHits::iterator iHitEnd = (*itT).hits().end();
    for ( PrHits::iterator itH = (*itT).hits().begin(); iHitEnd != itH; ++itH ) {
      tmp->addToLhcbIDs( (*itH)->id() );
    }
    tmp->setChi2PerDoF( (*itT).chi2PerDoF() );
    tmp->setNDoF(       (*itT).nDoF() );
    tmp->addInfo( LHCb::Track::PatQuality, (*itT).quality() );

    //ADD UT hits on track
    if ( NULL != m_addUTHitsTool ) {
      StatusCode sc = m_addUTHitsTool->addUTHits( *tmp );
      if (sc.isFailure()){
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug()<<" Failure in adding UT hits to track"<<endmsg;
      }
    }

    result->insert( tmp );
  
    // -- > Debug --------
    if ( m_debug ) info() << "Store track " << (*(result->end()-1))->key()
                          << " quality " << (*(result->end()-1))->info( LHCb::Track::PatQuality, 0. ) << endmsg;
    // -- < Debug --------
  }
  
}
//=============================================================================

//  Check if a stereo Hit matches the X hit from collectAllXHits
//=========================================================================
bool PrForwardTool::matchStereoHit( PrForwardTrack& track, PrHits::iterator Xhit, unsigned int uvZoneNumber ) {
  
  if ( 0 == m_hitManager->hits( uvZoneNumber ).size() ) return false;
  PrHitZone* zoneUv = m_hitManager->zone(uvZoneNumber);
  float zMag = m_geoTool->zMagnet( track );
  float zZone = (*Xhit)->z();//zZone is the same as in collectAllXHits
  float zRatio = ( zoneUv->z() - zMag ) / ( zZone - zMag );
  float xInZone = track.xFromVelo( zZone );
  float yInZone = track.yFromVelo( zZone );
  float xCentral = xInZone + yInZone * zoneUv->dxDy();
  float xInUv = track.xFromVelo( zoneUv->z() );
  
  float dx    = yInZone * zoneUv->dxDy();//x correction from rotation by stereo angle
  float xPredUv = xInUv + ( (*Xhit)->x() - xInZone) * zRatio - dx;//predicted hit in UV-layer
  float maxDx   = m_tolY + ( fabs( (*Xhit)->x() -xCentral ) + fabs( yInZone ) ) * m_tolYSlope;
  
  bool hasUV = false;
  for(const PrHit* UVHit : zoneUv->hits()){
    if(UVHit->x() < xPredUv - maxDx)continue;
    if(UVHit->x() > xPredUv + maxDx)continue;
    if(yInZone < (UVHit->yMin() - m_yTol_UVsearch))continue;
    if(yInZone > (UVHit->yMax() + m_yTol_UVsearch))continue;
    hasUV = true;
    break;
  }
  return hasUV;
}

//=========================================================================
