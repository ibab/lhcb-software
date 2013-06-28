// $Id: $
// STL
#include <vector>
#include <algorithm>

// boost
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/assign/list_of.hpp>

// from Gaudi
#include <GaudiKernel/ToolFactory.h>
#include <GaudiKernel/SystemOfUnits.h>
#include <GaudiKernel/boost_allocator.h>
#include <DetDesc/Condition.h>

// from LHCb
#include <Kernel/ILHCbMagnetSvc.h>

// Muon Detector
#include <MuonDet/DeMuonDetector.h>

// local
#include "HltVeloIsMuon.h"
#include "Hlt1MuonHit.h"
#include "Candidate.h"
#include "Hlt1MuonHitManager.h"
#include "Hlt1MuonStation.h"
#include "Candidate.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltVeloIsMuon
//
// 2010-15-03 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( Hlt, HltVeloIsMuon )

using boost::assign::list_of;
using std::vector;

//=============================================================================
Hlt::HltVeloIsMuon::HltVeloIsMuon( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent )
   : base_class( type, name , parent ),
     m_hitManager( 0 ), m_fieldSvc( 0 ),
     m_nStations( 5 ), m_nRegions( 4 ),
     m_magnetHit( 0 ), m_seeds( 0 )
{
   declareInterface< ITracksFromTrack >( this );

   declareProperty( "SeedWindowX", m_xWindow = 200 );
   declareProperty( "SeedWindowY", m_yWindow = 200 );
   declareProperty( "FoITolerance", m_FoITolerance = 1.05 );

   declareProperty( "MinMomentum", m_minMomentum = 6 * Gaudi::Units::GeV );
   declareProperty( "KickScale", m_kickScale = 1255 * Gaudi::Units::MeV );
   declareProperty( "KickOffset", m_kickOffset = 175 * Gaudi::Units::MeV );
   
   declareProperty( "MaxChi2DoFX", m_maxChi2DoFX = 10 );

   // Focal plane parametrisation from B. Hommels
   declareProperty( "MagnetPlaneParA", m_za = 1.069 * Gaudi::Units::m );
   declareProperty( "MagnetPlaneParB", m_zb = 5.203 * Gaudi::Units::m );

   declareProperty( "MaxMissedHits", m_maxMissed = 2 );
   setProduceHistos( false ); // yes, this indeed changes the default ;-)

   declareProperty( "SetQOverP", m_setQOverP = false );
   
   std::vector<unsigned int> tmp=list_of( 3 )( 4 )( 5 )( 2 );
   m_order = tmp;

   m_regionFoIX.reserve( m_nRegions );
   m_regionFoIY.reserve( m_nRegions );
}

//=============================================================================
Hlt::HltVeloIsMuon::~HltVeloIsMuon()
{
   boost::singleton_pool< Candidate, sizeof( Candidate ) >::release_memory();
} 

//=============================================================================
StatusCode Hlt::HltVeloIsMuon::initialize()
{
   StatusCode sc = GaudiHistoTool::initialize();
   if ( sc.isFailure() ) return sc;

   m_hitManager = tool< Hlt1MuonHitManager >( "Hlt1MuonHitManager" );

   // Magnetic Field
   m_fieldSvc = svc< ILHCbMagnetSvc >( "MagneticFieldSvc", true );

   // fill local arrays of pad sizes and region sizes
   m_det = getDet< DeMuonDetector >( "/dd/Structure/LHCb/DownstreamRegion/Muon" );

   m_padSizeX.resize( m_nStations * m_nRegions );
   m_padSizeY.resize( m_nStations * m_nRegions );
   m_regionInnerX.resize( m_nStations );
   m_regionOuterX.resize( m_nStations );
   m_regionInnerY.resize( m_nStations );
   m_regionOuterY.resize( m_nStations );

   for( unsigned int s = 0; s < m_nStations; ++s ){
      m_regionInnerX[ s ] = m_det->getInnerX( s );
      m_regionOuterX[ s ] = m_det->getOuterX( s );
      m_regionInnerY[ s ] = m_det->getInnerY( s );
      m_regionOuterY[ s ] = m_det->getOuterY( s );

      for( unsigned int r = 0; r < m_nRegions; ++r ) {
         m_padSizeX[ s * m_nRegions + r ] = m_det->getPadSizeX( s, r );
         m_padSizeY[ s * m_nRegions + r ] = m_det->getPadSizeY( s, r );
      }
   }

   // Condition pointers
   Condition* FoIFactor = 0;
   Condition* xFOIParameters = 0;
   Condition* yFOIParameters = 0;
   Condition* preSelMomentum = 0; 
   Condition* momentumCuts = 0;

   // Register conditions
   registerCondition< HltVeloIsMuon >( "Conditions/ParticleID/Muon/FOIfactor", FoIFactor );
   registerCondition< HltVeloIsMuon >( "Conditions/ParticleID/Muon/XFOIParameters", xFOIParameters );
   registerCondition< HltVeloIsMuon >( "Conditions/ParticleID/Muon/YFOIParameters", yFOIParameters );
   registerCondition< HltVeloIsMuon >( "Conditions/ParticleID/Muon/PreSelMomentum", preSelMomentum );
   registerCondition< HltVeloIsMuon >( "Conditions/ParticleID/Muon/MomentumCuts", momentumCuts );

   // Update conditions
   sc = runUpdate();
   if ( sc.isFailure() ) {
      Error( "Could not update conditions from the CondDB" );
      return sc;
   }

   // Get the parameters
   m_FoIFactor = m_FoITolerance * FoIFactor->param< double >( "FOIfactor" );
   m_xFoIParam1 = xFOIParameters->paramVect< double >( "XFOIParameters1" );
   m_xFoIParam2 = xFOIParameters->paramVect< double >( "XFOIParameters2" );
   m_xFoIParam3 = xFOIParameters->paramVect< double >( "XFOIParameters3" );

   m_yFoIParam1 = yFOIParameters->paramVect< double >( "YFOIParameters1" );
   m_yFoIParam2 = yFOIParameters->paramVect< double >( "YFOIParameters2" );
   m_yFoIParam3 = yFOIParameters->paramVect< double >( "YFOIParameters3" );

   m_preSelMomentum = preSelMomentum->param< double >( "PreSelMomentum" );

   m_momentumCuts = momentumCuts->paramVect< double >( "MomentumCuts" );

   return sc;
}

