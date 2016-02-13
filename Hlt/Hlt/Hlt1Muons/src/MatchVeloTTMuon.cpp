// STL
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>
// from Gaudi
#include <GaudiKernel/ToolFactory.h>
#include <GaudiKernel/SystemOfUnits.h>
#include <GaudiKernel/boost_allocator.h>

// from LHCb
#include <Kernel/ILHCbMagnetSvc.h>

// Muon Detector
#include <MuonDet/DeMuonDetector.h>

// Hlt1Muons
#include <Hlt1Muons/Candidate.h>

// local
#include "MatchVeloTTMuon.h"

// from MuonID
#include "MuonID/CommonMuonHitManager.h"
#include "MuonID/CommonMuonStation.h"
#include <MuonID/CommonMuonHit.h>

//-----------------------------------------------------------------------------
// Implementation file for class : MatchVeloTTMuon
//
// 2010-12-02 : Roel Aaij, Vasileios Syropoulos
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_COMPONENT( MatchVeloTTMuon )

namespace {
   using std::vector;
   using std::string;
   using std::pair;
   using std::less;
   using std::make_pair;
   const std::array<unsigned int,4> g_order{{ 2u, 3u, 4u, 1u }};
}

//=============================================================================
MatchVeloTTMuon::MatchVeloTTMuon( const string& type, const string& name,
                                  const IInterface* parent )
   : base_class( type, name, parent )
   , m_hitManager{nullptr}
   , m_extrapolator{nullptr}
   , m_fieldSvc{nullptr}
   , m_magnetHit{nullptr}
{
   declareInterface<ITracksFromTrack>( this ); 

   // Stuff for Velo seeds. 
   //J: This is not used by the VeloTTMuonMatch. Only by the VeloMuon Match
   declareProperty( "XWindow", m_xWindow = 200 ); //200
   declareProperty( "YWindow", m_yWindow = 400 ); //400

   declareProperty( "MinMomentum", m_minMomentum = 3 * Gaudi::Units::GeV ); //6 standard value
   declareProperty( "KickScale",   m_kickScale = 1255 * Gaudi::Units::MeV );
   declareProperty( "KickOffset",  m_kickOffset = 175 * Gaudi::Units::MeV );

   // Focal plane parametrisation from B. Hommels
   declareProperty( "MagnetPlaneParA", m_za = 1.069 * Gaudi::Units::m );
   declareProperty( "MagnetPlaneParB", m_zb = 5.203 * Gaudi::Units::m );

   // Stuff for VeloTT seeds
   //J: The FoiI are used by the VeloTTMuonMatch. They are scale factor
   declareProperty( "FoIToleranceX", m_FoIToleranceX = 5. ); //5
   declareProperty( "FoIToleranceY", m_FoIToleranceY = 1.5 ); //1.5
   declareProperty( "Extrapolator",  m_extrapolatorName = "TrackMasterExtrapolator:PUBLIC" );
 
   // General Stuff
   //J: Check MaxMissedHits. 1 apparently means 3 !
   declareProperty( "MaxChi2DoFX",   m_maxChi2DoFX = 20 ); //20
   declareProperty( "MaxMissedHits", m_maxMissed = 1 );  //1
   declareProperty( "SetQOverP",     m_setQOverP = false );
   setProduceHistos( false ); // yes, this indeed changes the default ;-)

}

