// $Id: $

// from Gaudi
#include <GaudiKernel/ToolFactory.h>
#include <DetDesc/Condition.h>

// LHCb
#include <Event/Track.h>
#include <Kernel/MuonTileID.h>

// Muon Detector
#include <MuonDet/DeMuonDetector.h>

// Hlt1Muons
#include <Hlt1Muons/Hlt1MuonHit.h>

// local
#include "IsMuonTool.h"
#include "Hlt1MuonHitManager.h"
#include "Hlt1MuonStation.h"

//-----------------------------------------------------------------------------
// Implementation file for class : IsMuonTool
//
// 2011-01-19 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( IsMuonTool )


//=============================================================================
IsMuonTool::IsMuonTool( const std::string& type, const std::string& name,
                        const IInterface* parent )
    : GaudiTool( type, name, parent )
    , m_foiFactor( 1. )
{
    declareInterface<ITracksFromTrack>( this );
}

//=============================================================================
StatusCode IsMuonTool::initialize()
{
    StatusCode sc = GaudiTool::initialize();
    if ( sc.isFailure() ) return sc;

    m_hitManager = tool<Hlt1MuonHitManager>( "Hlt1MuonHitManager" );

    // fill local arrays of pad sizes and region sizes
    m_det = getDet<DeMuonDetector>( "/dd/Structure/LHCb/DownstreamRegion/Muon" );

    for ( unsigned int s = 0; s < nStations; ++s ) {
        m_regionInner[s] = std::make_pair( m_det->getInnerX( s ), m_det->getInnerY( s ) );
        m_regionOuter[s] = std::make_pair( m_det->getOuterX( s ), m_det->getOuterY( s ) );

        m_stationZ[s] = m_det->getStationZ( s );

        for ( unsigned int r = 0; r < nRegions; ++r ) {
            m_padSize[s * nRegions + r] = std::make_pair( m_det->getPadSizeX( s, r ), m_det->getPadSizeY( s, r ) );
        }
    }

    // Condition pointers
    Condition* foiFactor = nullptr;
    Condition* xFOIParameters = nullptr;
    Condition* yFOIParameters = nullptr;
    Condition* preSelMomentum = nullptr;
    Condition* momentumCuts = nullptr;

    // Register conditions
    registerCondition<IsMuonTool>( "Conditions/ParticleID/Muon/FOIfactor",
                                   foiFactor );
    registerCondition<IsMuonTool>( "Conditions/ParticleID/Muon/XFOIParameters",
                                   xFOIParameters );
    registerCondition<IsMuonTool>( "Conditions/ParticleID/Muon/YFOIParameters",
                                   yFOIParameters );
    registerCondition<IsMuonTool>( "Conditions/ParticleID/Muon/PreSelMomentum",
                                   preSelMomentum );
    registerCondition<IsMuonTool>( "Conditions/ParticleID/Muon/MomentumCuts",
                                   momentumCuts );

    // Update conditions
    sc = runUpdate();
    if ( sc.isFailure() ) {
        Error( "Could not update conditions from the CondDB" );
        return sc;
    }

    // Get the parameters
    m_foiFactor = foiFactor->param<double>( "FOIfactor" );
    m_xfoiParam1 = xFOIParameters->paramVect<double>( "XFOIParameters1" );
    m_xfoiParam2 = xFOIParameters->paramVect<double>( "XFOIParameters2" );
    m_xfoiParam3 = xFOIParameters->paramVect<double>( "XFOIParameters3" );

    m_yfoiParam1 = yFOIParameters->paramVect<double>( "YFOIParameters1" );
    m_yfoiParam2 = yFOIParameters->paramVect<double>( "YFOIParameters2" );
    m_yfoiParam3 = yFOIParameters->paramVect<double>( "YFOIParameters3" );

    m_preSelMomentum = preSelMomentum->param<double>( "PreSelMomentum" );

    m_momentumCuts = momentumCuts->paramVect<double>( "MomentumCuts" );

    return sc;
}

//=============================================================================
StatusCode IsMuonTool::tracksFromTrack( const LHCb::Track& track,
                                        std::vector<LHCb::Track*>& tracks )
{
    // do the track extrapolations
    extrapolateTrack( track );

    // track is in acceptance? Track has minimum momentum?
    if ( !preSelection( track ) ) return StatusCode::SUCCESS;

    // find the coordinates in the fields of interest
    auto hits = findHits( track );

    if ( isMuon( track.p() ) ) {
        // Add found hits to track
        LHCb::Track* output = track.clone();
        // clone copies all ancestors and extrainfo, which is not what we want in HLT
        output->clearAncestors();
        output->setExtraInfo(LHCb::Track::ExtraInfo());
        tracks.push_back( output );
        for ( const auto& hit : hits ) {
            output->addToLhcbIDs( LHCb::LHCbID{hit->tile()} );
        }
    }
    return StatusCode::SUCCESS;
}

//=============================================================================
void IsMuonTool::extrapolateTrack( const LHCb::Track& track )
{

    // get state closest to M1 for extrapolation
    const LHCb::State& state = track.closestState( 9450. );

    // Project the state into the muon stations
    for ( unsigned int station = 0; station < nStations; ++station ) {
        // x(z') = x(z) + (dx/dz * (z' - z))
        m_track[station] = {  state.x() + state.tx() * ( m_stationZ[station] - state.z() ) ,
                              state.y() + state.ty() * ( m_stationZ[station] - state.z() ) };
    }
}