//=============================================================================
StatusCode Hlt::HltVeloIsMuon::finalize()
{
   clean();
   boost::singleton_pool< Candidate, sizeof( Candidate ) >::release_memory();
   return GaudiHistoTool::finalize();
}

//=============================================================================
StatusCode Hlt::HltVeloIsMuon::tracksFromTrack( const LHCb::Track &seed,
                                                vector< LHCb::Track* >& tracks )
{
   // Clean start
   clean();

   // Make a Candidate from the track
   std::auto_ptr< Candidate > veloSeed( new Candidate( &seed ) );

   unsigned int seedStation = m_order[ 0 ] - 1;
   findSeeds( veloSeed.get(), seedStation );
   if ( produceHistos() ) plot( m_seeds.size(), "NSeedHits", -0.5, 50.5, 51 );

   if ( msgLevel(MSG::DEBUG) ) {
      debug() << "Found " << m_seeds.size() << " seeds." << endmsg;
   }

   ConstCandidates goodCandidates;   

   BOOST_FOREACH( Candidate* c, m_seeds ) { 
      addHits( c );
      if ( msgLevel(MSG::DEBUG)) {
         debug() << "Found candidate with chi2/DoF " << c->chi2DoF() << endmsg;
      }
      if ( !c->fitted() ) continue;
      if ( produceHistos() ) plot( c->chi2DoF(), "Chi2DoFX", 0, 100, 100 );
      if ( !m_setQOverP && ( c->chi2DoF() < m_maxChi2DoFX ) ) {
         // There is a good enough candidate, put the seed into the output unmodified.
         tracks.push_back( const_cast< LHCb::Track* >( &seed ) );
         break;
      } else if ( m_setQOverP && ( c->chi2DoF() < m_maxChi2DoFX ) ) {
         goodCandidates.push_back( c );
      }
   }

   if ( m_setQOverP ) {
      ConstCandidates::const_iterator best = std::min_element
         (goodCandidates.begin(), goodCandidates.end(),
          boost::bind(std::less<double>(), 
                      boost::bind( &Candidate::chi2DoF, _1 ),
                      boost::bind( &Candidate::chi2DoF, _2 )));
      if ( best != goodCandidates.end() ) {
         LHCb::Track* out = seed.clone();
         const Candidate* c = *best;
         
         LHCb::State* state = out->stateAt( LHCb::State::EndVelo );
         double down = m_fieldSvc->isDown() ? -1 : 1;
         double q = down * ( ( c->slope() < c->tx() ) - ( c->slope() > c->tx() ) );
         state->setQOverP( q / c->p() );
         tracks.push_back( out );
      }
   }
   return StatusCode::SUCCESS;
}