//=============================================================================
StatusCode MatchVeloTTMuon::initialize()
{
   StatusCode sc = GaudiHistoTool::initialize();
   if ( sc.isFailure() ) return sc;
 
   // init hit manager
   m_hitManager = tool<CommonMuonHitManager>( "CommonMuonHitManager" );
   
   // init track extrapolator
   m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName, this );

   // init magnetic field
   m_fieldSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );
   
   // init muon detector
   m_det = getDet<DeMuonDetector>( "/dd/Structure/LHCb/DownstreamRegion/Muon" );

   // fill local arrays of pad sizes and region sizes

   m_padSizeX.resize( nStations * nRegions );
   m_padSizeY.resize( nStations * nRegions );

   for ( unsigned int s = 0; s < nStations; ++s ) {
      for ( unsigned int r = 0; r < nRegions; ++r ) {
         m_padSizeX[s * nRegions + r] = m_det->getPadSizeX( s, r );
         m_padSizeY[s * nRegions + r] = m_det->getPadSizeY( s, r );
      }
   }

   // declare FoI conditions
   Condition* FoIFactor = nullptr;
   Condition* xFOIParameters = nullptr;
   Condition* yFOIParameters = nullptr;

   // register FoI conditions
   registerCondition<MatchVeloTTMuon>( "Conditions/ParticleID/Muon/FOIfactor",           FoIFactor );
   registerCondition<MatchVeloTTMuon>( "Conditions/ParticleID/Muon/XFOIParameters", xFOIParameters );
   registerCondition<MatchVeloTTMuon>( "Conditions/ParticleID/Muon/YFOIParameters", yFOIParameters );

   // update conditions
   sc = runUpdate();
   if ( sc.isFailure() ) {
      Error( "Could not update conditions from the CondDB" );
      return sc;
   }

   // get FoI parameters locally
   m_FoIFactorX = m_FoIToleranceX * FoIFactor->param<double>( "FOIfactor" );
   m_FoIFactorY = m_FoIToleranceY * FoIFactor->param<double>( "FOIfactor" );
    
   m_xFoIParam1 = xFOIParameters->paramVect<double>( "XFOIParameters1" );
   m_xFoIParam2 = xFOIParameters->paramVect<double>( "XFOIParameters2" );
   m_xFoIParam3 = xFOIParameters->paramVect<double>( "XFOIParameters3" );

   m_yFoIParam1 = yFOIParameters->paramVect<double>( "YFOIParameters1" );
   m_yFoIParam2 = yFOIParameters->paramVect<double>( "YFOIParameters2" );
   m_yFoIParam3 = yFOIParameters->paramVect<double>( "YFOIParameters3" );

   return sc;
}

//=============================================================================
StatusCode MatchVeloTTMuon::finalize()
{
   i_clean();
   return GaudiHistoTool::finalize();
}

//=============================================================================
StatusCode MatchVeloTTMuon::tracksFromTrack( const LHCb::Track& seed,
                                             vector<LHCb::Track*>& tracks ) const
{
   // Clean start
   i_clean();

   // Find seeds
   unsigned int seedStation = g_order[0] ;
   i_findSeeds( seed, seedStation );

   // Add hits to seeds
   for ( Candidate& c : m_seeds ) i_addHits( c );

   // create histograms
   if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "Found " << m_seeds.size() << " seeds." << endmsg;
      for ( const Candidate& c : m_seeds ) {
         debug() << "Found candidate with chi2/DoF " << c.chi2DoF() << endmsg;
      }
   }

   if ( produceHistos() ) {
      plot( m_seeds.size(), "NSeedHits", -0.5, 50.5, 51 );
      for ( const Candidate& c : m_seeds ) {
         plot( c.chi2DoF(), "Chi2DoFX", 0, 100, 100 );
      }
   }
   
   // in this case, we only care whether a good seed exists for the specified track...
   if ( !m_setQOverP ) {
     
      if ( std::any_of( begin(m_seeds), end(m_seeds), [=](const Candidate& c) {
               return c.chi2DoF() < m_maxChi2DoFX ;
            }) ) {
	// There is a good enough candidate, put the seed into the output unmodified.
	tracks.push_back( const_cast<LHCb::Track*>( &seed ) );  

      }
   } else {
      auto best =
         std::min_element( begin(m_seeds), end(m_seeds),
                           []( const Candidate& lhs, const Candidate& rhs ) {
                              return lhs.chi2DoF() < rhs.chi2DoF();
                           } );
      if ( best != end(m_seeds) && best->chi2DoF() < m_maxChi2DoFX ) {
         std::unique_ptr<LHCb::Track> out{seed.clone()};
         out->addToAncestors( seed );
         out->addInfo( 35, best->slope() - best->tx() );
         double down = m_fieldSvc->isDown() ? -1 : 1;
         double q = down * ( ( best->slope() < best->tx() ) - ( best->slope() > best->tx() ) );
         LHCb::State* state = out->stateAt( LHCb::State::AtTT );
         state->setQOverP( q / best->p() );
         tracks.push_back( out.release() );
      }
   
   }

   return StatusCode::SUCCESS;
}

//=============================================================================
void MatchVeloTTMuon::i_findSeeds( const LHCb::Track& seed,
                                   const unsigned int seedStation ) const
{
  //Check if track has a state at TT.
  if( seed.hasTT() ) { 
    Candidate veloSeed{ &seed, LHCb::State::AtTT };
    i_findVeloTTSeeds( veloSeed, seedStation ); 
  }
  // If not, use old seed finding.
  else {
    Candidate veloSeed{ &seed }; 
    i_findVeloSeeds( veloSeed, seedStation );
  }
}

