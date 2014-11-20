// $Id: $
// STL
#include <vector>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include <GaudiKernel/boost_allocator.h>

// Muon Detector
#include <MuonDet/IMuonFastPosTool.h>
#include <MuonDet/DeMuonDetector.h>
#include <Event/MuonCoord.h>

// local
#include "Hlt1MuonHitManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt1MuonHitManager
//
// 2010-12-07 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Hlt1MuonHitManager )

//=============================================================================
Hlt1MuonHitManager::Hlt1MuonHitManager( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
    : GaudiTool( type, name, parent )
    , m_muonDet{nullptr}
    , m_loaded{false}
{
    declareInterface<Hlt1MuonHitManager>( this );
    // declare properties
    declareProperty( "MuonCoordLocation",
                     m_coordLocation = LHCb::MuonCoordLocation::MuonCoords );
    m_prepared.reset();
}


//=============================================================================
StatusCode Hlt1MuonHitManager::initialize()
{
    StatusCode sc = GaudiTool::initialize();
    if ( sc.isFailure() ) return sc;

    incSvc()->addListener( this, IncidentType::BeginEvent );

    m_muonDet = getDet<DeMuonDetector>( "/dd/Structure/LHCb/DownstreamRegion/Muon" );
    auto stations = m_muonDet->stations();
    m_stations.reserve( stations );
    assert(stations<=5);

    // X region boundaries
    // Define the regions, the last ones are increased a little to make sure
    // everything is caught.
    const std::array<std::vector<double>, 5> x_regions{
        {{-3840, -1920, -960, -480, 480, 960, 1920, 3940},
         {-4800, -1200, -600, 600, 1200, 4900},
         {-5180, -1292, -644, 644, 1292, 5280},
         {-5556, -1392, -696, 696, 1392, 5656},
         {-5952, -1488, -744, 744, 1488, 6052}}};

    for ( int i = 0; i < stations; ++i ) {
        m_stations.emplace_back( m_muonDet, i, x_regions[i] );
    }
    return sc;
}

//=============================================================================
void Hlt1MuonHitManager::handle( const Incident& incident )
{
    if ( IncidentType::BeginEvent != incident.type() ) return;

    m_prepared.reset();
    m_loaded = false;
    std::for_each( std::begin(m_coords), std::end(m_coords), 
                   [](std::vector<const LHCb::MuonCoord*>& v){ v.clear(); } );
}

//=============================================================================
Hlt1MuonHitRange Hlt1MuonHitManager::hits( double xmin,
                                           unsigned int station,
                                           unsigned int region )
{
    if ( !m_prepared[station] ) prepareHits( station );
    return m_stations[station].hits( xmin, region );
}
//=============================================================================
Hlt1MuonHitRange Hlt1MuonHitManager::hits( double xmin, double xmax,
                                           unsigned int station,
                                           unsigned int region )
{
    if ( !m_prepared[station] ) prepareHits( station );
    return m_stations[station].hits( xmin, xmax, region );
}

//=============================================================================
unsigned int Hlt1MuonHitManager::nRegions( unsigned int station ) const
{
    return m_stations[station].nRegions();
}

//=============================================================================
const Hlt1MuonStation& Hlt1MuonHitManager::station( unsigned int id ) const
{
    return m_stations[id];
}

//=============================================================================
void Hlt1MuonHitManager::prepareHits( unsigned int station )
{
    if ( !m_loaded ) loadCoords();

    Hlt1MuonHits hits; hits.reserve( m_coords[station].size() );
    for ( const auto& coord : m_coords[station] ) {
        double x = 0., dx = 0., y = 0., dy = 0., z = 0., dz = 0.;
        StatusCode sc = m_muonDet->Tile2XYZ( coord->key(), x, dx, y, dy, z, dz );
        if ( sc.isFailure() ) {
            Warning( "Impossible MuonTileID" );
            continue;
        }
        hits.emplace_back( coord->key(), x, dx, y, dy, z, dz );
    }
    // Put the hits in the station
    m_stations[station].setHits( std::move(hits) );
    m_prepared.set(station,true);
}

//=============================================================================
void Hlt1MuonHitManager::loadCoords()
{
    LHCb::MuonCoords* coords = getIfExists<LHCb::MuonCoords>( m_coordLocation );
    if ( !coords ) {
        Exception( "Cannot retrieve MuonCoords ", StatusCode::FAILURE );
    }
    for ( auto coord : *coords ) {
        m_coords[coord->key().station()].emplace_back( coord ) ;
    }
    m_loaded = true;
}
