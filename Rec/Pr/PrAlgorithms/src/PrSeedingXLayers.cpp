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
// 2016-01-16 : Sevda Esen Modification 
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
  declareProperty( "MinXPlanes",          m_minXPlanes           = 4                            );
  declareProperty( "MinSPlanes",          m_minSPlanes           = 4                            );
  declareProperty( "MinTPlanes",          m_minTPlanes           = 9                            );
  declareProperty( "MaxChi2PerDoF",       m_maxChi2PerDoF        = 4.0                          ); 
  declareProperty( "MaxParabolaSeedHits", m_maxParabolaSeedHits  = 5                            );
  declareProperty( "TolTyOffset",         m_tolTyOffset          = 0.002                        ); 
  declareProperty( "TolTySlope",          m_tolTySlope           = 0.015                        );
  declareProperty( "MaxIpAtZero",         m_maxIpAtZero          = 5000.                        );
  declareProperty( "MaxIpAtZeroIN",       m_maxIpAtZeroIN        = 3000.                        );
  declareProperty( "BestDist",            m_bestDist             = 10.0                         );//added
  declareProperty( "UseFix",              m_useFix               = true                         );//added 
  declareProperty( "TolTriangle",         m_tolTriangle          = 10.* Gaudi::Units::mm        );//added 
  declareProperty( "TolYOffset",          m_tolYOffset           = 100000.* Gaudi::Units::mm    );//added 
  declareProperty( "TolXStereo",          m_tolXStereo           = 2700.* Gaudi::Units::mm      );//added
  declareProperty( "TolXStereoIN",        m_tolXStereoIN         = 150.* Gaudi::Units::mm       );//added
  declareProperty( "TolXStereoTriangle",  m_tolXStereoTriangle   = 150.* Gaudi::Units::mm       );//added
  declareProperty( "TolCoord" ,           m_coord                = 0.005                        );//added
  // Parameters for debugging
  declareProperty( "DebugToolName",       m_debugToolName         = ""                          );
  declareProperty( "WantedKey",           m_wantedKey             = -100                        );
  declareProperty( "TimingMeasurement",   m_doTiming              = false                       );
  declareProperty( "PrintSettings",       m_printSettings         = false                       );
  //Parameter Track Model                                                                                                                              
  declareProperty( "UseCubicCorrection",  m_useCubic              = true                        );
  declareProperty( "dRatio",              m_dRatio                = -0.000262                   ); 
  declareProperty( "SlopeCorr",           m_SlopeCorr             = true                       ); 
  declareProperty( "UseCorrPosition",     m_useCorrPos            = true                        ); 
  declareProperty( "maxChi2HitsHigh",     m_maxChi2HitFullFitHigh = 5.5                         );
  declareProperty( "maxChi2HitsMed",      m_maxChi2HitFullFitMed  = 2.5                         );
  declareProperty( "maxChi2HitsLow",      m_maxChi2HitFullFitLow  = 2.0                         );
  declareProperty( "maxYatZeroLow",       m_maxY0Low              = 1000.                       );
  declareProperty( "maxYatzRefLow",       m_maxYZrefLow           = 2000.                       );
  declareProperty( "maxChi2HitsXHigh",    m_maxChi2HitsXHigh      = 5.5                         );
  declareProperty( "maxChi2HitsXMed",     m_maxChi2HitsXMed       = 3.0                         );
  declareProperty( "maxChi2HitsXLow",     m_maxChi2HitsXLow       = 2.0                         );
  declareProperty( "maxChi2HitsYHigh",    m_maxChi2HitsYHigh      = 1000.                        );
  declareProperty( "maxChi2HitsYMed",     m_maxChi2HitsYMed       = 100.                        );
  declareProperty( "maxChi2HitsYLow",     m_maxChi2HitsYLow       = 50.                        );
  declareProperty( "RadiusHole",          m_radiusHole            = 87.                         );
  declareProperty( "RemoveHole",          m_removeHole            = true                        );

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
  m_zReference = m_geoTool->zReference();


  for ( unsigned int iCase = 0 ; 3 > iCase ; ++iCase ) {
    int firstZone = 0; // firstZone = 1st X-Layer in T1
    int lastZone  = 22; // lastZone = 2nd X-Layer in T3
    if ( 1 == iCase ) firstZone = 6; // firstZone = 2nd X-Layer in T1
    if ( 2 == iCase ) lastZone  = 16; //lastZone = 1st X-layer in T3

    PrHitZone* fZone = m_hitManager->zone( firstZone );
    PrHitZone* lZone = m_hitManager->zone( lastZone  );
    m_invDeltaz[iCase]= 1./ (lZone->z() - fZone->z() );
    m_zRatio[iCase]   =  lZone->z(0.) / fZone->z(0.);  
  }

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
      ids.reserve(12);
      
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
  
  
  if ( m_doTiming ) {
    m_timerTool->stop( m_timeFromForward );
  }

  m_trackCandidates.clear();
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
//  Fit the X track, return OK if fit sucecssfull
//=========================================================================
bool PrSeedingXLayers::fitXProjection(PrSeedTrack& track ){
  if (msgLevel(MSG::DEBUG)) debug()<<"Fitting"<<endmsg;

  float mat[6];
  float rhs[3];
  const float zRef = m_zReference;
  // if(m_useCubic) track.setdRatio(m_dRatio);                                                                                                             
  for(int loop = 0;3>loop;++loop)
    {
      std::fill(mat,mat+6,0.);
      std::fill(rhs,rhs+3,0.);
      for( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
	const float dRatio = track.dRatio();
	float w = (*itH)->w();//squared                                                                                                                     
	if(m_SlopeCorr){
	  track.setSlopeCorr( true);
	  float Corr = std::cos( track.xSlope( (*itH)->z()));
	  if(loop ==0) Corr = 1.;
	  w = w*(Corr*Corr);
	}
	const float dz= 0.001*((*itH)->z() - zRef);
	const float deta = dz*dz*(1. + dRatio*dz);
	const float dist = track.distance( *itH );
	if (msgLevel(MSG::DEBUG)) debug()<<"Loop \t"<<loop<<"\n Distance From Hit \t"<<dist<<endmsg;
	mat[0]+= w;
	mat[1]+= w * dz;   mat[2]+= w * dz * dz;
	mat[3]+= w * deta; mat[4]+= w * dz * deta;  mat[5]+= w * deta * deta;
	rhs[0]+= w * dist;
	rhs[1]+= w * dist * dz;
	rhs[2]+= w * dist * deta;
      }
      ROOT::Math::CholeskyDecomp<float,3> decomp(mat);
      if(!decomp){
	return false;
      }
      //Solve linear system  
      decomp.Solve(rhs);
      rhs[1]*=1.e-3;
      rhs[2]*=1.e-6;
      if (msgLevel(MSG::DEBUG)) debug()<<"Loop \t"<<loop<<"\n a = \t"<<rhs[0]<<"\n b = \t"<<rhs[1]<<"\n c = \t"<<rhs[2]<<endmsg;
      // protect against unreasonable track parameter corrections
      // (check that out) 
      if(std::fabs(rhs[0]) > 1.e4 || std::fabs(rhs[1]) > 5. ||
	 std::fabs(rhs[2]) > 1.e-3 ) return false;
      //Small corrections
      track.updateParameters(rhs[0],rhs[1],rhs[2],0.,0.);
      if(loop==0 && m_useCubic){
	track.setdRatio(m_dRatio);
      }
      //Put back later faster maybe 
      if(loop >0 && std::abs(rhs[0]) < 5e-5 && std::abs(rhs[1]) < 5e-8 &&
	 std::abs(rhs[2]) < 5e-11){
	loop=2;
      }
      if(loop==2){
	float maxChi2 = 0.;
	PrHits::iterator worst = track.hits().begin();

	for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) 
	  {
	    float chi2_onHit = track.chi2( *itH );
	    if ( chi2_onHit > maxChi2 ){
	      maxChi2 = chi2_onHit;
	      worst=itH;
	    }
	  }
	
	if      ( track.hits().size()==6  && m_maxChi2HitsXHigh > maxChi2 ) return true;
	else if ( track.hits().size()==5  && m_maxChi2HitsXMed > maxChi2 ) return true;
	else if ( track.hits().size()==4  && m_maxChi2HitsXLow > maxChi2 ) return true;
	else if (track.hits().size() > m_minXPlanes){
	  track.hits().erase( worst );
	  loop=1;
	}
	else return false;
      }
    }
  return false;
}