//=============================================================================

void MatchVeloTTMuon::i_findVeloTTSeeds( const Candidate& veloSeed,
                                         const unsigned int seedStation ) const
{
  
  double zMagnet = m_zb + m_za * veloSeed.tx2();
  double xMagnet = 0., errXMagnet = 0.;
  veloSeed.xStraight( zMagnet, xMagnet, errXMagnet );
  double yMagnet = 0., errYMagnet = 0.;
  veloSeed.yStraight( zMagnet, yMagnet, errYMagnet );
  
  LHCb::MuonTileID id;
  m_magnetHit.reset( new CommonMuonHit( id, xMagnet, errXMagnet, yMagnet, errYMagnet, zMagnet, 0., false ) );
  
   // extrapolation from TT to M3 station - Same extrapolation as in the VeloMuonMatch

    double dSlope = m_kickScale / ( m_minMomentum - m_kickOffset );
    const CommonMuonStation& station = m_hitManager->station( seedStation );
    double zStation = station.z();


   const LHCb::State* state = veloSeed.track()->stateAt( LHCb::State::AtTT );
   m_stateAtM3 = LHCb::StateVector( state->stateVector(), state->z() );

   //Use VeloTT infos to see how the particle is bent   

   int mag = m_fieldSvc->isDown() ? -1 : 1;
   double charge = state->qOverP();
   

   double dz = ( zStation - zMagnet );
   double tan =0;
   
   if ((charge<0 && mag==1) || (charge>=0 && mag==-1)){
     tan = ( veloSeed.tx() + dSlope ) / ( 1 - veloSeed.tx() * dSlope );
   }

   else if ((charge>=0 && mag==1) || (charge<0 && mag==-1)){
     tan = ( veloSeed.tx() - dSlope ) / ( 1 + veloSeed.tx() * dSlope );
   }
   
   //for the M3 matching increase the window to improve efficiency
   double xMin = xMagnet + dz * tan - m_xWindow*10;
   double xMax = xMagnet + dz * tan + m_xWindow*10;
   
      
   // Calculate window in y                                                                                                                             
   double yMuon = 0., yRange = 0;
   veloSeed.yStraight( zStation, yMuon, yRange );
   yRange += m_yWindow;

   double yMin = yMuon - yRange*2;
   double yMax = yMuon + yRange*2;
  
   // debug info: print hits in M3
    if ( UNLIKELY( msgLevel( MSG::DEBUG ) ) ) {
      debug() << "Window: (" << xMin << "," << yMin << ") -> (" << xMax << "," << yMax << ")" << endmsg;
      debug() << "Hits in seed station:" << endmsg;
      for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
         for ( const auto& hit : m_hitManager->hits( xMin, xMax, seedStation, r ) ) {
            debug() << hit.x() << " " << hit.y() << endmsg;
         }
      }
   }
   // get hits inside window, and make M3 seeds for a given velo seed
   for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
      if ( !station.overlaps( r, xMin, xMax, yMin, yMax ) ) continue;
      for (const auto& hit : m_hitManager->hits( xMin, xMax, seedStation, r ) ) {
         if ( hit.y() > yMax || hit.y() < yMin ) continue;
         m_seeds.emplace_back ( veloSeed );
         Candidate& seed = m_seeds.back();
         seed.addHit( &hit );

	 // std::cout<<"hit x"<<hit.x()<<std::endl;
	 seed.slope() = ( hit.x() - xMagnet ) / ( hit.z() - zMagnet );
	 // seed.slope()  = m_stateAtM3.tx() ;
	 seed.p() = m_kickScale / fabs( seed.slope() - seed.tx() ) + m_kickOffset;
      }
   }
}



