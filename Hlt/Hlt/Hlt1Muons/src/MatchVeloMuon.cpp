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

// Hlt1Muons
#include <Hlt1Muons/Candidate.h>

// local
#include "MatchVeloMuon.h"

// from MuonID
#include "MuonID/CommonMuonHitManager.h"
#include "MuonID/CommonMuonStation.h"
#include <MuonID/CommonMuonHit.h>

//-----------------------------------------------------------------------------
// Implementation file for class : MatchVeloMuon
//
// 2010-12-02 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_COMPONENT( MatchVeloMuon )

using std::vector;
using std::less;
const std::array<unsigned int,4> order{{ 2u, 3u, 4u, 1u }};

//=============================================================================
MatchVeloMuon::MatchVeloMuon( const std::string& type, const std::string& name,
                              const IInterface* parent )
   : base_class( type, name, parent )
   , m_fieldSvc{nullptr}
   , m_magnetHit{nullptr}
{
   declareInterface<ITracksFromTrack>( this ); 

   declareProperty( "XWindow", m_xWindow = 200 );
   declareProperty( "YWindow", m_yWindow = 400 );

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

}

//=============================================================================
StatusCode MatchVeloMuon::initialize()
{
   StatusCode sc = GaudiHistoTool::initialize();
   if ( sc.isFailure() ) return sc;

   m_hitManager = tool<CommonMuonHitManager>( "CommonMuonHitManager" );

   if ( m_setQOverP ) {
      // Magnetic Field
      m_fieldSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );
   }

   return sc;
}

//=============================================================================
StatusCode MatchVeloMuon::finalize()
{
   clean();
   return GaudiHistoTool::finalize();
}


//=============================================================================
StatusCode MatchVeloMuon::tracksFromTrack( const LHCb::Track& seed,
                                           std::vector<LHCb::Track*>& tracks ) const
{
    // Clean start
    i_clean();

    // Find seeds
    unsigned int seedStation = order[0] ;
    i_findSeeds( seed, seedStation );

    for ( Candidate& c : m_seeds ) i_addHits( c );

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

    if ( !m_setQOverP ) {
        // in this case, we only care whether a good seed exists for the specified track...
        if ( std::any_of( std::begin(m_seeds), std::end(m_seeds), [=](const Candidate& c) {
            return c.chi2DoF() < m_maxChi2DoFX ;
        }) ) {
            // There is a good enough candidate, put the seed into the output
            // unmodified.
            tracks.push_back( const_cast<LHCb::Track*>( &seed ) );
        }
    } else {
        //TODO: instead of the best, shouldn't we take everything below m_maxChiDoFX??
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
            LHCb::State* state = out->stateAt( LHCb::State::EndVelo );
            state->setQOverP( q / best->p() );
            tracks.push_back( out.release() );
        }
    }
    return StatusCode::SUCCESS;
}

//=============================================================================
void MatchVeloMuon::i_findSeeds( const LHCb::Track& seed,
                                 const unsigned int seedStation ) const
{
    // Make a Candidate from the track
    Candidate veloSeed{ &seed }; 

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
        if ( !station.overlaps( r, xMin, xMax, yMin, yMax ) ) continue; //TODO: push into loop control -- request from station to a range of regions to loop over...

        // get hits, and add seed hits to container
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
void MatchVeloMuon::i_addHits( Candidate& seed ) const
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
void MatchVeloMuon::i_fitCandidate( Candidate& candidate ) const
{
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
    // double errA = sqrt( ( 1. + sumZ * sumZ / ( sumWeights * sumTmp2 ) ) /
    // sumWeights );
    // double errB = sqrt( 1. / sumTmp2 );

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

//=============================================================================
void MatchVeloMuon::i_clean() const
{
    // delete leftover seeds
    m_seeds.clear(); // leaves capacity invariant, hence we latch onto the max size
    m_magnetHit.reset();
}

//=============================================================================
void MatchVeloMuon::findSeeds( const LHCb::Track& seed, const unsigned int seedStation )
{
   i_findSeeds( seed, seedStation );
}

//=============================================================================
void MatchVeloMuon::addHits( Candidate& seed )
{
    i_addHits(seed);
}

//=============================================================================
void MatchVeloMuon::fitCandidate( Candidate& seed ) const
{
    i_fitCandidate(seed);
}

//=============================================================================
void MatchVeloMuon::clean()
{
    i_clean();
}