//=========================================================================
//  Fit the track, return OK if fit sucecssfull
//=========================================================================                                     
bool PrSeedingXLayers::fitSimultaneouslyXY( PrSeedTrack& track, int refit){

  float mat[15];
  float rhs[5];

  const float zRef = m_zReference;// m_geoTool->zReference();                                                                                             
  for ( int loop = refit; 3 > loop ; ++loop ){
    if(loop > 0  && m_useCubic && m_useCorrPos){
      float RadiusPosition = std::sqrt((0.0005*0.0005*track.ax()*track.ax() + 0.001*0.001*track.y(zRef)*track.y(zRef)));
      float dRatioPos = -1.*(2.633e-4 - 3.59957e-6*RadiusPosition + 4.7312e-5*RadiusPosition*RadiusPosition);
      track.setdRatio(dRatioPos);
    }
    const float dRatio = track.dRatio();

    std::fill(mat,mat+15,0.);
    std::fill(rhs,rhs+5,0.);
    for( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
      float w = (*itH)->w();
      if( m_SlopeCorr && loop >0){
        const float Corr = std::cos(track.xSlope( (*itH)->z()) );
        track.setSlopeCorr( true);
        w = w*(Corr*Corr);
      }
      const float dxdy = (*itH)->dxDy();
      const float yOnTrack = track.yOnTrack( (*itH) ) ;
      const float dz = 0.001*((*itH)->z( yOnTrack ) - zRef);
      const float deta = dz*dz*(1. + dz*dRatio);
      const float wdz = w * dz;
      const float weta = w * deta;
      const float wdxdy = w * dxdy;
      const float wdxdydz = wdxdy * dz;
      const float dist = track.distance(*itH);
      mat[0] += w;
      mat[1] += wdz;  mat[2] += wdz * dz;
      mat[3] += weta; mat[4] += weta * dz; mat[5] += weta * deta;
      mat[6] -= wdxdy;mat[7] -= wdxdydz;   mat[8] -= wdxdy * deta;  mat[9] += wdxdy * dxdy;
      mat[10] -= wdxdydz; mat[11] -= wdxdydz * dz;mat[12] -= wdxdydz * deta;mat[13] += wdxdydz * dxdy; mat[14] += wdxdydz * dz * dxdy;

      // fill right hand side                                                                                                                             
      rhs[0] += w * dist;
      rhs[1] += wdz * dist;
      rhs[2] += weta * dist;
      rhs[3] -= wdxdy * dist;
      rhs[4] -= wdxdydz * dist;
    }//Loop over Hits to fill the matrix                                                                                                                   
    ROOT::Math::CholeskyDecomp<float, 5> decomp(mat);
    if (!decomp) return false;
    decomp.Solve(rhs);
    rhs[1]*=1.e-3;
    rhs[2]*=1.e-6;
    rhs[4]*=1.e-3;
    rhs[3]-=rhs[4]*zRef;
    if( loop >0 &&(std::fabs(rhs[0]) > 1e4 || std::fabs(rhs[1]) > 5. ||
                   std::fabs(rhs[2]) > 1e-3 || std::fabs(rhs[3]) > 1e4 || std::fabs(rhs[4]) > 1.)) return false;
    track.updateParameters(rhs[0],rhs[1],rhs[2],rhs[3],rhs[4]);
    if(loop==1){
      float maxChi2 =-1.;
      for( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
      	float chi2_onHit = track.chi2( *itH);
	if ( chi2_onHit > maxChi2 ){
	  maxChi2 = chi2_onHit;
	}
      }//Set Max Chi2DoF
      
      track.setMaxChi2(maxChi2);


      if( (track.hits().size()>10) && maxChi2<m_maxChi2HitFullFitHigh) return true;
      else if( (track.hits().size()==10) && maxChi2<m_maxChi2HitFullFitMed) return true;
      else if( track.hits().size()<10 && maxChi2<m_maxChi2HitFullFitLow ) return true;
    }
    else if(loop==2){
      float maxChi2 =-1.;
      PrHits::iterator worst = track.hits().begin();

      for( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
      	float chi2_onHit = track.chi2( *itH);
	if ( chi2_onHit > maxChi2 ){
	  maxChi2 = chi2_onHit;
	  worst=itH;
	}
      }//Set Max Chi2DoF

      track.setMaxChi2(maxChi2);
      if( (track.hits().size()>10) && maxChi2<m_maxChi2HitFullFitHigh) return true;
      else if( (track.hits().size()==10) && maxChi2<m_maxChi2HitFullFitMed) return true;
      else if( track.hits().size()<10 && maxChi2<m_maxChi2HitFullFitLow ) return true;
      else if(track.hits().size() > m_minTPlanes){
	track.hits().erase( worst );
	if( (*worst)->isX()){
	  track.setnXnY( track.nx()-1, track.ny());
	}else{
	  track.setnXnY( track.nx(), track.ny()-1);
	}
	if( track.nx() <m_minXPlanes || track.ny() <m_minSPlanes ) return false;
	else loop=1;
      }
      else return false;
    }



  }
  return false;
}


