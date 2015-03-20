// $Id: $
// STL
#include <vector>
#include <algorithm>
#include <memory>

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

using std::vector;
using std::less;
const std::array<unsigned int,4> order{{ 2u, 3u, 4u, 1u }};

//=============================================================================
MatchVeloTTMuon::MatchVeloTTMuon( const std::string& type, const std::string& name,
                              const IInterface* parent )
   : base_class( type, name, parent )
   , m_fieldSvc{nullptr}
   , m_magnetHit{nullptr}
{
   declareInterface<ITracksFromTrack>( this ); 

   declareProperty( "FoIToleranceX",    m_FoIToleranceX = 6.  );
   declareProperty( "FoIToleranceY",    m_FoIToleranceY = 1.5 );
 
   declareProperty( "MaxChi2DoFX",       m_maxChi2DoFX = 30          );
   declareProperty( "Chi2withVertPlane", m_chi2withVertPlane = false );
 
   declareProperty( "MaxMissedHits", m_maxMissed = 1 );

   declareProperty( "SetQOverP", m_setQOverP = false );
   setProduceHistos( false ); // yes, this indeed changes the default ;-)

   // Configurables for veloTT candidates w/o TT hits
     declareProperty( "XWindow", m_xWindow = 300 );
     declareProperty( "YWindow", m_yWindow = 300 );

     declareProperty( "MinMomentum", m_minMomentum = 6 * Gaudi::Units::GeV );
     declareProperty( "KickScale",  m_kickScale = 1255 * Gaudi::Units::MeV );
     declareProperty( "KickOffset", m_kickOffset = 175 * Gaudi::Units::MeV );
     
     // Focal plane parametrisation from B. Hommels
     declareProperty( "MagnetPlaneParA", m_za = 1.069 * Gaudi::Units::m );
     declareProperty( "MagnetPlaneParB", m_zb = 5.203 * Gaudi::Units::m );
}

//=============================================================================
StatusCode MatchVeloTTMuon::initialize()
{
   StatusCode sc = GaudiHistoTool::initialize();
   if ( sc.isFailure() ) return sc;
 
   // init hit manager
   m_hitManager = tool<CommonMuonHitManager>( "CommonMuonHitManager" );
   
   // init track extrapolator
   m_extrapolator = tool<ITrackExtrapolator>( "TrackParabolicExtrapolator" );

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
                                           std::vector<LHCb::Track*>& tracks )
{
    // Clean start
    i_clean();

      //  There are veloTT candidates w/o TT hits: 
      //  For the moment bypass this by checking with the following if statement
      //  and spliting i_findSeeds, i_addHits, in i_findSeeds_noTT, i_addHitoTT.
      //  Thus in each case, TT/noTT, a different implementation is called.
      //  This is the best strategy that I could come up with that does not require
      //  a new Interface for MatchVeloTTMuon. Later we try to find something better.

    // Check if track has a state at TT.
    if( seed.hasTT() ) { 
        m_trackStateLoc = LHCb::State::AtTT ;
      
	Candidate veloSeed{ &seed, m_trackStateLoc }; 

	unsigned int seedStation = order[0] ;
    
	i_findSeeds( veloSeed, seedStation );

	for ( Candidate& c : m_seeds ) i_addHits( c );
    }
    // if not fall back to MatchVeloMuon
    else {
        warning()<<"MatchVeloTTMuon::tracksFromTrack(): No state at TT found. Fall back to MatchVeloMuon."<< endmsg;

	m_trackStateLoc = LHCb::State::EndVelo ;
      
	Candidate veloSeed{ &seed }; 

	unsigned int seedStation = order[0] ;
    
	i_findSeeds_noTT( veloSeed, seedStation );

	for ( Candidate& c : m_seeds ) i_addHits_noTT( c );
    }

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

    // in that case we do not apply any chi sq cut and return tracks inside FoI
    if ( m_maxChi2DoFX == -1 ){
      if ( !m_seeds.empty() ) tracks.push_back( const_cast<LHCb::Track*>( &seed ) );
      return StatusCode::SUCCESS;
    }

    // in this case, we only care whether a good seed exists for the specified track...
    if ( !m_setQOverP ) {
        if ( std::any_of( std::begin(m_seeds), std::end(m_seeds), [=](const Candidate& c) {
            return c.chi2DoF() < m_maxChi2DoFX ;
        }) ) {
            // There is a good enough candidate, put the seed into the output unmodified.
            tracks.push_back( const_cast<LHCb::Track*>( &seed ) );
        }
    } else {

   //TODO: Is the momentum information returned from MatchVeloTTMuon used by the forward tracker?
   //      if yes then instead of the best, shouldn't we take everything below m_maxChiDoFX??
      auto best =
            std::min_element( std::begin(m_seeds), std::end(m_seeds),
                              []( const Candidate& lhs, const Candidate& rhs ) {
                return lhs.chi2DoF() < rhs.chi2DoF();
            } );
        if ( best != std::end(m_seeds) && best->chi2DoF() < m_maxChi2DoFX ) {
            std::unique_ptr<LHCb::Track> out{seed.clone()};
            out->addToAncestors( seed );
            out->addInfo( 35, best->slope() - best->tx() );
            double down = m_fieldSvc->isDown() ? -1 : 1;
            double q = down * ( ( best->slope() < best->tx() ) - ( best->slope() > best->tx() ) );
            LHCb::State* state = out->stateAt( m_trackStateLoc );
            state->setQOverP( q / best->p() );
            tracks.push_back( out.release() );
        }
    }
    return StatusCode::SUCCESS;
}