//=============================================================================
void MatchVeloTTMuon::i_findVeloSeeds( const Candidate& veloSeed,
                                     const unsigned int seedStation ) const
{
    // forward extrapolation, make seed point
    double zMagnet = m_zb + m_za * veloSeed.tx2();
    double xMagnet = 0., errXMagnet = 0.;
    veloSeed.xStraight( zMagnet, xMagnet, errXMagnet );
    double yMagnet = 0., errYMagnet = 0.;
    veloSeed.yStraight( zMagnet, yMagnet, errYMagnet );

    LHCb::MuonTileID id;
    m_magnetHit.reset( new CommonMuonHit( id, xMagnet, errXMagnet, yMagnet,
                                          errYMagnet, zMagnet, 0., false ) );

    double dSlope = m_kickScale / ( m_minMomentum - m_kickOffset );
    const CommonMuonStation& station = m_hitManager->station( seedStation );
    double zStation = station.z();

    // Use sum rule for tan and approximate tan( dSlope ) with dSlope to
    // calculate window in x
    double dz = ( zStation - zMagnet );
    // double sign = ( veloSeed.tx() > 0) - ( veloSeed.tx() < 0 );
    double tanMin = ( veloSeed.tx() - dSlope ) / ( 1 + veloSeed.tx() * dSlope );
    double xMin = xMagnet + dz * tanMin - m_xWindow;
    double tanMax = ( veloSeed.tx() + dSlope ) / ( 1 - veloSeed.tx() * dSlope );
    double xMax = xMagnet + dz * tanMax + m_xWindow;

    // Calculate window in y
    double yMuon = 0., yRange = 0;
    veloSeed.yStraight( zStation, yMuon, yRange );
    yRange += m_yWindow;

    double yMin = yMuon - yRange;
    double yMax = yMuon + yRange;

    const LHCb::State* state = veloSeed.track()->stateAt( LHCb::State::EndVelo );
    m_stateAtM3 = LHCb::StateVector( state->stateVector(), state->z() );

    if ( UNLIKELY( msgLevel( MSG::DEBUG ) ) ) {
        debug() << "Window: (" << xMin << "," << yMin << ") -> (" << xMax << ","
                << yMax << ")" << endmsg;
        debug() << "Hits in seed station:" << endmsg;
        for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
            for ( const auto& hit : m_hitManager->hits( xMin, xMax, seedStation, r ) ) {
                debug() << hit.x() << " " << hit.y() << endmsg;
            }
        }
    }

    for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
       //TODO: push into loop control -- request from station to a range of regions to loop over...
       if ( !station.overlaps( r, xMin, xMax, yMin, yMax ) ) continue; 

        // get hits, and add seed hits to container
        for (const auto& hit : m_hitManager->hits( xMin, xMax, seedStation, r ) ) {
            if ( hit.y() > yMax || hit.y() < yMin ) continue;
            m_seeds.emplace_back ( veloSeed );
            Candidate& seed = m_seeds.back();
            seed.addHit( m_magnetHit.get() );
            seed.addHit( &hit );

            seed.slope() = ( hit.x() - xMagnet ) / ( hit.z() - zMagnet );
            seed.p() = m_kickScale / fabs( seed.slope() - seed.tx() ) + m_kickOffset;

	    //	    std::cout<<seed.p()<<std::endl;
        }
    }
}

//=============================================================================

void MatchVeloTTMuon::i_addHits( Candidate& seed ) const
{    

  // First hit is in magnet
  
  double zMagnet = m_magnetHit->z();
  double xMagnet = m_magnetHit->x();

  unsigned int nMissed = 0;
  for ( unsigned int i = 1; i <g_order.size() && nMissed <= m_maxMissed; ++i ) {
    // find candidate hits
    unsigned int s = g_order[i] ;

    // Get the station we're looking at.
    const CommonMuonStation& station = m_hitManager->station( s );
    double zStation = station.z();

    // Calculate window in x and y for this station
    double yMuon = 0., yRange = 0;
    seed.yStraight( zStation, yMuon, yRange );

    
    yRange = m_yWindow;

   const double yMin = yMuon - yRange;
    const double yMax = yMuon + yRange;
    
    const double xMuon = ( zStation - zMagnet ) * seed.slope() + xMagnet;
    const double xRange = m_xWindow;
    
    const double xMin = xMuon - xRange;
    const double xMax = xMuon + xRange;   
    
      // Look for the closest hit inside the search window
      const CommonMuonHit* closest = nullptr;
      double minDist2 = 0;

      for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
         if ( !station.overlaps(r, xMin, xMax, yMin, yMax ) ) continue;
         for ( const auto& hit : m_hitManager->hits( xMin, xMax,  s, r ) ) {
            if ( hit.y() > yMax || hit.y() < yMin ) continue;

            auto dx = xMuon - hit.x();
            auto dy = yMuon - hit.y();
            double dist2 = dx * dx + dy * dy;
            if ( !closest || dist2 < minDist2 ) {
	      // std::cout<<"hit.x()"<<hit.x()<<std::endl; 
	      closest = &hit;
               minDist2 = dist2;
            }
         }
      }

      if ( closest ) {
         seed.addHit( closest );
      } else {
         ++nMissed;
      }
   }

   // If a new candidate is good, fit it.

   // std:: cout<<"NE HO PERSE   "<<nMissed<<std::endl;
   if ( nMissed <= m_maxMissed ) i_fitCandidate( seed );
}