//=========================================================================
//  Remove the worst hit and refit.
//=========================================================================
bool PrSeedingXLayers::removeWorstAndRefit ( PrSeedTrack& track, bool xonlyFit ) {

  float maxChi2 = -1.;
  
  PrHits::iterator worst = track.hits().begin();
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
    float chi2 = track.chi2( *itH );
    if ( chi2 > maxChi2 ) {
      maxChi2 = chi2;
      worst = itH;
    }
  }
  track.hits().erase( worst );

  if (xonlyFit)  return fitXProjection(track); 
  else{
    if( (*worst)->isX()){
      track.setnXnY( track.nx()-1, track.ny());
    }else{
      track.setnXnY( track.nx(), track.ny()-1);
    }
    if( track.nx() <m_minXPlanes || track.ny() <m_minSPlanes ) return false;
    return fitSimultaneouslyXY(track, 1);
  }
  return false;
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
    float qOverP = m_geoTool->qOverP( *itT );
    LHCb::State tState;
    float z = StateParameters::ZEndT;
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

    debug()<<"Currently in case "<<iCase<<" . First zone "<<firstZone<<" . Last zone "<<lastZone<<endmsg;
      
     //Load of Hits on zones
    PrHitZone* fZone = m_hitManager->zone( firstZone );
    PrHitZone* lZone = m_hitManager->zone( lastZone  );
    float invDeltaz =  m_invDeltaz[iCase]; //1./ (lZone->z() - fZone->z() );
    float zRatio    =  m_zRatio[iCase];// lZone->z(0.) / fZone->z(0.);

    PrHits& fHits = fZone->hits();
    PrHits& lHits = lZone->hits();


    
#ifdef DEBUG_HISTO
    plot(zRatio,"zRatio","zRatio",0.,2,100);
    plot(fZone->hits().size(), "NumberOfHitsInFirstZone","NumberOfHitsInFirstZone", 0., 600., 100);
    plot(lZone->hits().size(), "NumberOfHitsInLastZone","NumberOfHitsInLastZone", 0., 600., 100);