//=============================================================================
bool IsMuonTool::preSelection( const LHCb::Track& track ) const
{
    // compare momentum and position to acceptance
    if ( track.p() < m_preSelMomentum ) return false;

    using xy_t = std::pair<double,double>;
    auto abs_lt = [](const xy_t& p1, const xy_t& p2) { 
        return fabs( p1.first ) < p2.first && fabs( p1.second) < p2.second; 
    } ;

    // Outer acceptance
    if ( ! abs_lt( m_track[0], m_regionOuter[0] ) ||
         ! abs_lt( m_track[nStations - 1],  m_regionOuter[nStations - 1] ) )  {
        // outside M1 - M5 region
        return false;
    }

    // Inner acceptance
    if ( abs_lt(  m_track[0], m_regionInner[0]  ) ||
         abs_lt(  m_track[nStations - 1],  m_regionInner[nStations - 1] ) ) {
        // inside M1 - M5 chamber hole
        return false;
    }

    return true;
}

//=============================================================================
Hlt1ConstMuonHits IsMuonTool::findHits( const LHCb::Track& track )
{
    using xy_t = std::pair<double,double>;

    class is_in_window {
        xy_t m_center;
        std::array<xy_t,4> m_foi;
    public:
        is_in_window( xy_t center
                    , xy_t foi0 , xy_t foi1 , xy_t foi2 , xy_t foi3 
                    , double sf ) 
            : m_center{std::move(center)}, 
              m_foi{{ std::move(foi0), std::move(foi1), std::move(foi2), std::move(foi3) }}
            {
                std::for_each( std::begin( m_foi ), std::end(m_foi),
                               [=](xy_t& p) { 
                                   p.first *= sf ; p.second *= sf; 
                } );
            }
        bool operator()(const Hlt1MuonHit& hit) const {
                auto region = hit.tile().region();
                assert(region<4);
                return ( fabs( hit.x() - m_center.first  ) < m_foi[region].first ) &&
                       ( fabs( hit.y() - m_center.second ) < m_foi[region].second ) ;
        }
    };

    auto is_in_window_ = [&](unsigned s) {
        auto p = track.p();
        return is_in_window{ m_track[s], foi(s,0,p), foi(s,1,p), 
                                         foi(s,2,p), foi(s,3,p), 
                                         m_foiFactor } ;
    };

    Hlt1ConstMuonHits hits;
    m_occupancy[0] = 0;
    // Start from 1 because M1 does not matter for IsMuon
    for ( unsigned int s = 1; s < nStations; ++s ) {
        // prepare the predicates for this station (and current track)
        auto predicate =  is_in_window_(s) ;
        // Convert Hlt1 regions to standard muon station regions
        const auto& station = m_hitManager->station( s );
        for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
            auto hr =  m_hitManager->hits( -m_regionOuter[s].first, s, r ); // TODO: no xMax?? +m_regionOuter[s].first ?? Maybe drop xMin???
            std::for_each( std::begin(hr), std::end(hr), [&](const Hlt1MuonHit& hit) {
                if (predicate(hit)) hits.push_back(&hit);
            } );
        }  // region
        m_occupancy[s] = hits.size(); // at this point: cumulative over stations upto s
    } // station
    // turn into cumulative occupancy into occupancy  per station
    std::adjacent_difference( std::begin(m_occupancy), std::end(m_occupancy), 
                              std::begin(m_occupancy) );
    return hits;
}

//=============================================================================
bool IsMuonTool::isMuon( double p ) const
{
    const double pr1 = m_preSelMomentum;
    const double pr2 = m_momentumCuts[0];
    const double pr3 = m_momentumCuts[1];

    auto has = [&]( unsigned i ) { return m_occupancy[i]!=0; };

    bool _12 = has( 1 ) && has( 2 )  ;

    bool id = false;

    if ( p < pr1 ) {
        id = false;
    } else if ( p < pr2 ) {
        id =  _12  ;
    } else if ( p < pr3 ) {
        id =  _12 && ( has( 3 ) || has( 4 ) ) ;
    } else {
        id =  _12 && has( 3 ) && has( 4 );
    } 

    if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "IsMuon = " << id << endmsg;
        debug() << "pr1=" << pr1 << endmsg;
        debug() << "pr2=" << pr2 << endmsg;
        debug() << "pr3=" << pr3 << endmsg;
        debug() << "IsMuon p=" << p << endmsg;
    }
    return id;
}

//=============================================================================
std::pair<double,double> IsMuonTool::foi( int station, int region, double p ) const
{
    auto i = station * nRegions + region;
    double dx = 0.5 * m_padSize[i].first;
    double dy = 0.5 * m_padSize[i].second;

    if ( p < 1000000. ) {
        auto p_GeV = p / Gaudi::Units::GeV;
        return{ ( m_xfoiParam1[i] +
                  m_xfoiParam2[i] * exp( -m_xfoiParam3[i] * p_GeV ) ) * dx,
                ( m_yfoiParam1[i] + 
                  m_yfoiParam2[i] * exp( -m_yfoiParam3[i] * p_GeV ) ) * dy };
    } else {
        return { m_xfoiParam1[i] * dx,
                 m_yfoiParam1[i] * dy };
    }
}
