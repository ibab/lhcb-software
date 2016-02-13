// $Id: $
// STL
#include <vector>
#include <algorithm>

// from Gaudi
#include <GaudiKernel/ToolFactory.h>
#include <GaudiKernel/SystemOfUnits.h>
#include <DetDesc/Condition.h>

// from LHCb
#include <Kernel/ILHCbMagnetSvc.h>

// Muon Detector
#include <MuonDet/DeMuonDetector.h>

// Hlt1Muons
#include <Hlt1Muons/Candidate.h>

// local
#include "HltVeloIsMuon.h"

// from MuonID
#include "MuonID/CommonMuonHitManager.h"
#include "MuonID/CommonMuonStation.h"
#include <MuonID/CommonMuonHit.h>

//-----------------------------------------------------------------------------
// Implementation file for class : HltVeloIsMuon
//
// 2010-15-03 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( Hlt, HltVeloIsMuon )

using std::vector;
const std::array<unsigned int,4> order{{ 3u, 4u, 5u, 2u }};


//=============================================================================
Hlt::HltVeloIsMuon::HltVeloIsMuon( const std::string& type, const std::string& name,
                                   const IInterface* parent )
    : base_class( type, name, parent )
    , m_hitManager{nullptr}
    , m_fieldSvc{nullptr}
    , m_magnetHit{nullptr}
{
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
}

//=============================================================================
StatusCode Hlt::HltVeloIsMuon::initialize()
{
    StatusCode sc = GaudiHistoTool::initialize();
    if ( sc.isFailure() ) return sc;

    m_hitManager = tool<CommonMuonHitManager>( "CommonMuonHitManager" );

    // Magnetic Field
    m_fieldSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );

    // fill local arrays of pad sizes and region sizes
    m_det = getDet<DeMuonDetector>( "/dd/Structure/LHCb/DownstreamRegion/Muon" );

    m_padSizeX.resize( nStations * nRegions );
    m_padSizeY.resize( nStations * nRegions );

    for ( unsigned int s = 0; s < nStations; ++s ) {

        for ( unsigned int r = 0; r < nRegions; ++r ) {
            m_padSizeX[s * nRegions + r] = m_det->getPadSizeX( s, r );
            m_padSizeY[s * nRegions + r] = m_det->getPadSizeY( s, r );
        }
    }

    // Condition pointers
    Condition* FoIFactor = nullptr;
    Condition* xFOIParameters = nullptr;
    Condition* yFOIParameters = nullptr;
    Condition* preSelMomentum = nullptr;
    Condition* momentumCuts = nullptr;

    // Register conditions
    registerCondition<HltVeloIsMuon>( "Conditions/ParticleID/Muon/FOIfactor",
                                      FoIFactor );
    registerCondition<HltVeloIsMuon>( "Conditions/ParticleID/Muon/XFOIParameters",
                                      xFOIParameters );
    registerCondition<HltVeloIsMuon>( "Conditions/ParticleID/Muon/YFOIParameters",
                                      yFOIParameters );
    registerCondition<HltVeloIsMuon>( "Conditions/ParticleID/Muon/PreSelMomentum",
                                      preSelMomentum );
    registerCondition<HltVeloIsMuon>( "Conditions/ParticleID/Muon/MomentumCuts",
                                      momentumCuts );

    // Update conditions
    sc = runUpdate();
    if ( sc.isFailure() ) {
        Error( "Could not update conditions from the CondDB" );
        return sc;
    }

    // Get the parameters
    m_FoIFactor = m_FoITolerance * FoIFactor->param<double>( "FOIfactor" );
    m_xFoIParam1 = xFOIParameters->paramVect<double>( "XFOIParameters1" );
    m_xFoIParam2 = xFOIParameters->paramVect<double>( "XFOIParameters2" );
    m_xFoIParam3 = xFOIParameters->paramVect<double>( "XFOIParameters3" );

    m_yFoIParam1 = yFOIParameters->paramVect<double>( "YFOIParameters1" );
    m_yFoIParam2 = yFOIParameters->paramVect<double>( "YFOIParameters2" );
    m_yFoIParam3 = yFOIParameters->paramVect<double>( "YFOIParameters3" );

    m_preSelMomentum = preSelMomentum->param<double>( "PreSelMomentum" );

    m_momentumCuts = momentumCuts->paramVect<double>( "MomentumCuts" );

    return sc;
}