#endif
    
    
    
    std::vector<PrHitZone*> xZones; 
    xZones.reserve(10);
    for ( int kk = firstZone+2; lastZone > kk ; kk += 2 ) {
      if ( m_hitManager->zone( kk )->isX() ) xZones.push_back( m_hitManager->zone(kk) );
    }


    //Last zone Hits iterator begin
    PrHits::iterator itLBeg = lHits.begin();
    // -- Define the iterators for the "in-between" layers
    //std::vector< PrHits::iterator > iterators;
    //iterators.reserve(24);

    //Loop over first Zone hits to define last Zone hit search window
    for ( PrHits::iterator itF = fHits.begin(); fHits.end() != itF; ++itF ) {
      //if ( 0 != iCase && (*itF)->isUsed() ) continue;
      float minXl = (*itF)->x() * zRatio - m_maxIpAtZero * ( zRatio - 1 );
      float maxXl = (*itF)->x() * zRatio + m_maxIpAtZero * ( zRatio - 1 );
      
      if( (*itF)->id().ftID().module()>9 ){
        minXl = (*itF)->x() * zRatio - m_maxIpAtZeroIN * ( zRatio - 1 );
        maxXl = (*itF)->x() * zRatio + m_maxIpAtZeroIN * ( zRatio - 1 );
      }
      
      
#ifdef DEBUG_HISTO
      plot(minXl, "minXl", "minXl", -6000, 6000, 100); 
      plot(maxXl, "maxXl", "maxXl", -6000, 6000, 100);
#endif     
      itLBeg = std::lower_bound( lHits.begin(), lHits.end(), minXl, lowerBoundX() );
      
      PrHits::iterator itL = itLBeg;
      //Loop in last zone hits up to the maxXl or end of Hit list
      while ( itL != lHits.end() && (*itL)->x() < maxXl ) {

        // tx = coefficient of the line connecting the hits found in first and last zones
        // x0 = offset of the line connecting the hits found in fist and last zones
        float tx = ((*itL)->x() - (*itF)->x()) * invDeltaz ; /// (lZone->z() - fZone->z()); 
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
        for ( std::vector<PrHitZone*>::iterator itZ = xZones.begin(); xZones.end() != itZ; ++itZ ) {

	  if( (*itZ)->planeCode() == 3 ) continue;
          if( (*itZ)->planeCode() == 8 ) break;

          float xP   = x0 + (*itZ)->z() * tx ;
          float xMax = xP + 2*fabs(tx)*m_tolXSup + 1.5 ;
          float xMin = xP - m_tolXInf ;
          
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
	    
	    if ( 0 != iCase){
              if ( (*itF)->isUsed() and  (*itH)->isUsed() ) continue;
              if ( (*itL)->isUsed() and  (*itH)->isUsed() ) continue;
            }

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
	    
	    
            float dz = (*itZ)->z() - m_zReference;
            float xAtZ = a*dz*dz + b*dz + c; //parabola extracted value of x at the z of the inter-layer
	    if(m_useCubic){
              xAtZ= a * dz * dz * (1. + m_dRatio* dz) + b * dz + c; 
	    }

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
          
          if( xHits.size() <  m_minXPlanes)  continue;          
          if( xHits.size()==4 &&  innerMod(xHits) > 1 ) continue;

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
	
        if( xHitsLists.size() > 1){
          std::stable_sort( xHitsLists.begin(), xHitsLists.end() );
          xHitsLists.erase( std::unique(xHitsLists.begin(), xHitsLists.end()), xHitsLists.end());
        }
        debug() << "xHitsLists size after removing duplicates: " << xHitsLists.size() << endmsg;

        for( PrHits xHits : xHitsLists ){
	  
          PrSeedTrack temp( part, m_zReference, xHits );
	  
          bool OK = fitXProjection( temp ); //1 fit
          //bool OK = fitTrack(temp )
          while ( !OK && temp.hits().size()>m_minXPlanes) {
            OK = removeWorstAndRefit( temp , true);
            debug()<<"Remove worse and refit"<<endmsg;
            debug()<<"***"<<endmsg;
            }
          if( temp.hits().size()==4 &&  innerMod(temp) > 1 ) continue;
          setChi2( temp );
          // ---------------------------------------
          //doesn't seem to help
	  //if(OK && temp.hits().size()==4) OK=addEmptyXLayers(part, temp, OK);

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
	    std::sort( temp.hits().begin(),  temp.hits().end(), compLHCbID());
            m_xCandidates.push_back( temp );
	  }
          // -------------------------------------
        } 
        ++itL;
      } //end loop last zone
    } //end loop first zone 
  } //end loop case

  //sort by size and chi2, good ones come first
 std::sort( m_xCandidates.begin(), m_xCandidates.end(), PrSeedTrack::LowerBySize() );                   
 
 for ( PrSeedTracks::iterator itT1 = m_xCandidates.begin(); m_xCandidates.end() !=itT1; ++itT1 ) {  
   if ( !(*itT1).valid() ) continue;
   for ( PrSeedTracks::iterator itT2 = itT1 + 1; m_xCandidates.end() !=itT2; ++itT2 ) {
     if ( !(*itT2).valid() ) continue;             
     
     PrHits::iterator itH1 = (*itT1).hits().begin();
     PrHits::iterator itH2 = (*itT2).hits().begin();
     PrHits::iterator itEnd1 = (*itT1).hits().end();
     PrHits::iterator itEnd2 = (*itT2).hits().end();
     int Compare = (*itT1).hits().size()+(*itT2).hits().size();
     int Comparediff = (*itT1).hits().size()-(*itT2).hits().size();
     unsigned int maxCommon = 3;
     if( Compare <10 )	maxCommon = 1;	//5 vs 4
     else if( Compare ==10 && Comparediff==0 ) maxCommon = 2; //5 vs 5 
     else if( Compare ==10 )                   maxCommon = 1; //6 vs 4 
     
     unsigned int nCommon = 0;
     while( itH1 != itEnd1 && itH2 != itEnd2 ){
       if((*itH1)->id() == (*itH2)->id()){
	 ++nCommon;
	 
	 ++itH1;
	 ++itH2;
       }
       else if( (*itH1)->id() < (*itH2)->id() ){
	 ++itH1;
       }
       else{
	 ++itH2;
       }
       if( nCommon >=maxCommon )  break;
     }
     
     if(nCommon >= maxCommon){
       (*itT2).setValid(false);
     }
   }
 }
 
 if(m_xOnly){
   for( PrSeedTracks::iterator itT = m_xCandidates.begin() ; m_xCandidates.end()!= itT; ++itT){
      if((*itT).valid()) m_trackCandidates.push_back( (*itT));
   }
 }
 
}

