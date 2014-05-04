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

using std::vector;
using std::sort;
using std::pair;

namespace
{
template <typename iterator_>
struct iter_pair_range : pair<iterator_, iterator_>
{
    using super = pair<iterator_, iterator_>;
    using super::super; // delegate c'tor
    using iterator = iterator_;
    iterator begin() const
    {
        return this->first;
    }
    iterator end() const
    {
        return this->second;
    }

};

template <typename Iter, typename I2> // I2 must be convertable to Iter
iter_pair_range<typename std::decay<Iter>::type> make_range( Iter&& begin, I2&& end )
{
    return {std::forward<Iter>( begin ), std::forward<I2>( end )};
}

template <typename Iter, typename I2> // I2 must be convertable to Iter
iter_pair_range<typename std::decay<Iter>::type> make_range( pair<Iter, I2>&& p )
{
    return {std::forward<pair<Iter, I2>>( p )};
}
}

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

    m_nHits.fill(0);
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
StatusCode Hlt1MuonHitManager::finalize()
{
    // boost::singleton_pool<Hlt1MuonHit, sizeof(Hlt1MuonHit)>::release_memory();
    return GaudiTool::finalize();
}

//=============================================================================
void Hlt1MuonHitManager::handle( const Incident& incident )
{
    if ( IncidentType::BeginEvent != incident.type() ) return;

    for ( unsigned int station = 0; station < m_stations.size(); ++station ) {
        if ( m_prepared[station] ) m_stations[station].clearHits();
    }
    m_prepared.reset();
    m_nHits.fill( 0 );
    m_loaded = false;
    m_coords.clear();
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
Hlt1MuonRegion Hlt1MuonHitManager::region( unsigned int station,
                                           unsigned int region ) const
{
    return m_stations[station].region( region );
}

//=============================================================================
void Hlt1MuonHitManager::prepareHits( unsigned int station )
{
    if ( !m_loaded ) loadCoords();

    std::vector<Hlt1MuonHit*> hits; hits.reserve( m_nHits[station] );
    for ( const auto& entry : make_range( m_coords.equal_range( station ) ) ) {
        const LHCb::MuonCoord* coord = entry.second;
        double x = 0., dx = 0., y = 0., dy = 0., z = 0., dz = 0.;
        StatusCode sc = m_muonDet->Tile2XYZ( coord->key(), x, dx, y, dy, z, dz );
        if ( sc.isFailure() ) {
            Warning( "Impossible MuonTileID" );
            continue;
        }
        hits.emplace_back( new Hlt1MuonHit{coord->key(), x, dx, y, dy, z, dz} );
    }
    // Put the hits in the station
    m_stations[station].setHits( std::move(hits) ); // transfer ownership
    m_prepared.set(station,true);
}

//=============================================================================
void Hlt1MuonHitManager::loadCoords()
{
    LHCb::MuonCoords* coords = getIfExists<LHCb::MuonCoords>( m_coordLocation );
    if ( !coords ) {
        Exception( "Cannot retrieve MuonCoords ", StatusCode::FAILURE );
    }

    // TODO: replace m_coords with a partitioned vector?
    for ( auto coord : *coords ) {
        auto station = coord->key().station();
        m_coords.insert( {station, coord} );
        ++m_nHits[station];
    }

    m_loaded = true;
}
