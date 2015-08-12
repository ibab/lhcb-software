// Include files 

// from Gaudi
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
// 2014-11-24 : Renato Quagliani Modification
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrSeedingXLayers )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrSeedingXLayers::PrSeedingXLayers( const std::string& name,
                                    ISvcLocator* pSvcLocator)
: 
#ifdef DEBUG_HISTO
GaudiHistoAlg ( name , pSvcLocator ),
#else 
  GaudiAlgorithm (name, pSvcLocator ),
#endif
  m_hitManager(nullptr),
  m_geoTool(nullptr),
  m_debugTool(nullptr),
  m_timerTool(nullptr)
{
  declareProperty( "InputName",           m_inputName            = LHCb::TrackLocation::Forward );
  declareProperty( "OutputName",          m_outputName           = LHCb::TrackLocation::Seed    );
  declareProperty( "HitManagerName",      m_hitManagerName       = "PrFTHitManager"             );
  declareProperty( "DecodeData",          m_decodeData           = false                        );
  declareProperty( "XOnly",               m_xOnly                = false                        );
  declareProperty( "MaxChi2InTrack",      m_maxChi2InTrack       = 5.5                          ); 
  declareProperty( "TolXInf",             m_tolXInf              = 0.5 * Gaudi::Units::mm       );
  declareProperty( "TolXSup",             m_tolXSup              = 8.0 * Gaudi::Units::mm       );
  declareProperty( "MinXPlanes",          m_minXPlanes           = 5                            );
  declareProperty( "MaxChi2PerDoF",       m_maxChi2PerDoF        = 4.0                          ); 
  declareProperty( "MaxParabolaSeedHits", m_maxParabolaSeedHits  = 4                            );
  declareProperty( "TolTyOffset",         m_tolTyOffset          = 0.002                        ); 
  declareProperty( "TolTySlope",          m_tolTySlope           = 0.015                        );
  declareProperty( "MaxIpAtZero",         m_maxIpAtZero          = 5000.                        );
  declareProperty( "BestDist",            m_bestDist             = 10.0                         );//added
  declareProperty( "UseFix",              m_useFix               = true                         );//added 
  declareProperty( "TolTriangle",         m_tolTriangle          = 10.* Gaudi::Units::mm        );//added 
  declareProperty( "TolYOffset",          m_tolYOffset           = 100000.* Gaudi::Units::mm    );//added 
  declareProperty( "TolXStereo",          m_tolXStereo           = 2700.* Gaudi::Units::mm      );//added
  declareProperty( "TolCoord" ,           m_coord                = 0.005                        );//added
  // Parameters for debugging
  declareProperty( "DebugToolName",       m_debugToolName         = ""                          );
  declareProperty( "WantedKey",           m_wantedKey             = -100                        );
  declareProperty( "TimingMeasurement",   m_doTiming              = false                       );
  declareProperty( "PrintSettings",       m_printSettings         = false                       );
}
//=============================================================================
// Destructor
//=============================================================================
PrSeedingXLayers::~PrSeedingXLayers() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrSeedingXLayers::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be exectued first
  if ( sc.isFailure() ) return sc;  

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hitManager = tool<PrHitManager>( m_hitManagerName );
  m_hitManager->buildGeometry();
  m_geoTool = tool<PrGeometryTool>("PrGeometryTool");

  m_debugTool   = 0;
  if ( "" != m_debugToolName ) {
    m_debugTool = tool<IPrDebugTool>( m_debugToolName );
    info()<<"Debug tool "<<m_debugToolName<<" loaded."<<endmsg;
  } else {
    m_wantedKey = -100;  //no debug
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
  
  if( m_decodeData ) info() << "Will decode the FT clusters!" << endmsg;
  
  // -- Print the settings of this algorithm in a readable way
  if( m_printSettings){
    
    info() << "========================================"             << endmsg
           << " InputName            = " <<  m_inputName             << endmsg
           << " OutputName           = " <<  m_outputName            << endmsg
           << " HitManagerName       = " <<  m_hitManagerName        << endmsg
           << " DecodeData           = " <<  m_decodeData            << endmsg
           << " XOnly                = " <<  m_xOnly                 << endmsg
           << " MaxChi2InTrack       = " <<  m_maxChi2InTrack        << endmsg
           << " TolXInf              = " <<  m_tolXInf               << endmsg
           << " TolXSup              = " <<  m_tolXSup               << endmsg
           << " MinXPlanes           = " <<  m_minXPlanes            << endmsg
           << " MaxChi2PerDoF        = " <<  m_maxChi2PerDoF         << endmsg
           << " MaxParabolaSeedHits  = " <<  m_maxParabolaSeedHits   << endmsg
           << " TolTyOffset          = " <<  m_tolTyOffset           << endmsg
           << " TolTySlope           = " <<  m_tolTySlope            << endmsg
           << " UseFix               = " <<  m_useFix                << endmsg
           << " TolTriangle          = " <<  m_tolTriangle           << endmsg
           << " TolXStereo           = " <<  m_tolXStereo            << endmsg
           << " TolYOffset           = " <<  m_tolYOffset            << endmsg
           << " MaxIpAtZero          = " <<  m_maxIpAtZero           << endmsg
           << " DebugToolName        = " <<  m_debugToolName         << endmsg
           << " WantedKey            = " <<  m_wantedKey             << endmsg
           << " TimingMeasurement    = " <<  m_doTiming              << endmsg
           << "========================================"             << endmsg;
  }
#ifdef DEBUG_HISTO
  setHistoTopDir("FT/");
#endif

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

  // -- This is only needed if the seeding is the first algorithm using the FT
  // -- As the Forward normally runs first, it's off per default
  if( m_decodeData ) m_hitManager->decodeData();
  
  int multiplicity =0;
    
  
  // -- All Hits Used = False
  for ( unsigned int zone = 0; m_hitManager->nbZones() > zone; ++zone ) {
    
    for ( PrHits::const_iterator itH = m_hitManager->hits( zone ).begin();
          m_hitManager->hits( zone ).end() != itH; ++itH ) {
      (*itH)->setUsed( false );
      multiplicity++;    
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
  
  //==========================================================================
  // Extract the seed part from the forward tracks and remove Hits associated.
  //==========================================================================
  
  if ( "" != m_inputName ) {
    // -- sort hits according to LHCbID (single generic channel identifier)
    for(int i = 0; i < 24; i++){
      PrHitZone* zone = m_hitManager->zone(i);
      std::stable_sort( zone->hits().begin(),  zone->hits().end(), compLHCbID());
    }
    // ------------------------------------------
    
    //Import forward tracks
    LHCb::Tracks* forward = get<LHCb::Tracks>( m_inputName );
    
    //Loop over forward tracks container
    for ( LHCb::Tracks::iterator itT = forward->begin(); forward->end() != itT; ++itT ) {
      
      //Vector of LHCbID
      std::vector<LHCb::LHCbID> ids;
      ids.reserve(20);
      
      // Loop over LHCbIDs of the forward track
      for ( std::vector<LHCb::LHCbID>::const_iterator itId = (*itT)->lhcbIDs().begin();
            (*itT)->lhcbIDs().end() != itId; ++itId ) {
        if ( (*itId).isFT() && (*itId).ftID().layer() < 12 ) {
          LHCb::FTChannelID ftId =(*itId).ftID();
          int zoneNb = 2 * ftId.layer() + ftId.mat(); //zones top are even (0, 2, 4, ....,22)  and zones bottom are odd
          //Load the PrHitZone
          PrHitZone* zone = m_hitManager->zone(zoneNb);
          
          // -- The hits are sorted according to LHCbID, we can therefore use a lower bound to speed up the search
          
          PrHits::iterator itH = std::lower_bound(  zone->hits().begin(),  zone->hits().begin(), *itId, lowerBoundLHCbID() );
          
          for ( ; zone->hits().end() != itH; ++itH ) {
            if( *itId < (*itH)->id() ) break;
            if ( (*itH)->id() == *itId ) (*itH)->setUsed( true );
          }
          ids.push_back( *itId );
        }
      }
      
      
      // Forward output loaded to TTrack container with History flag
      LHCb::Track* seed = new LHCb::Track;
      seed->setLhcbIDs( ids );
      seed->setType( LHCb::Track::Ttrack );
      seed->setHistory( LHCb::Track::PrSeeding );
      seed->setPatRecStatus( LHCb::Track::PatRecIDs );
      seed->addToStates( (*itT)->closestState( 9000. ) );
      result->insert( seed );
    }
    
    // -- sort hits according to x for each zone
    for(int i = 0; i < 24; i++){
      PrHitZone* zone = m_hitManager->zone(i);
      std::stable_sort( zone->hits().begin(),  zone->hits().end(), compX());
    }
  }
  
  

  m_trackCandidates.clear();
  if ( m_doTiming ) {
    m_timerTool->stop( m_timeFromForward );
  }

  // -- Loop through lower and upper half
  for ( unsigned int part= 0; 2 > part; ++part ){
    //Start timing
    if ( m_doTiming ) {
      m_timerTool->start( m_timeXProjection);
    }
    //1st -- X layers hits & projections
    findXProjections2( part );
    
    
    if ( m_doTiming ) {
      m_timerTool->stop( m_timeXProjection);
      m_timerTool->start( m_timeStereo);
    }
    //Add Stereo
    if ( ! m_xOnly ) addStereo2( part );
    if ( m_doTiming ) {
      m_timerTool->stop( m_timeStereo);
    }
  }

  if ( m_doTiming ) {
    m_timerTool->start( m_timeFinal);
  }
  // Convert to LHCb Tracks
  makeLHCbTracks( result );

  if ( m_doTiming ) {
    m_timerTool->stop( m_timeFinal);
    float tot = m_timerTool->stop( m_timeTotal );
    info() << format( "                                            Time %8.3f ms", tot )
           << endmsg;
#ifdef DEBUG_HISTO
      plot2D(multiplicity,tot,"timing","timing",0,10000,0,1000,100,100);
#endif   
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
    if( fabs(den) < 1e-9 ) return false;
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
      
#ifdef DEBUG_HISTO
      plot(chi2,"TrackChi2_X","TrackChi2_X",0.,10,100);
#endif    
            
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
#ifdef DEBUG_HISTO                                                                                                                                                                     
    plot((*itT).chi2PerDoF(),"Track Chi2/DOF","Track Chi2/DOF",-0.5,30,300);                                                             
    plot((*itT).nDoF(), "Track nDoF", "Track nDoF",-0.5,40 ,300);
    //plot(fZone->hits().size(), "NumberOfHitsInFirstZone","NumberOfHitsInFirstZone", 0., 600., 100);        
    //plot(lZone->hits().size(), "NumberOfHitsInLastZone","NumberOfHitsInLastZone", 0., 600., 100);
#endif                                                                                                                                                                                        
    
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
void PrSeedingXLayers::printHit ( const PrHit* hit, std::string title ) {
  info() << "  " << title << " "
         << format( " Plane%3d zone%2d z0 %8.2f x0 %8.2f  size%2d charge%3d coord %8.3f used%2d ",
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
//=========================================================================
// modified method to find the x projections
//=========================================================================
void PrSeedingXLayers::findXProjections2( unsigned int part ){
  m_xCandidates.clear();
  //part =0 should be y<0
  //part =1 should be y>0
  for ( unsigned int iCase = 0 ; 3 > iCase ; ++iCase ) {
    //Case 0
    int firstZone = part; // firstZone = 1st X-Layer in T1
    int lastZone  = 22 + part; // lastZone = 2nd X-Layer in T3
    //Case 1
    if ( 1 == iCase ) firstZone = part + 6; // firstZone = 2nd X-Layer in T1
    //Case 3
    if ( 2 == iCase ) lastZone  = 16 + part; //lastZone = 1st X-layer in T3

    if (m_debugTool){
      info()<<"Currently in case "<<iCase<<" . First zone "<<firstZone<<" . Last zone "<<lastZone<<endmsg;
     }
      
     //Load of Hits on zones
    PrHitZone* fZone = m_hitManager->zone( firstZone );
    PrHitZone* lZone = m_hitManager->zone( lastZone  );
    PrHits& fHits = fZone->hits();
    PrHits& lHits = lZone->hits();

    float zRatio =  lZone->z(0.) / fZone->z(0.);
    
#ifdef DEBUG_HISTO
    plot(zRatio,"zRatio","zRatio",0.,2,100);
    plot(fZone->hits().size(), "NumberOfHitsInFirstZone","NumberOfHitsInFirstZone", 0., 600., 100);
    plot(lZone->hits().size(), "NumberOfHitsInLastZone","NumberOfHitsInLastZone", 0., 600., 100);
#endif
    
    
    
    std::vector<PrHitZone*> xZones; 
    xZones.reserve(12);
    for ( int kk = firstZone+2; lastZone > kk ; kk += 2 ) {
      if ( m_hitManager->zone( kk )->isX() ) xZones.push_back( m_hitManager->zone(kk) );
    }
    //Last zone Hits iterator begin
    PrHits::iterator itLBeg = lHits.begin();
    // -- Define the iterators for the "in-between" layers
    std::vector< PrHits::iterator > iterators;
    iterators.reserve(24);

    //Loop over first Zone hits to define last Zone hit search window
    for ( PrHits::iterator itF = fHits.begin(); fHits.end() != itF; ++itF ) {
      if ( 0 != iCase && (*itF)->isUsed() ) continue;
      float minXl = (*itF)->x() * zRatio - m_maxIpAtZero * ( zRatio - 1 );
      float maxXl = (*itF)->x() * zRatio + m_maxIpAtZero * ( zRatio - 1 );
      
#ifdef DEBUG_HISTO
      plot(minXl, "minXl", "minXl", -6000, 6000, 100); 
      plot(maxXl, "maxXl", "maxXl", -6000, 6000, 100);
#endif     
       itLBeg = std::lower_bound( lHits.begin(), lHits.end(), minXl, lowerBoundX() );
       
       
      //Was in Old Version
      //while ( itLBeg != lHits.end() && (*itLBeg)->x() < minXl ) 
      //{
      //  ++itLBeg;
      //  if ( lHits.end() == itLBeg ) break;
      //}
      
      PrHits::iterator itL = itLBeg;
      //Loop in last zone hits up to the maxXl or end of Hit list
      while ( itL != lHits.end() && (*itL)->x() < maxXl ) {
        //find hits in the last zone search window [minXl,maxXl]
        if ( 0 != iCase && (*itL)->isUsed()) {
          ++itL;
          continue; 
        }
        // tx = coefficient of the line connecting the hits found in first and last zones
        // x0 = offset of the line connecting the hits found in fist and last zones
        float tx = ((*itL)->x() - (*itF)->x()) / (lZone->z() - fZone->z() );
        float x0 = (*itF)->x() - (*itF)->z() * tx;
#ifdef DEBUG_HISTO
	plot(tx, "tx", "tx", -1.,1., 100 );
	plot(x0, "x0", "x0", 0., 6000., 100.);
#endif
        PrHits parabolaSeedHits;
        parabolaSeedHits.clear();
        parabolaSeedHits.reserve(5);
        
        // -- loop over first two x zones
        // --------------------------------------------------------------------------------
        unsigned int counter = 0;
        bool skip = true;
        if( iCase != 0 ) skip = false;

        
        for ( std::vector<PrHitZone*>::iterator itZ = xZones.begin(); xZones.end() != itZ; ++itZ ) {
          
          ++counter;
          // -- to make sure, in case = 0, only x layers of the 2nd T station are used
          // I think this can be written shorter, basically it selects only zones in T2 
          if(skip){
            skip = false;
            continue;
          }
          if( iCase == 0){
            if(counter > 3) break;
          }else{
            if(counter > 2) break;
          }
          float xP   = x0 + (*itZ)->z() * tx;
          float xMax = xP + 2*fabs(tx)*m_tolXSup + 1.5;
          float xMin = xP - m_tolXInf;
          
#ifdef DEBUG_HISTO
	  plot(xP, "xP_x0pos", "xP_x0pos", -10000., 10000., 100); 
	  plot(xMax, "xMax_x0pos", "xMax_x0pos", -10000., 10000., 100);
	  plot(xMin, "xMin_x0pos", "xMax_x0pos", -10000., 10000., 100);
#endif
	  
       
          if ( x0 < 0 ) {
            xMin = xP - 2*fabs(tx)*m_tolXSup - 1.5;
            xMax = xP + m_tolXInf;
            
#ifdef DEBUG_HISTO 
              plot(xP, "xP_x0neg", "xP_x0neg", -10000., 10000., 100); 
              plot(xMax, "xMax_x0neg", "xMax_x0neg", -10000., 10000., 100);
              plot(xMin, "xMin_x0neg", "xMax_x0neg", -10000., 10000., 100);
#endif
	      
          }
          
          if ( x0 > 0 && tx >0 )
            debug()<<"x0 =  "<<x0<<" , tx = "<<tx<<" [ "<<xMin<<" , "<<xP<<" , "<<xMax<<" ]"<<endmsg;
          
          
          PrHits::iterator itH = std::lower_bound( (*itZ)->hits().begin(), (*itZ)->hits().end(), xMin, lowerBoundX() );
          for ( ; (*itZ)->hits().end() != itH; ++itH ) {
            if ( (*itH)->x() < xMin ) continue;
            if ( (*itH)->x() > xMax ) break;
            //Sascha ???? if ( ((*itH)->x() > xMax) || ((*itH)->x()< xMin) ) break;
            parabolaSeedHits.push_back( *itH );
          }
        }
        
        //--------------------------------------------------------------------
        
        debug() << "We have " << parabolaSeedHits.size() << " hits to seed the parabolas" << endmsg;
        
        std::vector<PrHits> xHitsLists;
        xHitsLists.clear();
        
	
        // -- float xP   = x0 + (*itZ)->z() * tx;
        // -- Alles klar, Herr Kommissar?
        // -- The power of Lambda functions!
        // -- Idea is to reduce ghosts in very busy events and prefer the high momentum tracks
        // -- For this, the seedHits are storted according to their distance to the linear extrapolation
        // -- so that the ones with the least distance can be chosen in the end

        // what happens if no sorting and more parabola seed hits
        std::stable_sort( parabolaSeedHits.begin(), parabolaSeedHits.end(),
                          [x0,tx](const PrHit* lhs, const PrHit* rhs)
                          ->bool{return fabs(lhs->x() - (x0+lhs->z()*tx)) <  fabs(rhs->x() - (x0+rhs->z()*tx)); });
        
        // test this
        unsigned int maxParabolaSeedHits = m_maxParabolaSeedHits;
        if( parabolaSeedHits.size() < m_maxParabolaSeedHits){
          maxParabolaSeedHits = parabolaSeedHits.size();
        }

        for(unsigned int i = 0; i < maxParabolaSeedHits; ++i){

          float a = 0;
          float b = 0;
          float c = 0;

          PrHits xHits;
          xHits.clear();

          // -- formula is: x = a*dz*dz + b*dz + c = x, with dz = z - zRef
          solveParabola( *itF, parabolaSeedHits[i], *itL, a, b, c);

          debug() << "parabola equation: x = " << a << "*z^2 + " << b << "*z + " << c << endmsg;

          for ( std::vector<PrHitZone*>::iterator itZ = xZones.begin(); xZones.end() != itZ; ++itZ ) {
	    
            float dz = (*itZ)->z() - m_geoTool->zReference();
            float xAtZ = a*dz*dz + b*dz + c; //parabola extracted value of x at the z of the inter-layer
	    
            float xP   = x0 + (*itZ)->z() * tx;  //Linear Prediction
            float xMax = xAtZ + fabs(tx)*2.0 + 0.5; //max ...put outside the params?
            float xMin = xAtZ - fabs(tx)*2.0 - 0.5; //min ...put outside the params?


            debug() << "x prediction (linear): " << xP <<  "x prediction (parabola): " << xAtZ << endmsg;

            // -- Only use one hit per layer, which is closest to the parabola in the range of m_bestDist!
            PrHit* best = nullptr;
            float bestDist = m_bestDist;

            PrHits::iterator itH = std::lower_bound( (*itZ)->hits().begin(), (*itZ)->hits().end(), xMin, lowerBoundX() );
            for (; (*itZ)->hits().end() != itH; ++itH ) 
            {
              //this is probably useless
              if ( (*itH)->x() < xMin ) continue;
              if ( (*itH)->x() > xMax ) break;
              if( fabs((*itH)->x() - xAtZ ) < bestDist)
              {
                bestDist = fabs((*itH)->x() - xAtZ );
                best = *itH;
              }
            }
            if( best != nullptr) 
            {
              xHits.push_back( best );
            }
          } //end Loop in Beetween Zones
          
          xHits.push_back( *itF ); //add also to xHits the First and last layer one
          xHits.push_back( *itL );
          
          if (xHits.size()<5) continue;
          
          // shifted before continue due to debug reasons
          std::stable_sort(xHits.begin(), xHits.end(), compX());
          if( xHits.size() <  m_minXPlanes)  continue;

          bool isEqual = false;
	  //xHitsLists = vector of<PrHits> 
          for( PrHits hits : xHitsLists){
            if( hits == xHits ){
              isEqual = true;
              break;
            }
          }
          if( !isEqual ) xHitsLists.push_back( xHits );
        }
        
        debug() << "xHitsLists size before removing duplicates: " << xHitsLists.size() << endmsg;

        // -- remove duplicates
	
        if( xHitsLists.size() > 2){
          std::stable_sort( xHitsLists.begin(), xHitsLists.end() );
          xHitsLists.erase( std::unique(xHitsLists.begin(), xHitsLists.end()), xHitsLists.end());
        }
        debug() << "xHitsLists size after removing duplicates: " << xHitsLists.size() << endmsg;

        for( PrHits xHits : xHitsLists ){
	  
          PrSeedTrack temp( part, m_geoTool->zReference(), xHits );

          bool OK = fitTrack( temp ); //1 fit
          //bool OK = fitTrack(temp )
          while ( !OK ) {
            OK = removeWorstAndRefit( temp );
            debug()<<"Remove worse and refit"<<endmsg;
            debug()<<"***"<<endmsg;
            }
          setChi2( temp );
          // ---------------------------------------
          

          // interesting cut! check this 6 hard coded?
          float maxChi2 = m_maxChi2PerDoF + 6*tx*tx;
          
#ifdef DEBUG_HISTO
	  plot(maxChi2,"maxChi2","maxChi2",0.,10,100);          
#endif    
	  
          if ( OK &&
               temp.hits().size() >= m_minXPlanes &&
               temp.chi2PerDoF()  < maxChi2   ) {
            if ( temp.hits().size() == 6 ) {
              for ( PrHits::iterator itH = temp.hits().begin(); temp.hits().end() != itH; ++ itH) {
		// what happens if this is not done??? and why is it done?
                // only x layers, it brings 3% in efficiency but plus 30% clone rate and 20% ghost rate
                // prefers tracks with 6 hits or?
                (*itH)->setUsed( true );
              }
            }
            m_xCandidates.push_back( temp );
          }
          // -------------------------------------
        } 
        ++itL;
      } //end loop last zone
    } //end loop first zone 
  } //end loop case

  //order Track candidates in increasing size
  std::stable_sort( m_xCandidates.begin(), m_xCandidates.end(), PrSeedTrack::GreaterBySize() );
  
  //====================================================================
  // Remove clones, i.e. share more than 2 hits (why not 3?)
  //====================================================================
  for ( PrSeedTracks::iterator itT1 = m_xCandidates.begin(); m_xCandidates.end() !=itT1; ++itT1 ) {
    if ( !(*itT1).valid() ) continue;
    if ( (*itT1).hits().size() != 6 ) {
      int nUsed = 0;
      
      //count number of used hits in same 
      for ( PrHits::iterator itH = (*itT1).hits().begin(); (*itT1).hits().end() != itH; ++ itH) {
        if ( (*itH)->isUsed()) ++nUsed;
      }
      if ( 1 < nUsed ) 
      {
        (*itT1).setValid( false );
        continue;
      }
    }

    for ( PrSeedTracks::iterator itT2 = itT1 + 1; m_xCandidates.end() !=itT2; ++itT2 ) {
      if ( !(*itT2).valid() ) continue;
      int nCommon = 0;
      PrHits::iterator itH1 = (*itT1).hits().begin();
      PrHits::iterator itH2 = (*itT2).hits().begin();

      PrHits::iterator itEnd1 = (*itT1).hits().end();
      PrHits::iterator itEnd2 = (*itT2).hits().end();

      while ( itH1 != itEnd1 && itH2 != itEnd2 ) {
        if ( (*itH1)->id() == (*itH2)->id() ) {
          ++nCommon;
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
    if ( m_xOnly && (*itT1).valid() ) m_trackCandidates.push_back( *itT1 );
  }
}


int PrSeedingXLayers::matchXCandidate( PrSeedTrack& track ){
  setKey(-1);
  std::map<int,std::vector <int> >keys;
  info()<<"MAtch candidate"<<endmsg;
  int nHits = track.hits().size();
  // for (auto& hit :  track.hits() ){
  //   int key = m_debugTool->getKey( hit->id() );
  //   if ( key>0 )
  //     keys[key].push_back(1);
  // }
  
  unsigned int nMost = 0;
  int key = -121;
  for (auto& element :  keys ){
    if (nMost<element.second.size()) {
      key = element.first;
      nMost = element.second.size();
    }
    
  }
  double fraction =  nMost/nHits;
  if (fraction>0.65) return key;
  else return -201;
}


//=========================================================================
// Modified version of adding the stereo layers
//=========================================================================
void PrSeedingXLayers::addStereo2( unsigned int part ) {
  PrSeedTracks xProjections;
  for ( PrSeedTracks::iterator itT1 = m_xCandidates.begin(); m_xCandidates.end() !=itT1; ++itT1 ) {
    if ( !(*itT1).valid() ) continue;
    xProjections.push_back( *itT1 );
  }
  unsigned int firstZone = part + 2;
  unsigned int lastZone  = part + 22;
  //when the fixing is active firstZone is always T1 1st uv layer and lastZone is last uv layer
  if (m_useFix){
    firstZone = 2;
    lastZone  = 22;
  }
  for ( PrSeedTracks::iterator itT = xProjections.begin(); xProjections.end() !=itT; ++itT ){
    PrHits myStereo;
    myStereo.reserve(30);
    // loop over all zones to improve efficiency of stereo hits & triangle fixing
    const unsigned int stepSize = m_useFix ? 1 : 2;
    for ( unsigned int kk = firstZone; lastZone > kk ; kk+= stepSize ) {
      if ( m_hitManager->zone(kk)->isX() ) 
      {
        continue;
      }
      
      float dxDy = m_hitManager->zone(kk)->dxDy();
      float zPlane = m_hitManager->zone(kk)->z(); //to be sure of it
      float xPred = (*itT).x(m_hitManager->zone(kk)->z() ); //Predicted X
     
#ifdef DEBUG_HISTO
      plot2D(xPred,zPlane,"xPred_ZPlane","xPred_ZPlane",-3000.,3000.,6000.,12000.,1000,1000);
#endif
      
      
      
      // increase a bit??
      // as x is at 0, can't you just take either xMin or xMax???
      float xMin = xPred + m_tolXStereo* dxDy;
      float xMax = xPred - m_tolXStereo* dxDy;
      //simmetric search windows in the X axis
      
      if ( xMin > xMax ) {
        float tmp = xMax;
        xMax = xMin;
        xMin = tmp;
      }

#ifdef DEBUG_HISTO
      plot2D(xMax-xMin,xPred,"xMax-xMin Vs xPred","xMax-xMin Vs xPred",-3000.,3000.,-3000.,3000.,1000,1000);
#endif 
      
      
      PrHits::iterator itH = m_hitManager->zone(kk)->hits().begin();
      
      for ( ;
            m_hitManager->zone(kk)->hits().end() != itH; ++itH ) {
        if ( (*itH)->x() < xMin ) continue;
        if ( (*itH)->x() > xMax ) continue;
        
         if (m_useFix){
	    
	   const double yOnTrack = ( xPred - (*itH)->x() ) / dxDy;
	    
	   if ( yOnTrack  < (*itH)->yMin() - m_tolTriangle  ) {
	     continue;
	   }
	   if ( yOnTrack  > (*itH)->yMax() + m_tolTriangle  ) {
	     continue; 
            }
	   if ( (0 == part) && (yOnTrack < -m_tolYOffset) ) {
	     continue;
	   }
            if ( (1 == part) && (yOnTrack > m_tolYOffset) ) { 
	      continue; // bottom
            }  
	 }
	 
	 
	 (*itH)->setCoord( ((*itH)->x() - xPred) / dxDy  / zPlane );
	 
	
	 if ( 1 == part && (*itH)->coord() < -m_coord ) {
          continue;
	 }
	 if ( 0 == part && (*itH)->coord() >  m_coord ) {
	   continue;
	 }
	 myStereo.push_back( *itH );
      }
    }
    std::stable_sort( myStereo.begin(), myStereo.end(), PrHit::LowerByCoord() );
    PrPlaneCounter plCount;
    //Save position of this x candidate, for later use
    unsigned int firstSpace = m_trackCandidates.size();
    PrHits::iterator itBeg = myStereo.begin();
    PrHits::iterator itEnd = itBeg + 5; //Why 5?
    //PrHits::iterator itEnd = itBeg + 4;

    while ( itEnd < myStereo.end() ) {
      // why should this increase, somehow momentum is missing or?
      float tolTy = m_tolTyOffset + m_tolTySlope * fabs( (*itBeg)->coord() );
      // Take the next 4 hits and check if inside a given tolerance      
      if ( (*(itEnd-1))->coord() - (*itBeg)->coord() < tolTy ) {
        while( itEnd+1 < myStereo.end() &&
               (*itEnd)->coord() - (*itBeg)->coord() < tolTy ) {
          ++itEnd;
        }

        // count planes
        plCount.set( itBeg, itEnd );    
        
        if ( 4 < plCount.nbDifferent() ) {
          PrSeedTrack temp( *itT );
          for ( PrHits::iterator itH = itBeg; itEnd != itH; ++itH ) {
            temp.addHit( *itH );
          }
          bool ok = fitTrack( temp );
	  
          ok = fitTrack( temp );
          ok = fitTrack( temp );

          while ( !ok && temp.hits().size() > 10 ) {
            ok = removeWorstAndRefit( temp );
          }
	  
          if ( ok ) {
            setChi2( temp );
           float maxChi2 = m_maxChi2PerDoF + 6*temp.xSlope(9000)*temp.xSlope(9000);
           
	   //number of hits hard coded??? More than in PatSeeding? 
	   if ( temp.hits().size() > 9 ||
                 temp.chi2PerDoF() < maxChi2 ) {
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
            //equal size, take the one with the better chi2
          } else if ( m_trackCandidates[kk].chi2() < m_trackCandidates[ll].chi2() ) {
            m_trackCandidates[ll].setValid( false );
          } else {
            m_trackCandidates[kk].setValid( false );
          }
        }
      }
    }//loop candidates removal
    
  }
}


//=========================================================================
// Solve parabola using Cramer's rule
//========================================================================
void PrSeedingXLayers::solveParabola(const PrHit* hit1, const PrHit* hit2, const PrHit* hit3, float& a, float& b, float& c){
 
  const float x3 = hit3->x();
  
  //New Parabola Method
  const float z1_PB = hit1->z() - hit3->z();
  const float z2_PB = hit2->z() - hit3->z();
  const float x1_PB = hit1->x() - hit3->x();
  const float x2_PB = hit2->x() - hit3->x();
  
  const float det_PB = z1_PB*z2_PB*(z1_PB-z2_PB);
  
  if( fabs(det_PB) < 1e-8 ){
    a = 0.0;
    b = 0.0;
    c = 0.0;
    return;
    
  }

  a=0; b=0;c=0;
  a = (z2_PB*x1_PB-z1_PB*x2_PB)/det_PB;
  b = (-z2_PB*z2_PB*x1_PB+z1_PB*z1_PB*x2_PB)/det_PB;
  const float z3_PB = hit3->z() - m_geoTool->zReference();
  

  c = x3 + a * z3_PB * z3_PB - b *z3_PB;
  b -= 2 * a * z3_PB;

}