//=========================================================================

//=========================================================================
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
  float fraction =  nMost/nHits;
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
    myStereo.reserve(6);
    // loop over all zones to improve efficiency of stereo hits & triangle fixing 
    
    const unsigned int stepSize = m_useFix ? 1 : 2;
    for ( unsigned int kk = firstZone; lastZone > kk ; kk+= stepSize ) {
      
      bool wrongSide= kk%2;
      if(part==1) wrongSide= !wrongSide; 
      
      PrHitZone* uvZone = m_hitManager->zone(kk);
      if ( uvZone->isX() )   continue;
	
      float dxDy = uvZone->dxDy();
      float zPlane = uvZone->z(); //to be sure of it  
      float invdxDy = 1./dxDy;
      float invzPlane = 1./zPlane;
      float xPred = (*itT).x(uvZone->z() ); //Predicted X                                            

#ifdef DEBUG_HISTO
      plot2D(xPred,zPlane,"xPred_ZPlane","xPred_ZPlane",-3000.,3000.,6000.,12000.,1000,1000);
#endif

      float xMin; 
      float xMax;

      if(!wrongSide){
	xMin = xPred + m_tolXStereo* dxDy;
	xMax = xPred - m_tolXStereoIN * dxDy;
	
	if(part==0) {
	  xMax = xPred - m_tolXStereo* dxDy;
	  xMin = xPred + m_tolXStereoIN * dxDy;
	}
	if ( xMin > xMax ) {
	  float tmp = xMax;
	  xMax = xMin;
	  xMin = tmp;
	}
      }else{
	//regions for triangle
	xMin = xPred + m_tolXStereoTriangle * dxDy;
	xMax = xPred - m_tolXStereoTriangle * dxDy;
	
	if ( xMin > xMax ) {
	  float tmp = xMax;
	  xMax = xMin;
	  xMin = tmp;
	}
      }


#ifdef DEBUG_HISTO
      plot2D(xMax-xMin,xPred,"xMax-xMin Vs xPred","xMax-xMin Vs xPred",-3000.,3000.,-3000.,3000.,1000,1000);
#endif

      PrHits::iterator itH= uvZone->hits().begin();
      PrHits::iterator itEnd= uvZone->hits().end();
      
      itH   = std::lower_bound( uvZone->hits().begin(), uvZone->hits().end(), xMin, lowerBoundX() );

      for ( ;  itEnd != itH; ++itH ) {

	if( (*itH)->x() > xMax) break;
	const float yOnTrack = ( xPred - (*itH)->x() )  * invdxDy;

	if(m_removeHole){
	  double radius =  yOnTrack*yOnTrack + xPred*xPred;
	  if(  radius < m_radiusHole*m_radiusHole) continue;
	}
	
	if ( yOnTrack  < (*itH)->yMin() - m_tolTriangle )  continue;
	if ( yOnTrack  > (*itH)->yMax() + m_tolTriangle )  continue;
	
	(*itH)->setCoord( ((*itH)->x() - xPred) * invdxDy  * invzPlane );
	
	if ( 1 == part && (*itH)->coord() < -m_coord )  continue;
	if ( 0 == part && (*itH)->coord() >  m_coord )  continue;
	
	myStereo.push_back( *itH );
      }
    }

    std::stable_sort( myStereo.begin(), myStereo.end(), PrHit::LowerByCoord() );
    
    PrPlaneCounter plCount;
    std::vector<PrPlaneCounter> plCounters;
    //Save position of this x candidate, for later use                                                                                                                                                          
    unsigned int firstSpace = m_trackCandidates.size();
    
    PrHits::iterator itBeg = myStereo.begin();
    PrHits::iterator itEnd = itBeg + m_minSPlanes  ; //Why 5?

    
    while ( itEnd < myStereo.end() ) {
      const float tolTy = m_tolTyOffset + m_tolTySlope * std::abs( (*itBeg)->coord() );
      
      if ( (*(itEnd-1))->coord() - (*itBeg)->coord() < tolTy ) {
        while( itEnd+1 < myStereo.end() &&
               (*itEnd)->coord() - (*itBeg)->coord() < tolTy ) {
          ++itEnd;
        }
      }
      plCount.set( itBeg, itEnd );       

      if ( m_minSPlanes <= plCount.nbDifferent()) plCounters.push_back(plCount);
      
      ++itBeg;
      itEnd = itBeg + m_minSPlanes; 
    }
    
    std::sort( plCounters.begin(), plCounters.end(), PrPlaneCounter::LowerBySize() );
    
    unsigned int found=0;
    for( PrPlaneCounter plane : plCounters){
      PrSeedTrack temp( *itT );
      
      int nused=0;
      for ( PrHits::const_iterator itH = plane.first(); plane.last() != itH; ++itH ) {
	if((*itT).hits().size() ==6 or  !(*itH)->isUsed() )temp.addHit( *itH );
	//if( (*itH)->isUsed()) nused++;
      }
	  
      temp.setnXnY( (*itT).hits().size(), temp.hits().size()-(*itT).hits().size());
	  
      bool first_ok=true;
      //if( temp.hits().size() < m_minTPlanes ) first_ok=false;
      //else if( temp.hits().size()<10 &&  innerMod(temp) > 2 ) first_ok=false;
      if( temp.nx() < m_minXPlanes || temp.ny() < m_minSPlanes )first_ok=false;
      //else if((*itT).hits().size() < 6 && nused>0 ) first_ok=false;

	  
      if(!first_ok) continue;
      float ay=0;
      float by=0;
      bool second_ok = fitYLine( *itT, plane.first(), plane.last(), ay, by );
      if(!second_ok) continue;
      temp.setYParam(ay, by);
      
      bool ok = fitSimultaneouslyXY( temp , 0);
      
      while ( !ok && temp.hits().size() > m_minTPlanes ) {
	ok = removeWorstAndRefit( temp, false );
      }
	  
      if( temp.hits().size()<12) ok=addEmptyLayers(part, temp, ok);

      if( ok && (temp.hits().size() > m_minTPlanes ||  (temp.hits().size()<10 &&  innerMod(temp) < 3 ))){
	  
	setChi2( temp );
	float maxChi2 = m_maxChi2PerDoF + 6*temp.xSlope(9000)*temp.xSlope(9000);

	if(temp.chi2PerDoF() < maxChi2 ){
	  
	  std::sort( temp.hits().begin(),  temp.hits().end(), compLHCbID());
	  m_trackCandidates.push_back( temp );
	  ++found;
	  
	  if(temp.hits().size()==12) {
	    for ( PrHits::iterator itH = temp.hits().begin(); temp.hits().end() != itH; ++ itH) {
	      (*itH)->setUsed( true );
	    }
	    break;
	  }
	}
      }
      if (found >1) break;
    }
	    
    
    //start the clone wars
    if ( m_trackCandidates.size() > firstSpace+1 ) {
      //sort by size and chi2, first one is the good one
      std::sort( m_trackCandidates.begin()+firstSpace , m_trackCandidates.end(), PrSeedTrack::LowerBySize() );
      for ( unsigned int kk = firstSpace+1; m_trackCandidates.size() > kk ; ++kk ) {
	m_trackCandidates[kk].setValid(false) ;
      }
    }

  }//x projections
}

