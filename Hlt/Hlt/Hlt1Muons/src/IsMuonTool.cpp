// $Id: $
// STL
#include <set>

// boost
#include <boost/foreach.hpp>

// from Gaudi
#include <GaudiKernel/ToolFactory.h>
#include <DetDesc/Condition.h>

// LHCb
#include <Event/Track.h>
#include <Kernel/MuonTileID.h>

// Muon Detector
#include <MuonDet/DeMuonDetector.h>

// local
#include "IsMuonTool.h"
#include "Hlt1MuonHit.h"
#include "Hlt1MuonHitManager.h"
#include "Hlt1MuonStation.h"

//-----------------------------------------------------------------------------
// Implementation file for class : IsMuonTool
//
// 2011-01-19 : Roel Aaij
//-----------------------------------------------------------------------------

using std::vector;
using std::set;
using std::string;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( IsMuonTool );


//=============================================================================
IsMuonTool::IsMuonTool( const string& type, const string& name,
                        const IInterface* parent )
    : GaudiTool( type, name, parent )
    , m_foiFactor( 1. )
    , m_occupancy( 5, 0 )
    , m_regionHits( 4 )
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
        m_regionInnerX[s] = m_det->getInnerX( s );
        m_regionOuterX[s] = m_det->getOuterX( s );
        m_regionInnerY[s] = m_det->getInnerY( s );
        m_regionOuterY[s] = m_det->getOuterY( s );

        m_stationZ[s] = m_det->getStationZ( s );

        for ( unsigned int r = 0; r < nRegions; ++r ) {
            m_padSizeX[s * nRegions + r] = m_det->getPadSizeX( s, r );
            m_padSizeY[s * nRegions + r] = m_det->getPadSizeY( s, r );
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
IsMuonTool::~IsMuonTool()
{
}

//=============================================================================
StatusCode IsMuonTool::tracksFromTrack( const LHCb::Track& track,
                                        std::vector<LHCb::Track*>& tracks )
{
    // Clear temporary storage
    m_occupancy.assign( 5, 0 );
    m_trackX.clear();
    m_trackY.clear();

    // do the track extrapolations
    extrapolateTrack( track );

    // track is in acceptance? Track has minimum momentum?
    if ( !preSelection( track ) ) return StatusCode::SUCCESS;

    // find the coordinates in the fields of interest
    findHits( track );

    if ( isMuon( track.p() ) ) {
        // Add found hits to track
        LHCb::Track* output = track.clone();
        tracks.push_back( output );
        for ( const Hlt1MuonHit* hit : m_hits ) {
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
        m_trackX.push_back( state.x() +
                            state.tx() * ( m_stationZ[station] - state.z() ) );
        m_trackY.push_back( state.y() +
                            state.ty() * ( m_stationZ[station] - state.z() ) );
    }
}

//=============================================================================
bool IsMuonTool::preSelection( const LHCb::Track& track ) const
{
    // compare momentum and position to acceptance
    if ( track.p() < m_preSelMomentum ) return false;

    // Outer acceptance
    if ( !( fabs( m_trackX[0] ) < m_regionOuterX[0] &&
            fabs( m_trackY[0] ) < m_regionOuterY[0] ) ||
         !( fabs( m_trackX[nStations - 1] ) < m_regionOuterX[nStations - 1] &&
            fabs( m_trackY[nStations - 1] ) < m_regionOuterY[nStations - 1] ) ) {
        // outside M1 - M5 region
        return false;
    }

    // Inner acceptance
    if ( ( fabs( m_trackX[0] ) < m_regionInnerX[0] &&
           fabs( m_trackY[0] ) < m_regionInnerY[0] ) ||
         ( fabs( m_trackX[nStations - 1] ) < m_regionInnerX[nStations - 1] &&
           fabs( m_trackY[nStations - 1] ) < m_regionInnerY[nStations - 1] ) ) {
        // inside M1 - M5 chamber hole
        return false;
    }

    return true;
}

//=============================================================================
void IsMuonTool::findHits( const LHCb::Track& track )
{
    // clear previously stored hits
    m_hits.clear();

    // Start from 1 because M1 does not matter for IsMuon
    for ( unsigned int s = 1; s < nStations; ++s ) {
        const auto& station = m_hitManager->station( s );

        // TODO/FIXME:  why is m_regionHits not a local variable??
        for ( auto& hits : m_regionHits ) hits.clear();

        // Convert Hlt1 regions to standard muon station regions
        for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
            // Get hits
            for ( auto* hit : m_hitManager->hits( -m_regionOuterX[s], s, r ) ) {
                m_regionHits[hit->tile().region()].push_back( hit );
            }
        }

        for ( unsigned int region = 0; region < nRegions; ++region ) {
            const double foiXDim = m_foiFactor * foiX( s, region, track.p() );
            const double foiYDim = m_foiFactor * foiY( s, region, track.p() );

            for ( auto hit : m_regionHits[region] ) {
                double x = hit->x();
                double y = hit->y();

                // check if the hit is in the window
                if ( ( fabs( x - m_trackX[s] ) < foiXDim ) &&
                     ( fabs( y - m_trackY[s] ) < foiYDim ) ) {
                    m_hits.push_back( hit );
                    m_occupancy[s] += 1;
                }
            } // hits
        }     // region
    }         // station
}

//=============================================================================
bool IsMuonTool::isMuon( const double p ) const
{
    const double pr1 = m_preSelMomentum;
    const double pr2 = m_momentumCuts[0];
    const double pr3 = m_momentumCuts[1];

    set<unsigned int> stations;
    for ( unsigned int i = 1; i < m_occupancy.size(); ++i ) {
        if ( m_occupancy[i] != 0 ) stations.insert( i );
    }

    bool id = false;

    if ( p > pr1 && p < pr2 ) {
        if ( stations.count( 1 ) && stations.count( 2 ) ) id = true;
    } else if ( p > pr2 && p < pr3 ) {
        if ( stations.count( 1 ) && stations.count( 2 ) &&
             ( stations.count( 3 ) || stations.count( 4 ) ) )
            id = true;
    } else if ( p > pr3 ) {
        if ( stations.count( 1 ) && stations.count( 2 ) && stations.count( 3 ) &&
             stations.count( 4 ) )
            id = true;
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
double IsMuonTool::foiX( const int station, const int region, const double p ) const
{
    double dx = m_padSizeX[station * nRegions + region] / 2.;

    if ( p < 1000000. ) {
        return ( m_xfoiParam1[station * nRegions + region] +
                 m_xfoiParam2[station * nRegions + region] *
                     exp( -m_xfoiParam3[station * nRegions + region] * p /
                          Gaudi::Units::GeV ) ) *
               dx;
    } else {
        return m_xfoiParam1[station * nRegions + region] * dx;
    }
}

//=============================================================================
double IsMuonTool::foiY( const int station, const int region, const double p ) const
{
    double dy = m_padSizeY[station * nRegions + region] / 2.;

    if ( p < 1000000. ) {
        return ( m_yfoiParam1[station * nRegions + region] +
                 m_yfoiParam2[station * nRegions + region] *
                     exp( -m_yfoiParam3[station * nRegions + region] * p /
                          Gaudi::Units::GeV ) ) *
               dy;
    } else {
        return m_yfoiParam1[station * nRegions + region] * dy;
    }
}