//=============================================================================
void Hlt::HltVeloIsMuon::findSeeds( const Candidate* veloSeed,
                                    const unsigned int seedStation )
{
   // forward extrapolation, make seed point
   double zMagnet = m_zb + m_za * veloSeed->tx2();
   double xMagnet = 0., errXMagnet = 0.;
   veloSeed->xStraight( zMagnet, xMagnet, errXMagnet );
   double yMagnet = 0., errYMagnet = 0.;
   veloSeed->yStraight( zMagnet, yMagnet, errYMagnet );

   LHCb::MuonTileID id;
   m_magnetHit = new Hlt1MuonHit( id, xMagnet, errXMagnet,
                                  yMagnet, errYMagnet,
                                  zMagnet, 0. );

   double dSlope = dtx( m_minMomentum );
   const Hlt1MuonStation& station = m_hitManager->station( seedStation );
   double zStation = station.z();

   // Use sum rule for tan and approximate tan( dSlope ) with dSlope to 
   // calculate window in x
   double dz = ( zStation - zMagnet ) / veloSeed->cosTy();
   // double sign = ( veloSeed->tx() > 0) - ( veloSeed->tx() < 0 );
   double tanMin = ( veloSeed->tx() - dSlope ) / ( 1 + veloSeed->tx() * dSlope );
   double xMin = xMagnet + dz * tanMin - m_xWindow;
   double tanMax = ( veloSeed->tx() + dSlope ) / ( 1 - veloSeed->tx() * dSlope );
   double xMax = xMagnet + dz * tanMax + m_xWindow;

   // Calculate window in y
   double yMuon = 0., yRange = 0;
   veloSeed->yStraight( zStation, yMuon, yRange );
   yRange += m_yWindow;

   double yMin = yMuon - yRange;
   double yMax = yMuon + yRange;

   if ( msgLevel(MSG::DEBUG) ) {
      debug() << "Window: (" << xMin << "," << yMin << ") -> (" 
              << xMax << "," << yMax << ")" << endmsg;
      debug() << "Hits in seed station:" << endmsg;
      for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
         Hlt1MuonHitRange hits = m_hitManager->hits( xMin, seedStation, r );
         BOOST_FOREACH( Hlt1MuonHit* hit, hits ) {
            debug() << hit->x() << " " << hit->y() << endmsg;
         }
      }
   }

   for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
      const Hlt1MuonRegion& region = station.region( r );
      if ( !region.overlap( xMin, xMax, yMin, yMax ) ) continue;

      // Get hits
      Hlt1MuonHitRange hits = m_hitManager->hits( xMin, seedStation, r );
      if ( msgLevel(MSG::DEBUG) ) {
         debug() << "Hits in seed region " << r << ":" << endmsg;
         BOOST_FOREACH( Hlt1MuonHit* hit, hits ) {
            debug() << hit->x() << " " << hit->y() << endmsg;
         }
      }
         
      if ( hits.empty() ) continue; 

      // add seed hits to container
      BOOST_FOREACH( Hlt1MuonHit* hit, hits ) {
         if ( hit->x() > xMax ) break;
         if ( hit->y() > yMax || hit->y() < yMin ) continue;
         Candidate* seed = new Candidate( *veloSeed );
         seed->addHit( m_magnetHit );
         seed->addHit( hit );

         seed->slope() = ( hit->x() - xMagnet ) / ( hit->z() - zMagnet );
         seed->p()     = momentum( seed->slope() - seed->tx() );

         m_seeds.push_back( seed );
      }
   }
}

//=============================================================================
void Hlt::HltVeloIsMuon::addHits( Candidate* seed )
{
   // First hit is in magnet
   double zMagnet = m_magnetHit->z();
   double xMagnet = m_magnetHit->x();

   unsigned int nMissed = 0;
   for ( unsigned int i = 1; i < m_order.size(); ++i ) {
      // find candidate hits
      unsigned int s = m_order[ i ] - 1;

      // Get the station we're looking at.
      const Hlt1MuonStation& station = m_hitManager->station( s );
      double zStation = station.z();

      // Clear and cache region FoIs
      m_regionFoIX.clear();
      m_regionFoIY.clear();
      double maxFoIX = 0;
      double maxFoIY = 0;
      // Find the maximum FoI to use as a search window.
      for ( unsigned int region = 0; region < m_nRegions; ++region ) {
         const double foiX = m_FoIFactor * FoIX( s, region, seed->p() );
         const double foiY = m_FoIFactor * FoIY( s, region, seed->p() );
         if ( foiX > maxFoIX ) maxFoIX = foiX;
         if ( foiY > maxFoIY ) maxFoIY = foiY;
         m_regionFoIX[ region ] = foiX;
         m_regionFoIY[ region ] = foiY;
      }

      // Calculate window in x and y for this station
      double yMuon = 0., yRange = 0;
      seed->yStraight( zStation, yMuon, yRange );
      const double yMin = yMuon - maxFoIY;
      const double yMax = yMuon + maxFoIY;

      const double xMuon = ( zStation - zMagnet ) * seed->slope() + xMagnet;
      const double xMin = xMuon - maxFoIX;
      const double xMax = xMuon + maxFoIX;

      // Look for the closest hit inside the search window
      const Hlt1MuonHit* closest = 0;
      double minDist2 = 0;
      for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
         const Hlt1MuonRegion& region = station.region( r );
         if ( !region.overlap( xMin, xMax, yMin, yMax ) ) continue;
         Hlt1MuonHitRange hits = m_hitManager->hits( xMin, s, r );
         BOOST_FOREACH( Hlt1MuonHit* hit, hits ) {
            if ( hit->x() > xMax ) break;

            // Take the actual FoI into account
            unsigned int r = hit->tile().region();
            double xMinFoI = xMuon - m_regionFoIX[ r ];
            double xMaxFoI = xMuon + m_regionFoIX[ r ];
            double yMinFoI = yMuon - m_regionFoIY[ r ];
            double yMaxFoI = yMuon + m_regionFoIY[ r ];
            if ( hit->x() > xMaxFoI || hit->x() < xMinFoI
                 || hit->y() > yMaxFoI || hit->y() < yMinFoI ) continue;

            double dist2 =  ( xMuon - hit->x() ) * ( xMuon - hit->x() )
                          + ( yMuon - hit->y() ) * ( yMuon - hit->y() ) ;
            if ( !closest || dist2 < minDist2 ) {
               closest = hit;
               minDist2 = dist2;
            }
         }
      }

      if ( closest != 0 ) {
         seed->addHit( closest );
      } else {
         ++nMissed ;
      }
      if ( nMissed > m_maxMissed ) break;
   }
   // If a new candidate is good, fit it.
   if ( nMissed <= m_maxMissed ) {
      fitCandidate( seed );
   }
}