//=========================================================================
// Solve parabola using Cramer's rule
//========================================================================
void PrSeedingXLayers::solveParabola(const PrHit* hit1, const PrHit* hit2, const PrHit* hit3, float& a, float& b, float& c){
 
  const float z1 = hit1->z() - m_zReference;//->zReference();       
  const float z2 = hit2->z() - m_zReference;//m_geoTool->zReference();        
  const float z3 = hit3->z() - m_zReference;//m_geoTool->zReference();                                                                                     
  const float x1 = hit1->x();
  const float x2 = hit2->x();
  const float x3 = hit3->x();

  const float corrZ1 = 1.+m_dRatio*z1;
  const float corrZ2 = 1.+m_dRatio*z2;
  const float corrZ3 = 1.+m_dRatio*z3;
  const float det = (z1*z1)*corrZ1*z2 + z1*(z3*z3)*corrZ3 + (z2*z2)*corrZ2*z3 - z2*(z3*z3)*corrZ3 - z1*(z2*z2)*corrZ2 - z3*(z1*z1)*corrZ1;
  if( std::fabs(det) < 1e-8 )
    {
      a = 0.0;
      b = 0.0;
      c = 0.0;
      return;
    }
  const float recdet = 1./det;
  const float det1 = (x1)*z2 + z1*(x3) + (x2)*z3 - z2*(x3) - z1*(x2) - z3*(x1);
  const float det2 = (z1*z1)*corrZ1*x2 + x1*(z3*z3)*corrZ3 + (z2*z2)*corrZ2*x3 - x2*(z3*z3)*corrZ3 - x1*(z2*z2)*corrZ2 - x3*(z1*z1)*corrZ1;
  const float det3 = (z1*z1)*corrZ1*z2*x3 + z1*(z3*z3)*corrZ3*x2 + (z2*z2)*corrZ2*z3*x1  
    - z2*(z3*z3)*corrZ3*x1 - z1*(z2*z2)*corrZ2*x3 - z3*(z1*z1)*corrZ1* x2;

  a = recdet*det1;
  b = recdet*det2;
  c = recdet*det3;
  /*
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
  */
}

