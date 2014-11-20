// $Id: $
// Include files
#include <exception>

// STL
#include <vector>

// Gaudi
#include <GaudiKernel/Range.h>

// Muon detector
#include <MuonDet/DeMuonDetector.h>
#include <Kernel/MuonTileID.h>
#include <Event/MuonCoord.h>

// Hlt1Muons
#include <Hlt1Muons/Hlt1MuonHit.h>

// local
#include "Hlt1MuonStation.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt1MuonStation
//
// 2010-12-07 : Roel Aaij
//-----------------------------------------------------------------------------

using Gaudi::range;
using std::vector;

//=============================================================================
Hlt1MuonStation::Hlt1MuonStation( DeMuonDetector* det, int station,
                                  vector<double> regions )
    : m_xboundaries{ std::move(regions) }
    , m_hits{}
    , m_index{ nRegionsY * ( m_xboundaries.size()-1 ) + 1 }
    , m_z{     det->getStationZ( station )}
    , m_ymax{  det->getOuterY( station ) }
    , m_station{station}

{
}

//=============================================================================
Hlt1MuonHitRange Hlt1MuonStation::hits( double xmin, unsigned int region ) const
{
    auto first = std::find_if( m_index[region], m_index[region+1],
                          [=]( const Hlt1MuonHit& hit ) {
                            return ( hit.x() + hit.dx() / 2. ) > xmin;
    } );
    return { first, m_index[region+1] } ;
}

//=============================================================================
Hlt1MuonHitRange Hlt1MuonStation::hits( double xmin, double xmax, unsigned int region ) const
{
    auto first = std::find_if( m_index[region], m_index[region+1],
                          [=]( const Hlt1MuonHit& hit ) {
                            return ( hit.x() + hit.dx() / 2. ) > xmin;
    } );
    auto last = std::find_if( first, m_index[region+1], [=]( const Hlt1MuonHit& hit) {
                            //TODO: this weird asymmetry between first, last is for historical reasons...
                            return hit.x() > xmax;
                            // return ( hit.x() - hit.dx() / 2.) > xmax;
    } );
    return { first, last };
}
//=============================================================================
Hlt1MuonHitRange Hlt1MuonStation::hits( unsigned int region ) const
{
    return { m_index[region], m_index[region+1] };
}

//=============================================================================
void Hlt1MuonStation::setHits( Hlt1MuonHits&& hts )
{
    m_hits = std::move(hts);

    //TODO: first get rid of invalid hits ( x < xlow, y<ymin, y>ymax )
    // m_hits.erase( std::remove_if( std::begin(m_hits), std::end(m_hits), ... ) );
    auto by_x = [](const Hlt1MuonHit& lhs, const Hlt1MuonHit& rhs) { return lhs.x() < rhs.x(); };
    auto y_lt_ = [](double ymax) { return [=]( const Hlt1MuonHit &h ) { return h.y() < ymax; }; };
    auto x_lt_ = [](double xmax) { return [=]( const Hlt1MuonHit &h ) { return h.x() < xmax; }; };

    auto id = std::begin(m_index);
    *id = std::begin(m_hits);
    // partition in x
    for ( auto x = std::begin(m_xboundaries)+1 ; x!=std::end(m_xboundaries); ++x ) { 
        auto imax = std::partition( *id, std::end(m_hits), x_lt_( *x ) ); 
        // within each x partition, partition in y
        for(unsigned j=0;j<nRegionsY;++j ) {
            *(id+1) = std::partition( *id, imax, y_lt_( ymin()+(j+1)*dy() ) );
            // within each region, sort by x 
            std::sort( *id, *(id+1), by_x );
            ++id; // next region...
        }
        assert( *id == imax ); // no invalid hits in y...
    }
    assert( *id == std::end(m_hits) );
    assert( uint(std::distance( std::begin(m_index), id )) == uint(m_index.size() - 1) );
}