//=============================================================================
void MatchVeloTTMuon::i_findSeeds( const Candidate& veloSeed,
				   const unsigned int seedStation )
{
    // extrapolation from TT to M3 station
    const CommonMuonStation& station = m_hitManager->station( seedStation );
    
    const double zStation = station.z();
    const LHCb::State* state = veloSeed.track()->stateAt( m_trackStateLoc );
    LHCb::StateVector stateAtM3( state->stateVector(), state->z() );

    Gaudi::TrackMatrix propmatrix ;
    m_extrapolator->propagate(stateAtM3, zStation, &propmatrix) ; 

    // Find the maximum FoI to use as a search window.
    double maxFoIX = 0, maxFoIY = 0;
    for ( unsigned int region = 0; region < nRegions; ++region ) {
      auto foiX = m_FoIFactorX * FoIX( seedStation, region, stateAtM3.p() );
      auto foiY = m_FoIFactorY * FoIY( seedStation, region, stateAtM3.p() );
      if ( foiX > maxFoIX ) maxFoIX = foiX;
      if ( foiY > maxFoIY ) maxFoIY = foiY;
    }

    // FoI boundaries
    double xMin = stateAtM3.x() - maxFoIX;
    double xMax = stateAtM3.x() + maxFoIX;   
    double yMin = stateAtM3.y() - maxFoIY;
    double yMax = stateAtM3.y() + maxFoIY;

    // store foi information (this info is used for visaling the M3 foi)
    double field  = m_fieldSvc->isDown() ? -1 :  1;
    double charge = state->qOverP() > 0  ?  1 : -1;
    m_foiInfo[0] = std::make_pair( stateAtM3.x(),stateAtM3.y() );
    m_foiInfo[1] = std::make_pair( xMin,xMax );
    m_foiInfo[2] = std::make_pair( yMin,yMax );
    m_foiInfo[3] = std::make_pair( charge, field );

    // debug info: print hits in M3
    if ( msgLevel( MSG::DEBUG ) ) {
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

	seed.slope()  = stateAtM3.tx() ;
	seed.slopeY() = stateAtM3.ty() ;
	seed.p() = 1/std::abs( stateAtM3.qOverP() ) ;

        }
    }
}

//=============================================================================
// method for veloTT candidateds w/o TT hits. (This is a copy of MatchVeloMuon::i_findSeeds )
void MatchVeloTTMuon::i_findSeeds_noTT( const Candidate& veloSeed,
				   const unsigned int seedStation )
{

    // forward extrapolation, make seed point
    double zMagnet = m_zb + m_za * veloSeed.tx2();
    double xMagnet = 0., errXMagnet = 0.;
    veloSeed.xStraight( zMagnet, xMagnet, errXMagnet );
    double yMagnet = 0., errYMagnet = 0.;
    veloSeed.yStraight( zMagnet, yMagnet, errYMagnet );

    LHCb::MuonTileID id;
    m_magnetHit.reset( new CommonMuonHit( id, xMagnet, errXMagnet, yMagnet, errYMagnet, zMagnet, 0., false ) );

    double dSlope = dtx( m_minMomentum );
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
    
    // store foi information
    m_foiInfo[0] = std::make_pair( (xMax-xMin)/2.,yMuon );
    m_foiInfo[1] = std::make_pair( xMin,xMax );
    m_foiInfo[2] = std::make_pair( yMin,yMax );
    m_foiInfo[3] = std::make_pair( 0, 0 );

    if ( msgLevel( MSG::DEBUG ) ) {
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
      if ( !station.overlaps( r, xMin, xMax, yMin, yMax ) ) continue;

        for (const auto& hit : m_hitManager->hits( xMin, xMax, seedStation, r ) ) {
	  if ( hit.y() > yMax || hit.y() < yMin ) continue;

            m_seeds.emplace_back ( veloSeed );
            Candidate& seed = m_seeds.back();
            seed.addHit( m_magnetHit.get() );
            seed.addHit( &hit );
            seed.slope() = ( hit.x() - xMagnet ) / ( hit.z() - zMagnet );
            seed.p() = momentum( seed.slope() - seed.tx() );

        }
    }
}