//=============================================================== 
//--- number of  inner modules a track passes
//===============================================================
int PrSeedingXLayers::innerMod( const PrSeedTrack& track  ) {
  unsigned int ninner=0;
  for ( PrHits::const_iterator itF = track.hits().begin(); track.hits().end() != itF; ++itF ) {
    if( (*itF)->id().ftID().module()>9 ) ninner++;
  }
  return ninner;
}

int PrSeedingXLayers::innerMod( PrHits thesehits ) {
  unsigned int ninner=0;
  for ( PrHits::iterator itF = thesehits.begin(); thesehits.end() != itF; ++itF ) {
    if( (*itF)->id().ftID().module()>9 ) ninner++;
  }
  return ninner;
}



//========================================================================= 
// adding hits in empty layers 
//========================================================================= 
bool PrSeedingXLayers::addEmptyXLayers(  unsigned int part,  PrSeedTrack& track , bool ok ) {
  
  PrHits myEmptyLayers;
  myEmptyLayers.reserve(2);

  PrHits myEmptyLayer;;
  myEmptyLayer.reserve(2);  

  int emptyPlanes[12]={1};

  for ( PrHits::iterator itH2 = track.hits().begin(); track.hits().end() != itH2; ++itH2 ) {
    emptyPlanes[(*itH2)->planeCode()]=0;
  }
  
  for ( unsigned int kk = part; m_hitManager->nbZones()  > kk ; kk += 2 ) {
    
    if( 0 == emptyPlanes[kk/2] ) continue;
    PrHitZone* zone = m_hitManager->zone(kk);   
    if(!zone->isX()) continue;
    
    float zPlane = zone->z(); 
    float xPred = track.x(zPlane ); //Predicted X

    float xMax = xPred + 15.  ;
    float xMin = xPred - 15.  ;

    PrHits::iterator itH = std::lower_bound( zone->hits().begin(), zone->hits().end(), xMin, lowerBoundX() );
    
    for ( ; zone->hits().end() != itH; ++itH ) {
      if ( (*itH)->x() < xMin  ) continue;
      if ( (*itH)->x() > xMax  ) break;
      
      float chi2 = track.chi2( *itH );
      if(chi2<m_maxChi2InTrack) {
        myEmptyLayer.push_back( *itH );
      }
    }
    
    if(myEmptyLayer.size() >0){
      
      PrHits::iterator best = myEmptyLayer.begin();
      float minChi2 = track.chi2( *best);
      
      for ( PrHits::iterator itH = myEmptyLayer.begin()+1; myEmptyLayer.end() != itH; ++ itH) {
	float chi2 = track.chi2( *itH );
	if ( chi2 < minChi2 ) {
	  minChi2 = chi2;
	  best = itH;
	}
      }
      myEmptyLayers.push_back( *best );
      myEmptyLayer.clear();
    }
  }

  if( myEmptyLayers.size()>0 ) {
    info()<<"found hits ";
    for ( PrHits::iterator itH = myEmptyLayers.begin(); myEmptyLayers.end() != itH; ++ itH) {
      track.addHit( *itH );
    }
    
    ok = fitXProjection( track);
    
    while ( !ok && track.hits().size() >  m_minXPlanes  ) {
      ok = removeWorstAndRefit( track, true );
    }
    info()<<" added "<< track.hits().size()<<" ok  "<<ok<<endmsg;
    return ok;
  }
  return ok;
}


