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
  //  declareProperty( "TolXInfIN",           m_tolXInfIN            = 0.5 * Gaudi::Units::mm       );
  //declareProperty( "TolXSupIN",           m_tolXSupIN            = 8.0 * Gaudi::Units::mm       );
  declareProperty( "MinXPlanes",          m_minXPlanes           = 4                            );
  declareProperty( "MinSPlanes",          m_minSPlanes           = 4                            );
  declareProperty( "MinTPlanes",          m_minTPlanes           = 9                            );
  declareProperty( "MaxChi2PerDoF",       m_maxChi2PerDoF        = 4.0                          ); 
  declareProperty( "MaxParabolaSeedHits", m_maxParabolaSeedHits  = 4                            );
  declareProperty( "TolTyOffset",         m_tolTyOffset          = 0.002                        ); 
  declareProperty( "TolTySlope",          m_tolTySlope           = 0.015                        );
  declareProperty( "MaxIpAtZero",         m_maxIpAtZero          = 6000.                        );
  declareProperty( "MaxIpAtZeroIN",       m_maxIpAtZeroIN        = 2500.                        );
  declareProperty( "BestDist",            m_bestDist             = 10.0                         );//added
  declareProperty( "UseFix",              m_useFix               = true                         );//added 
  declareProperty( "TolTriangle",         m_tolTriangle          = 10.* Gaudi::Units::mm        );//added 
  declareProperty( "TolYOffset",          m_tolYOffset           = 100000.* Gaudi::Units::mm    );//added 
  declareProperty( "TolXStereo",          m_tolXStereo           = 2700.* Gaudi::Units::mm      );//added
  declareProperty( "TolXStereoIN",        m_tolXStereoIN         = 500.* Gaudi::Units::mm       );//added
  declareProperty( "TolCoord" ,           m_coord                = 0.005                        );//added
  // Parameters for debugging
  declareProperty( "DebugToolName",       m_debugToolName         = ""                          );
  declareProperty( "WantedKey",           m_wantedKey             = -100                        );
  declareProperty( "TimingMeasurement",   m_doTiming              = false                       );
  declareProperty( "PrintSettings",       m_printSettings         = false                       );
  //Parameter Track Model                                                                                                                              
  declareProperty( "UseCubicCorrection",  m_useCubic              = true                        );
  declareProperty( "dRatio",              m_dRatio                = -0.000262                   ); 
  declareProperty( "CConst",              m_ConstC                =  2.458e8                    );  
  declareProperty( "UseCorrPosition",     m_useCorrPos            = true                        ); 
  declareProperty( "maxChi2Hits_high",    m_maxChi2HitFullFitHigh = 5.5                         );
  declareProperty( "maxChi2Hits_low",     m_maxChi2HitFullFitLow  = 2.5                         );
  declareProperty( "maxYatZeroLow",       m_maxY0Low              = 70.                         );
  declareProperty( "maxYatzRefLow",       m_maxYZrefLow           = 700.                        );
  declareProperty( "maxChi2HitsX",        m_maxChi2HitsX          = 5.5                         );


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
//  Fit the X track, return OK if fit sucecssfull
//=========================================================================
bool PrSeedingXLayers::fitXProjection(PrSeedTrack& track ){
  if (msgLevel(MSG::DEBUG)) debug()<<"Fitting"<<endmsg;

  double mat[6];
  double rhs[3];
  const double zRef = m_zReference;
  // if(m_useCubic) track.setdRatio(m_dRatio);                                                                                                             
  for(int loop = 0;3>loop;++loop)
    {
      std::fill(mat,mat+6,0.);
      std::fill(rhs,rhs+3,0.);
      for( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
	const double dRatio = track.dRatio();
	double w = (*itH)->w();//squared                                                                                                                     
	if(m_SlopeCorr){
	  track.setSlopeCorr( true);
	  double Corr = std::cos( track.xSlope( (*itH)->z()));
	  if(loop ==0) Corr = 1.;
	  w = w*(Corr*Corr);
	}
	const double dz= 0.001*((*itH)->z() - zRef);
	const double deta = dz*dz*(1. + dRatio*dz);
	const double dist = track.distance( *itH );
	if (msgLevel(MSG::DEBUG)) debug()<<"Loop \t"<<loop<<"\n Distance From Hit \t"<<dist<<endmsg;
	mat[0]+= w;
	mat[1]+= w * dz;   mat[2]+= w * dz * dz;
	mat[3]+= w * deta; mat[4]+= w * dz * deta;  mat[5]+= w * deta * deta;
	rhs[0]+= w * dist;
	rhs[1]+= w * dist * dz;
	rhs[2]+= w * dist * deta;
      }
      ROOT::Math::CholeskyDecomp<double,3> decomp(mat);
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
	break;
      }
    }
  //Compute some values on the track                                                                                                                       
  double maxChi2 = 0.;
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) 
    {
      double chi2_onHit = track.chi2( *itH );
      if ( chi2_onHit > maxChi2 ){
	maxChi2 = chi2_onHit;
      }
    }
  // double X0 = track.ax() - track.bx()*m_geoTool->zReference()+track.cx()*m_ConstC;                                                                      
  double X0 = track.ax() - track.bx()*m_zReference+track.cx()*m_ConstC;

  track.setX0(X0);
  track.setMaxChi2(maxChi2);
  return ( m_maxChi2HitsX > maxChi2 );
}