//=============================================================================
void MatchVeloTTMuon::i_addHits( Candidate& seed )
{    
    unsigned int nMissed = 0;
    for ( unsigned int i = 1; i < order.size() && nMissed <= m_maxMissed; ++i ) {
        // find candidate hits
        unsigned int s = order[i] ;

        // get the station we're looking at.
        const CommonMuonStation& station = m_hitManager->station( s );
        double zStation = station.z();
	
	// get last hit of current seed
	const CommonMuonHit* last_hit = seed.hits().back();

	// str. line extrapolation from last hit position to the current station 
	double dz = zStation - last_hit->z() ;
	const double yMuon = last_hit->y() + dz * seed.slopeY();
	const double xMuon = last_hit->x() + dz * seed.slope() ;

	// Find the maximum FoI to use as a search window.
	double maxFoIX = 0, maxFoIY = 0;
	for ( unsigned int region = 0; region < nRegions; ++region ) {
	  auto foiX = m_FoIFactorX * FoIX( s, region, seed.p() );
	  auto foiY = m_FoIFactorY * FoIY( s, region, seed.p() );
	  if ( foiX > maxFoIX ) maxFoIX = foiX;
	  if ( foiY > maxFoIY ) maxFoIY = foiY;
	}

	// fois in subsequent stations are not scaled up, since the momentum estimate is improved after M3 seeding 
	// y window
        const double yMin  = yMuon - maxFoIY / m_FoIToleranceY;
        const double yMax  = yMuon + maxFoIY / m_FoIToleranceY;

	// x window
	const double xMin = xMuon - maxFoIX / m_FoIToleranceX;
        const double xMax = xMuon + maxFoIX / m_FoIToleranceX;

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
    if ( nMissed <= m_maxMissed ) i_fitCandidate( seed );
}

//=============================================================================
// method for veloTT candidateds w/o TT hits. ( This is a copy of MatchVeloMuon::i_addHits )
void MatchVeloTTMuon::i_addHits_noTT( Candidate& seed )
{
    // First hit is in magnet
    double zMagnet = m_magnetHit->z();
    double xMagnet = m_magnetHit->x();

    unsigned int nMissed = 0;
    for ( unsigned int i = 1; i < order.size() && nMissed <= m_maxMissed; ++i ) {
        // find candidate hits
        unsigned int s = order[i] ;

        // Get the station we're looking at.
        const CommonMuonStation& station = m_hitManager->station( s );
        double zStation = station.z();

        // Calculate window in x and y for this station
        double yMuon = 0., yRange = 0;
        seed.yStraight( zStation, yMuon, yRange );
        yRange = m_yWindow; // TODO: isn't this supposed to by yRange += m_yWindow ??

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
    if ( nMissed <= m_maxMissed ) i_fitCandidate( seed );
}

//=============================================================================
void MatchVeloTTMuon::i_fitCandidate( Candidate& candidate ) const
{
  if ( m_chi2withVertPlane ) { 
        const CommonConstMuonHits& hits = candidate.hits();

        double sumWeightsX = 0., sumZ_x = 0., sumX = 0.;
	double sumWeightsY = 0., sumZ_y = 0., sumY = 0.;

        for ( const CommonMuonHit* hit : hits ) {
	    // xz plane
            double dx = hit->dx();
            double weightX = 4.0 / ( dx * dx );
	    sumWeightsX += weightX;
            sumZ_x += hit->z() * weightX; 
	    sumX   += hit->x() * weightX;
	    // yz plane
	    double dy = hit->dy();
	    double weightY = 4.0 / ( dy * dy );
	    sumWeightsY += weightY;
            sumZ_y += hit->z() * weightY; 
	    sumY   += hit->y() * weightY;

        }
        double ZOverWeightsX = sumZ_x / sumWeightsX;
	double ZOverWeightsY = sumZ_y / sumWeightsY;

        double sumTmp2X = 0., sumTmp2Y = 0.;
        double b_xz = 0., b_yz = 0.;
        for ( const CommonMuonHit* hit : hits ) {
            // xz plane
	    double errX = hit->dx() / 2.;
            double tmpX = ( hit->z() - ZOverWeightsX ) / errX;
            sumTmp2X += tmpX * tmpX;
            b_xz += tmpX * hit->x() / errX;
	    // yz plane
            double errY = hit->dy() / 2.;
            double tmpY = ( hit->z() - ZOverWeightsY ) / errY;
            sumTmp2Y += tmpY * tmpY;
            b_yz += tmpY * hit->y() / errY;
        }
        
        b_xz /= sumTmp2X;
	b_yz /= sumTmp2Y;
        double a_xz = ( sumX - sumZ_x * b_xz ) / sumWeightsX;
        double a_yz = ( sumY - sumZ_y * b_yz ) / sumWeightsY;
        // double errA = sqrt( ( 1. + sumZ * sumZ / ( sumWeights * sumTmp2 ) ) /
        // sumWeights );
        // double errB = sqrt( 1. / sumTmp2 );
        
        double chi2 = 0.;
        for ( const CommonMuonHit* hit : hits ) {
	    // xz plane
            double errX = hit->dx() / 2.;
            double tmpX = ( hit->x() - a_xz - b_xz * hit->z() ) / errX;
            chi2 += tmpX * tmpX;
	    // yz plane
            double errY = hit->dy() / 2.;
            double tmpY = ( hit->y() - a_yz - b_yz * hit->z() ) / errY;
            chi2 += tmpY * tmpY;

        }
        
        candidate.nDoF() = hits.size() - 2;
        candidate.chi2() = chi2;
        candidate.slope() = b_xz;
        candidate.p() = momentum( b_xz - candidate.tx() );
  }

  else {
    const CommonConstMuonHits& hits = candidate.hits();

    double sumWeights = 0., sumZ = 0., sumX = 0.;

    for ( const CommonMuonHit* hit : hits ) {
        double dx = hit->dx();
        double weight = 4.0 / ( dx * dx );
        sumWeights += weight;
        sumZ += hit->z() * weight;
        sumX += hit->x() * weight;
    }
    double ZOverWeights = sumZ / sumWeights;

    double sumTmp2 = 0.;
    double b = 0.;
    for ( const CommonMuonHit* hit : hits ) {
        double err = hit->dx() / 2.;
        double tmp = ( hit->z() - ZOverWeights ) / err;
        sumTmp2 += tmp * tmp;
        b += tmp * hit->x() / err;
    }

    b /= sumTmp2;
    double a = ( sumX - sumZ * b ) / sumWeights;

    double chi2 = 0.;
    for ( const CommonMuonHit* hit : hits ) {
        double err = hit->dx() / 2.;
        double tmp = ( hit->x() - a - b * hit->z() ) / err;
        chi2 += tmp * tmp;
    }

    candidate.nDoF() = hits.size() - 2;
    candidate.chi2() = chi2;
    candidate.slope() = b;
    candidate.p() = momentum( b - candidate.tx() );

  }
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
void MatchVeloTTMuon::i_clean()
{
    // delete leftover seeds
    m_seeds.clear(); // leaves capacity invariant, hence we latch onto the max size
}

//=============================================================================
void MatchVeloTTMuon::findSeeds( const Candidate& seed, const unsigned int seedStation )
{
  warning()<<"Call findSeeds"<<endmsg;

  if ( m_trackStateLoc == LHCb::State::AtTT ){
    i_findSeeds( seed, seedStation );
  }
  else {
    i_findSeeds_noTT( seed, seedStation );
  }
}

//=============================================================================
void MatchVeloTTMuon::addHits( Candidate& seed )
{
  warning()<<"Call addHits"<<endmsg;
  if ( m_trackStateLoc == LHCb::State::AtTT ){
    i_addHits( seed );
  }
  else {
    i_addHits_noTT( seed );
  }
  
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