//=============================================================================
StatusCode Hlt::HltVeloIsMuon::finalize()
{
    clean();
    return GaudiHistoTool::finalize();
}

//=============================================================================
StatusCode Hlt::HltVeloIsMuon::tracksFromTrack( const LHCb::Track& seed,
                                                vector<LHCb::Track*>& tracks ) const
{
    // Clean start
    clean();

    // Make a Candidate from the track
    Candidate veloSeed{ &seed };

    unsigned int seedStation = order[0] - 1;
    findSeeds( veloSeed, seedStation );
    if ( produceHistos() ) plot( m_seeds.size(), "NSeedHits", -0.5, 50.5, 51 );

    for ( Candidate& c : m_seeds ) addHits( c );

    if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "Found " << m_seeds.size() << " seeds." << endmsg;
        for ( const Candidate& c : m_seeds ) {
            debug() << "Found candidate with chi2/DoF " << c.chi2DoF() << endmsg;
        }
    }

    m_seeds.erase( std::remove_if( std::begin(m_seeds), std::end(m_seeds), [](const Candidate& c) { return !c.fitted(); } ),
                   std::end(m_seeds) );

    if ( produceHistos() ) {
        for ( const Candidate& c : m_seeds ) plot( c.chi2DoF(), "Chi2DoFX", 0, 100, 100 );
    }

    if (!m_setQOverP) {
        if (std::any_of( std::begin(m_seeds), std::end(m_seeds), [=](const Candidate& c) {
              return  c.chi2DoF() < m_maxChi2DoFX ;
        } ) ) {
            // There is a good enough candidate, put the seed into the output
            // unmodified.
            tracks.push_back( const_cast<LHCb::Track*>( &seed ) );
        }
    } else {
        // TODO: should we use all candidates below m_maxChi2DoFX instead??
        auto best =
            std::min_element( std::begin(m_seeds), std::end(m_seeds),
                              []( const Candidate& lhs, const Candidate& rhs ) {
                return lhs.chi2DoF() < rhs.chi2DoF();
            } );
        if ( best != std::end(m_seeds) && best->chi2DoF() < m_maxChi2DoFX ) {
            auto out = std::unique_ptr<LHCb::Track>{seed.clone()};

            LHCb::State* state = out->stateAt( LHCb::State::EndVelo );
            double down = m_fieldSvc->isDown() ? -1 : 1;
            double q =
                down * ( ( best->slope() < best->tx() ) - ( best->slope() > best->tx() ) );
            state->setQOverP( q / best->p() );
            tracks.push_back( out.release() );
        }
    }
    return StatusCode::SUCCESS;
}

//=============================================================================
void Hlt::HltVeloIsMuon::findSeeds( const Candidate& veloSeed,
                                    const unsigned int seedStation ) const
{
    // forward extrapolation, make seed point
    double zMagnet = m_zb + m_za * veloSeed.tx2();
    double xMagnet = 0., errXMagnet = 0.;
    veloSeed.xStraight( zMagnet, xMagnet, errXMagnet );
    double yMagnet = 0., errYMagnet = 0.;
    veloSeed.yStraight( zMagnet, yMagnet, errYMagnet );

    m_magnetHit.reset(new CommonMuonHit{LHCb::MuonTileID{}, xMagnet, errXMagnet, yMagnet, errYMagnet, zMagnet, 0., false});

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
            CommonMuonHitRange hits = m_hitManager->hits( xMin, xMax, seedStation, r );
            for ( const auto& hit : hits ) {
                debug() << hit.x() << " " << hit.y() << endmsg;
            }
        }
        for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
            if ( !station.overlaps( r, xMin, xMax, yMin, yMax ) ) continue;
            // Get hits
            auto hits = m_hitManager->hits( xMin, xMax, seedStation, r );
            debug() << "Hits in seed region " << r << ":" << endmsg;
            for ( const auto& hit : hits ) {
                debug() << hit.x() << " " << hit.y() << endmsg;
            }
        }
    }

    for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
        if ( !station.overlaps( r, xMin, xMax, yMin, yMax ) ) continue;

        // add seed hits to container
        for ( const auto& hit : m_hitManager->hits( xMin, xMax, seedStation, r ) ) {
            if ( hit.y() > yMax || hit.y() < yMin ) continue;

            m_seeds.emplace_back(  veloSeed );
            Candidate& seed = m_seeds.back();
            seed.addHit( m_magnetHit.get() );
            seed.addHit( &hit );
            seed.slope() = ( hit.x() - xMagnet ) / ( hit.z() - zMagnet );
            seed.p() = momentum( seed.slope() - seed.tx() );
        }
    }
}