//=========================================================================                                                                                
//  Fit the track, return OK if fit sucecssfull                                                                                                            
//=========================================================================                                                                                
bool PrSeedingXLayers::fitSimultaneouslyXY( PrSeedTrack& track, int refit){

  if( track.nx() <m_minXPlanes || track.ny() <m_minSPlanes ) return false;
  double mat[15];
  double rhs[5];

  const double zRef = m_zReference;// m_geoTool->zReference();                                                                                             
  for ( int loop = refit; 3 > loop ; ++loop ){
    if(loop ==1 && m_useCubic && m_useCorrPos){
      double RadiusPosition = std::sqrt((0.0005*0.0005*track.ax()*track.ax() + 0.001*0.001*track.y(zRef)*track.y(zRef)));
      double dRatioPos = -1.*(2.633e-4 - 3.59957e-6*RadiusPosition + 4.7312e-5*RadiusPosition*RadiusPosition);
      track.setdRatio(dRatioPos);
    }

    std::fill(mat,mat+15,0.);
    std::fill(rhs,rhs+5,0.);
    for( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
      double w = (*itH)->w();
      if( m_SlopeCorr && loop >0){
        const double Corr = std::cos(track.xSlope( (*itH)->z()) );
        track.setSlopeCorr( true);
        w = w*(Corr*Corr);
      }
      const double dxdy = (*itH)->dxDy();
      const double yOnTrack = track.yOnTrack( (*itH) ) ;
      const double   dz = 0.001*((*itH)->z( yOnTrack ) - zRef);
      const double dRatio = track.dRatio();
      const double deta = dz*dz*(1. + dz*dRatio);
      const double wdz = w * dz;
      const double weta = w * deta;
      const double wdxdy = w * dxdy;
      const double wdxdydz = wdxdy * dz;
      const double dist = track.distance(*itH);
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
    ROOT::Math::CholeskyDecomp<double, 5> decomp(mat);
    if (!decomp) return false;
    decomp.Solve(rhs);
    rhs[1]*=1.e-3;
    rhs[2]*=1.e-6;
    rhs[4]*=1.e-3;
    rhs[3]-=rhs[4]*zRef;
    if( loop >0 &&(std::fabs(rhs[0]) > 1e4 || std::fabs(rhs[1]) > 5. ||
                   std::fabs(rhs[2]) > 1e-3 || std::fabs(rhs[3]) > 1e4 || std::fabs(rhs[4]) > 1.)) return false;
    track.updateParameters(rhs[0],rhs[1],rhs[2],rhs[3],rhs[4]);
  }
  double maxChi2 =0.;
  for( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
    double chi2_onHit = track.chi2( *itH);
    if ( chi2_onHit > maxChi2 ){
      maxChi2 = chi2_onHit;
    }
  }//Set Max Chi2DoF                                                                                                                                       
  double X0 = track.ax() - track.bx()*m_zReference+track.cx()*m_ConstC;
  track.setX0(X0);
  track.setMaxChi2(maxChi2);
  if( (track.hits().size()>10) && maxChi2<m_maxChi2HitFullFitHigh) return true;
  if( std::fabs(track.y(0.))< m_maxY0Low &&
      maxChi2<m_maxChi2HitFullFitLow &&
      std::fabs(track.y(zRef)) < m_maxYZrefLow ) return true;
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

    debug()<<"Currently in case "<<iCase<<" . First zone "<<firstZone<<" . Last zone "<<lastZone<<endmsg;
      
     //Load of Hits on zones
    PrHitZone* fZone = m_hitManager->zone( firstZone );
    PrHitZone* lZone = m_hitManager->zone( lastZone  );
    double invDeltaz= 1./ (lZone->z() - fZone->z() );
    double zRatio =  lZone->z(0.) / fZone->z(0.);

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
    std::vector< PrHits::iterator > iterators;
    iterators.reserve(24);

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

          // shifted before continue due to debug reasons
          std::stable_sort(xHits.begin(), xHits.end(), compX());


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

 
 std::sort( m_xCandidates.begin(), m_xCandidates.end(), PrSeedTrack::GreaterBySize() );                   
 
 for ( PrSeedTracks::iterator itT1 = m_xCandidates.begin(); m_xCandidates.end() !=itT1; ++itT1 ) {  
   if ( !(*itT1).valid() ) continue;
   for ( PrSeedTracks::iterator itT2 = itT1 + 1; m_xCandidates.end() !=itT2; ++itT2 ) {
     if ( !(*itT2).valid() ) continue;             
     
     PrHits::iterator itH1 = (*itT1).hits().begin();
     PrHits::iterator itH2 = (*itT2).hits().begin();
     PrHits::iterator itEnd1 = (*itT1).hits().end();
     PrHits::iterator itEnd2 = (*itT2).hits().end();
     int Compare = (*itT1).hits().size()+(*itT2).hits().size();
     unsigned int maxCommon = 3;
     if( Compare <10 )	maxCommon = 1;	//5 vs 4
     else if( Compare ==10 )maxCommon = 2; //5 vs 5 or 6 vs 4 
     
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
       (*itT1).setValid(false);
       break;
     }
   }
 }
 
 if(m_xOnly){
   for( PrSeedTracks::iterator itT1 = m_xCandidates.begin() ; m_xCandidates.end()!= itT1; ++itT1){
      if((*itT1).valid()) m_trackCandidates.push_back( (*itT1));
   }
 }
    
  

 
  /*
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

      if ( (*itT2).hits().size() != 6 ) {
	int nUsed2 = 0;
      
	//count number of used hits in same 
	for ( PrHits::iterator itH2 = (*itT2).hits().begin(); (*itT2).hits().end() != itH2; ++ itH2) {
	  if ( (*itH2)->isUsed()) ++nUsed2;
	}
	if ( 1 < nUsed2 ) 
	  {
	    (*itT2).setValid( false );
	    continue;
	  }
      }

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
*/
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
      float invdxDy = 1./dxDy;
      float invzPlane = 1./zPlane;
      float xPred = (*itT).x(m_hitManager->zone(kk)->z() ); //Predicted X                                            

#ifdef DEBUG_HISTO
      plot2D(xPred,zPlane,"xPred_ZPlane","xPred_ZPlane",-3000.,3000.,6000.,12000.,1000,1000);
#endif

      float xMin = xPred + m_tolXStereo* dxDy;
      float xMax = xPred - m_tolXStereoIN * dxDy;
      //simmetric search windows in the X axis                                                                                                                                                                  
      if(part==0) {
        xMax = xPred - m_tolXStereo* dxDy;
        xMin = xPred + m_tolXStereoIN * dxDy;
      }
      

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
        if ( (*itH)->x() > xMax ) break;

	const double yOnTrack = ( xPred - (*itH)->x() )  * invdxDy;
	if (m_useFix && ((part==0&&kk%2==1) || (part==1&&kk%2==0) )){

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

	(*itH)->setCoord( ((*itH)->x() - xPred) * invdxDy  * invzPlane );


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
    PrHits::iterator itEnd = itBeg + m_minSPlanes  ; //Why 5?

    while ( itEnd < myStereo.end() ) {
      float tolTy = m_tolTyOffset + m_tolTySlope * fabs( (*itBeg)->coord() );
      // Take the next 4 hits and check if inside a given tolerance                                                                                                                                             
      if ( (*(itEnd-1))->coord() - (*itBeg)->coord() < tolTy ) {
        while( itEnd+1 < myStereo.end() &&
               (*itEnd)->coord() - (*itBeg)->coord() < tolTy ) {
          ++itEnd;
        }

	// count planes                                                                                                                                                                                         
        plCount.set( itBeg, itEnd );

	if ( m_minSPlanes-1 < plCount.nbDifferent() ) {
          PrSeedTrack temp( *itT );
          for ( PrHits::iterator itH = itBeg; itEnd != itH; ++itH ) {
            temp.addHit( *itH );
          }
	  temp.setnXnY( (*itT).hits().size(), temp.hits().size()-(*itT).hits().size());
          bool ok = fitSimultaneouslyXY( temp , 0);

          //ok = fitTrack( temp );
          //ok = fitTrack( temp );
	  
          while ( !ok && temp.hits().size() > m_minTPlanes ) {
            ok = removeWorstAndRefit( temp, false );
          }

	  if(temp.hits().size()<12) ok=addEmptyLayers(part, temp);
          if ( ok  &&  (temp.hits().size() > m_minTPlanes ||  (temp.hits().size()<10 &&  innerMod(temp) < 4 ))){
	    setChi2( temp );
	    float maxChi2 = m_maxChi2PerDoF + 6*temp.xSlope(9000)*temp.xSlope(9000);
	    if(temp.chi2PerDoF() < maxChi2 ){                                                                         
	      m_trackCandidates.push_back( temp );
	      std::sort( temp.hits().begin(),  temp.hits().end(), compLHCbID());
	      if(temp.hits().size()==12) break;
	      else   itBeg = itEnd-1;
	    }
	  }
        }
      }
      ++itBeg;
      itEnd = itBeg + m_minSPlanes;
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
    }//loop candidates removal                                                                                           
    

  }
}