//=============================================================================

void MatchVeloTTMuon::i_fitCandidate( Candidate& candidate ) const
{
   const CommonConstMuonHits& hits = candidate.hits();

   auto hitx = [](const CommonMuonHit* hit) { return make_pair(hit->x(), hit->dx()); };
   auto hity = [](const CommonMuonHit* hit) { return make_pair(hit->y(), hit->dy()); };
   using fun = std::function<pair<double, double>(const CommonMuonHit*)>;
   
   auto chi2 = [&hits](const fun& hitInfo) {
      double sw = 0., sz = 0., sc = 0.;

      for ( const CommonMuonHit* hit : hits ) {
         auto c = hitInfo(hit);
         // xz plane
         double w = 4.0 / ( c.second * c.second );

         sw += w;
         sz += hit->z()  * w; 
         sc   += c.first * w;
      }

      double zow = sz / sw;


      double st = 0.;
      double b = 0.;
      for ( const CommonMuonHit* hit : hits ) {
         auto c = hitInfo(hit);
         // xz plane
	 double err = c.second / 2.;
         double tmp = ( hit->z() - zow ) / err;
         st += tmp * tmp;
         b += tmp * c.first / err;
      }
      b /= st;
      double a = ( sc - sz * b ) / sw;

      double chi2 = 0.;
      for ( const CommonMuonHit* hit : hits ) {
         auto c = hitInfo(hit);
         double err = c.second / 2.;
         double tmp = ( c.first - a - b * hit->z() ) / err;

         chi2 += tmp * tmp;
      }

      return make_pair(chi2, b);
   };
     
   candidate.nDoF() = hits.size() - 2;   
   auto chi2x = chi2(hitx);
   auto chi2y = chi2(hity);
   candidate.chi2() = chi2x.first + chi2y.first;
   candidate.slope() = chi2x.second;
   candidate.p() = m_kickScale / fabs(candidate.slope() - candidate.tx() ) + m_kickOffset;
}

  

//=============================================================================
double MatchVeloTTMuon::FoIX( const int station, const int region,
                              const double p ) const
{
   double dx = m_padSizeX[station * nRegions + region] / 2.;

   if ( p < 1000000. ) {
      return ( m_xFoIParam1[station * nRegions + region] +
               m_xFoIParam2[station * nRegions + region] *
               exp( -m_xFoIParam3[station * nRegions + region] * p /
                    Gaudi::Units::GeV ) ) *
         dx;
   } else {
      return m_xFoIParam1[station * nRegions + region] * dx;
   }
}

//=============================================================================
double MatchVeloTTMuon::FoIY( const int station, const int region,
                              const double p ) const
{
   double dy = m_padSizeY[station * nRegions + region] / 2.;

   if ( p < 1000000. ) {
      return ( m_yFoIParam1[station * nRegions + region] +
               m_yFoIParam2[station * nRegions + region] *
               exp( -m_yFoIParam3[station * nRegions + region] * p /
                    Gaudi::Units::GeV ) ) *
         dy;
   } else {
      return m_yFoIParam1[station * nRegions + region] * dy;
   }
}

//=============================================================================
void MatchVeloTTMuon::i_clean() const
{
   // delete leftover seeds
   m_seeds.clear(); // leaves capacity invariant, hence we latch onto the max size
}

//=============================================================================
void MatchVeloTTMuon::findSeeds( const LHCb::Track& seed, const unsigned int seedStation )
{
   i_findSeeds( seed, seedStation );
}

//=============================================================================
void MatchVeloTTMuon::addHits( Candidate& seed )
{
   i_addHits( seed );
}

//=============================================================================
void MatchVeloTTMuon::fitCandidate( Candidate& seed ) const
{
   i_fitCandidate(seed);
}

//=============================================================================
void MatchVeloTTMuon::clean()
{
   i_clean();
}