//=============================================================================
void Hlt::HltVeloIsMuon::fitCandidate( Candidate* candidate ) const
{
   const Hlt1ConstMuonHits& hits = candidate->hits();

   double sumWeights = 0., sumZ = 0., sumX = 0., sumTmp2 = 0.;

   BOOST_FOREACH( const Hlt1MuonHit* hit, hits ) {
      double err = hit->dx() / 2.;
      double weight = 1.0 / ( err * err );
      sumWeights += weight;
      sumZ += hit->z() * weight;
      sumX += hit->x() * weight;
   }
   double ZOverWeights = sumZ / sumWeights;

   double b = 0;
   BOOST_FOREACH( const Hlt1MuonHit* hit, hits ) {
      double err = hit->dx() / 2.;
      double tmp = ( hit->z() - ZOverWeights ) / err;
      sumTmp2 += tmp * tmp;
      b += tmp * hit->x() / err;
   }

   b /= sumTmp2;
   double a = ( sumX - sumZ * b ) / sumWeights;
   // double errA = sqrt( ( 1. + sumZ * sumZ / ( sumWeights * sumTmp2 ) ) / sumWeights );
   // double errB = sqrt( 1. / sumTmp2 );

   double chi2 = 0.;
   BOOST_FOREACH( const Hlt1MuonHit* hit, hits ) {
      double err = hit->dx() / 2.;
      double tmp = ( hit->x() - a - b * hit->z() ) / err;
      chi2 += tmp * tmp;
   }

   candidate->fitted() = true;
   candidate->nDoF() = hits.size() - 2;
   candidate->chi2() = chi2;
   candidate->slope() = b;
   candidate->p()     = momentum( b - candidate->tx() );
}

//=============================================================================
void Hlt::HltVeloIsMuon::clean()
{
   // delete leftover seeds
   BOOST_FOREACH( Candidate* candidate, m_seeds ) { delete candidate; }
   m_seeds.clear();
   delete m_magnetHit;
   m_magnetHit = 0;
}

//=============================================================================
double Hlt::HltVeloIsMuon::FoIX( const int station,
                                 const int region,
                                 const double p ) const
{
   double dx = m_padSizeX[ station * m_nRegions + region ] / 2.;
   
   if ( p < 1000000. ) {
      return ( m_xFoIParam1[ station * m_nRegions + region ] +
               m_xFoIParam2[ station * m_nRegions + region ] *
               exp( - m_xFoIParam3[ station * m_nRegions + region ] 
                    * p / Gaudi::Units::GeV ) ) * dx;
   } else {
      return m_xFoIParam1[ station * m_nRegions + region ] * dx;
   }
}

//=============================================================================
double Hlt::HltVeloIsMuon::FoIY( const int station,
                                 const int region,
                                 const double p ) const
{
   double dy = m_padSizeY[ station * m_nRegions + region ] / 2.;

   if ( p < 1000000. ){
      return ( m_yFoIParam1[ station * m_nRegions + region ] +
               m_yFoIParam2[ station * m_nRegions + region ] *
               exp( - m_yFoIParam3[ station * m_nRegions + region ] 
                   * p / Gaudi::Units::GeV ) ) * dy;
   } else {
      return m_yFoIParam1[ station * m_nRegions + region ] * dy;
   }
}