//=========================================================================
// Solve parabola using Cramer's rule
//========================================================================
void PrSeedingXLayers::solveParabola(const PrHit* hit1, const PrHit* hit2, const PrHit* hit3, float& a, float& b, float& c){
 
  const double z1 = hit1->z() - m_zReference;//->zReference();       
  const double z2 = hit2->z() - m_zReference;//m_geoTool->zReference();        
  const double z3 = hit3->z() - m_zReference;//m_geoTool->zReference();                                                                                     
  const double x1 = hit1->x();
  const double x2 = hit2->x();
  const double x3 = hit3->x();

  const double corrZ1 = 1.+m_dRatio*z1;
  const double corrZ2 = 1.+m_dRatio*z2;
  const double corrZ3 = 1.+m_dRatio*z3;
  const double det = (z1*z1)*corrZ1*z2 + z1*(z3*z3)*corrZ3 + (z2*z2)*corrZ2*z3 - z2*(z3*z3)*corrZ3 - z1*(z2*z2)*corrZ2 - z3*(z1*z1)*corrZ1;
  if( std::fabs(det) < 1e-8 )
    {
      a = 0.0;
      b = 0.0;
      c = 0.0;
      return;
    }
  const double recdet = 1./det;
  const double det1 = (x1)*z2 + z1*(x3) + (x2)*z3 - z2*(x3) - z1*(x2) - z3*(x1);
  const double det2 = (z1*z1)*corrZ1*x2 + x1*(z3*z3)*corrZ3 + (z2*z2)*corrZ2*x3 - x2*(z3*z3)*corrZ3 - x1*(z2*z2)*corrZ2 - x3*(z1*z1)*corrZ1;
  const double det3 = (z1*z1)*corrZ1*z2*x3 + z1*(z3*z3)*corrZ3*x2 + (z2*z2)*corrZ2*z3*x1  
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
bool PrSeedingXLayers::addEmptyLayers(  unsigned int part,  PrSeedTrack& track  ) {

  PrHits myEmptyLayer;;
  myEmptyLayer.reserve(12);

  
  int emptyPlanes[12]={1};
  for ( PrHits::iterator itH2 = track.hits().begin(); track.hits().end() != itH2; ++itH2 ) {
    emptyPlanes[(*itH2)->planeCode()]=0;
  }
  
  debug()<<"let's add some hits"<<endmsg;
  for ( unsigned int kk = part; m_hitManager->nbZones()  > kk ; kk += 2 ) {

    if( 0 == emptyPlanes[kk/2] ) continue;
    float yPred = track.y(m_hitManager->zone(kk)->z() ); //Predicted y                                                                                                                                                  
    float dxDy = m_hitManager->zone(kk)->dxDy();
    float zPlane = m_hitManager->zone(kk)->z(); 
    zPlane= m_hitManager->zone(kk)->z(yPred);  //correct for dzDy  
    yPred = track.y(zPlane );//to be sure of it
    float xPred = track.x(zPlane ); //Predicted X

    float xMax = xPred + dxDy*yPred  ;
    float xMin = xPred + dxDy*yPred  ;

    if ( xMin > xMax ) {
      float tmp = xMax;
      xMax = xMin;
      xMin = tmp;
    }

    PrHits::iterator itH = m_hitManager->zone(kk)->hits().begin();
    for ( ;
          m_hitManager->zone(kk)->hits().end() != itH; ++itH ) {
      if ( (*itH)->x() < xMin-2.1  ) continue;
      if ( (*itH)->x() > xMax+2.1  ) break;
      if( yPred < (*itH)->yMin() -2.1)continue;
      if( yPred > (*itH)->yMax() +2.1  )continue;;
      
      float chi2 = track.chi2( *itH );
      if(chi2<m_maxChi2InTrack) {
        myEmptyLayer.push_back( *itH );
      }
    }
  }
  
  bool ok=true;
  if( myEmptyLayer.size()>0 ) {

    debug()<<"-------------------------------"<<endmsg;
    for ( PrHits::iterator itH = myEmptyLayer.begin(); myEmptyLayer.end() != itH; ++ itH) {

      track.addHit( *itH );
      
      if( (*itH)->isX()){
	track.setnXnY( track.nx()-1, track.ny());
      }else{
	track.setnXnY( track.nx(), track.ny()-1);
      }

      ok = fitSimultaneouslyXY( track, 1);

      while ( !ok && track.hits().size() >  m_minTPlanes  ) {
        ok = removeWorstAndRefit( track, false );
      }
    }
    
    setChi2( track );
  }
  return ok;
}