//========================================================================= 
// adding hits in empty layers 
//========================================================================= 
bool PrSeedingXLayers::addEmptyLayers(  unsigned int part,  PrSeedTrack& track , bool ok ) {
  
  PrHits myEmptyLayers;
  myEmptyLayers.reserve(3);

  PrHits myEmptyLayer;;
  myEmptyLayer.reserve(3);  

  int emptyPlanes[12]={1};

  for ( PrHits::iterator itH2 = track.hits().begin(); track.hits().end() != itH2; ++itH2 ) {
    emptyPlanes[(*itH2)->planeCode()]=0;
  }
  
  for ( unsigned int kk = part; m_hitManager->nbZones()  > kk ; kk += 2 ) {
    
    if( 0 == emptyPlanes[kk/2] ) continue;

    PrHitZone* zone = m_hitManager->zone(kk);
    float yPred = track.y(zone->z() ); //Predicted y                                                                                                                                                  
    float dxDy = zone->dxDy();
    float zPlane = zone->z(); 
    zPlane= zone->z(yPred);  //correct for dzDy  
    yPred = track.y(zPlane );//to be sure of it
    float xPred = track.x(zPlane ); //Predicted X

    float xMax = xPred + dxDy*yPred   ;
    float xMin = xPred + dxDy*yPred  ;

    if ( xMin > xMax ) {
      float tmp = xMax;
      xMax = xMin;
      xMin = tmp;
    }

    xMax += 5.1;
    xMin -= 5.1;
    
    PrHits::iterator itH = std::lower_bound( zone->hits().begin(), zone->hits().end(), xMin, lowerBoundX() );
   
    
    for ( ; zone->hits().end() != itH; ++itH ) {
      if ( (*itH)->x() < xMin  ) continue;
      if ( (*itH)->x() > xMax  ) break;
      if( yPred < (*itH)->yMin() - 5.1 )continue;
      if( yPred > (*itH)->yMax() + 5.1 )continue;;
      
      float chi2 = track.chi2( *itH );
      if(chi2<m_maxChi2InTrack) {
        myEmptyLayer.push_back( *itH );
      }
    }
    
    if(myEmptyLayer.size() >0){

      PrHits::iterator best = myEmptyLayer.begin();
      float minChi2 = track.chi2( *best);
    
      for ( PrHits::iterator itH = myEmptyLayer.begin()+1; myEmptyLayer.end() != itH; ++ itH) {
	float chi2 = track.chi2( *itH );
	if ( chi2 < minChi2 ) {
	  minChi2 = chi2;
	  best = itH;
	}
      }
      myEmptyLayers.push_back( *best );
      myEmptyLayer.clear();
    }
  }

  if( myEmptyLayers.size()>0 ) {
    
    for ( PrHits::iterator itH = myEmptyLayers.begin(); myEmptyLayers.end() != itH; ++ itH) {
      track.addHit( *itH );
     
      if( (*itH)->isX()){
	track.setnXnY( track.nx()+1, track.ny());
      }else{
	track.setnXnY( track.nx(), track.ny()+1);
      }
    }
    
    ok = fitSimultaneouslyXY( track, 1);


    while ( !ok && track.hits().size() >  m_minTPlanes  ) {
      ok = removeWorstAndRefit( track, false );
    }
      
    return ok;
  }
  return ok;
}

//------------------------------------------------------------------------------
// fit only UV hits
//------------------------------------------------------------------------------
 // method to fit the track passing the iterators pointing to the initial and last element of the hough clusters
bool PrSeedingXLayers::fitYLine(const PrSeedTrack& track, PrHits::const_iterator itBeg, PrHits::const_iterator itEnd,  float& ay, float& by){
  //Re-initialize the track params

    float mat[3];
    float rhs[2];
    int nHitsUV=0;
    ay=999.;
    by=999.;
    float chi2line;

    float ax = track.ax();
    float bx  = track.bx();
    float cx  = track.cx();
    float dRatio =  track.dRatio();

    for( int i = 0; i<3; i++){
      std::fill(rhs,rhs+2,0.);
      std::fill(mat,mat+3,0.);
      for(PrHits::const_iterator hit = itBeg; itEnd!= hit; ++hit){
        if( (*hit)->isX()) continue;
        float w = (*hit)->w();
	const float z     = (*hit)->z();
	const float dz    = z -   m_zReference;
	const float yAtZ  = ay + by*(z -   m_zReference);
	const float xAtZ  = ax + bx*dz + cx*dz*dz*(1.+dRatio*dz);
	const float dist  = (*hit)->x(yAtZ)-xAtZ; 
        const float dxDy  = (*hit)->dxDy();
        const float wdxDy = dxDy*w;
        const float wdxDydz = wdxDy * dz;
        if(m_SlopeCorr){
	  float cos = std::cos( (float)(*hit)->z() );
          w = w*(cos*cos);
        }
        mat[0]+=wdxDy*dxDy;
        mat[1]+=wdxDy*dxDy*dz;
        mat[2]+=wdxDydz*dz * dxDy;
        rhs[0]-=wdxDy * dist;
        rhs[1]-=wdxDydz * dist;
      }
      ROOT::Math::CholeskyDecomp<float,2> decomp(mat);
      if(!decomp){ 
	return false;
      }
      decomp.Solve(rhs);
      ay+=rhs[0];
      by+=rhs[1];
    }
    chi2line = 0.;
    for( PrHits::const_iterator hit = itBeg; itEnd!=hit ;++hit){
      ++nHitsUV;
      double erry = (*hit)->w();
      const float z     = (*hit)->z();
      const float dz    = z -   m_zReference;
      const float yAtZ  = ay + by*(z -   m_zReference);
      const float xAtZ  = ax + bx*dz + cx*dz*dz*(1.+dRatio*dz);
      const float dist  = (*hit)->x(yAtZ)-xAtZ; 
      
      if(m_SlopeCorr){
	double xSlopeVal = bx + 2*cx*dz + 3*cx*dRatio*dz*dz; ;
	double cos = std::cos( xSlopeVal );
	erry = erry/(cos*cos);
      }
      chi2line+= dist*dist*erry;
    }
    
    
    
    if( nHitsUV>5 && chi2line < m_maxChi2HitsYHigh )     return true;
    else if( nHitsUV==5 && chi2line < m_maxChi2HitsYMed ) return true;
    else if( nHitsUV==4 && chi2line < m_maxChi2HitsYLow ) return true;
    return false;
}