//=============================================================================
void Hlt::HltVeloIsMuon::addHits( Candidate& seed ) const
{
    // First hit is in magnet
    double zMagnet = m_magnetHit->z();
    double xMagnet = m_magnetHit->x();

    unsigned int nMissed = 0;
    for ( unsigned int i = 1; i < order.size(); ++i ) {
        // find candidate hits
        unsigned int s = order[i] - 1;

        // Get the station we're looking at.
        const CommonMuonStation& station = m_hitManager->station( s );
        double zStation = station.z();

        // Clear and cache region FoIs
        double maxFoIX = 0;
        double maxFoIY = 0;
        // Find the maximum FoI to use as a search window.
        for ( unsigned int region = 0; region < nRegions; ++region ) {
            auto foiX = m_FoIFactor * FoIX( s, region, seed.p() );
            auto foiY = m_FoIFactor * FoIY( s, region, seed.p() );
            if ( foiX > maxFoIX ) maxFoIX = foiX;
            if ( foiY > maxFoIY ) maxFoIY = foiY;
            m_regionFoIX[region] = foiX;
            m_regionFoIY[region] = foiY;
        }

        // Calculate window in x and y for this station
        double yMuon = 0., yRange = 0;
        seed.yStraight( zStation, yMuon, yRange );
        const double yMin = yMuon - maxFoIY;
        const double yMax = yMuon + maxFoIY;

        const double xMuon = ( zStation - zMagnet ) * seed.slope() + xMagnet;
        const double xMin = xMuon - maxFoIX;
        const double xMax = xMuon + maxFoIX;

        // Look for the closest hit inside the search window
        const CommonMuonHit* closest = nullptr;
        double minDist2 = 0;
        for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
            if ( !station.overlaps( r,  xMin, xMax, yMin, yMax ) ) continue; // TODO: move into loop control...
            for ( const auto& hit : m_hitManager->hits( xMin, xMax, s, r ) ) {
                // Take the actual FoI into account
                unsigned int r = hit.tile().region();
                auto dx = hit.x()-xMuon;
                auto dy = hit.y()-yMuon;
                if ( fabs(dx) > m_regionFoIX[r] ||
                     fabs(dy) > m_regionFoIY[r]  ) continue;
                auto dist2 = dx*dx + dy*dy ; 
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
        if ( nMissed > m_maxMissed ) break;
    }
    // If a new candidate is good, fit it.
    if ( nMissed <= m_maxMissed ) fitCandidate( seed );
}

//=============================================================================
void Hlt::HltVeloIsMuon::fitCandidate( Candidate& candidate ) const
{
    const CommonConstMuonHits& hits = candidate.hits();

    double sumWeights = 0., sumZ = 0., sumX = 0.;

    for ( const CommonMuonHit* hit : hits ) {
        double err = 0.5 * hit->dx();
        double weight = 1.0 / ( err * err );
        sumWeights += weight;
        sumZ += hit->z() * weight;
        sumX += hit->x() * weight;
    }
    double ZOverWeights = sumZ / sumWeights;

    double b = 0, sumTmp2 = 0;
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

    double chi2 = std::accumulate( std::begin(hits), std::end(hits), 0.0, 
                                   [=](double chi2, const CommonMuonHit* hit)  {
                                        double err = hit->dx() / 2.;
                                        double tmp = ( hit->x() - a - b * hit->z() ) / err;
                                        return chi2 += tmp * tmp;
    });

    candidate.fitted() = true;
    candidate.nDoF() = hits.size() - 2;
    candidate.chi2() = chi2;
    candidate.slope() = b;
    candidate.p() = momentum( b - candidate.tx() );
}

//=============================================================================
void Hlt::HltVeloIsMuon::clean() const
{
    // delete leftover seeds
    m_seeds.clear();
    m_magnetHit.reset();
}

//=============================================================================
double Hlt::HltVeloIsMuon::FoIX( const int station, const int region,
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
double Hlt::HltVeloIsMuon::FoIY( const int station, const int region,
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
