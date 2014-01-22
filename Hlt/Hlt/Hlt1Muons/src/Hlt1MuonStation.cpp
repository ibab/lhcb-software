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

// local
#include "Hlt1MuonStation.h"
#include "Hlt1MuonRegion.h"
#include "Hlt1MuonHit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt1MuonStation
//
// 2010-12-07 : Roel Aaij
//-----------------------------------------------------------------------------

using Gaudi::Range_;
using Gaudi::range;
using std::vector;
using std::exception;

//=============================================================================
Hlt1MuonStation::Hlt1MuonStation( DeMuonDetector* det, int station,
                                  const vector< double >& regions )
   : m_muonDet{ det }
   , m_station{ station }
   , m_z{ det->getStationZ(station) }
   , m_nRegionsX( regions.size() - 1 ) // narrowing from unsigned long -> unsigned int
   , m_nRegionsY{ 7u }
{
   m_hits.resize( nRegions() );
   m_regions.reserve( nRegions() );

   m_ymax = det->getOuterY( station );
   m_ymin = - m_ymax;
   m_dy = 2 * m_ymax / m_nRegionsY;

   for( unsigned int i = 1; i < regions.size(); ++i ) {
      for( unsigned int j = 0; j < m_nRegionsY; ++j ) {
         const unsigned int id = ( i - 1 ) * m_nRegionsY + j;
         double ymin = m_ymin + j * m_dy;
         m_regions.emplace_back( id, regions[ i - 1], regions[ i ],
                                              ymin, ymin + m_dy );
      }
   }
}

//=============================================================================
Hlt1MuonStation::~Hlt1MuonStation()
{
   clearHits();
   m_hits.clear();
} 

//=============================================================================
Hlt1MuonHitRange Hlt1MuonStation::hits( double xmin,
                                        unsigned int region ) const
{
   auto it = std::find_if( m_hits[region].begin(),m_hits[region].end(),
                           [=](const Hlt1MuonHit* hit) { 
                                double x = hit->x() + hit->dx() / 2.;
                                return  x > xmin;
                           } );
   return Hlt1MuonHitRange( it, m_hits[region].end() );
}

//=============================================================================
Hlt1MuonHitRange Hlt1MuonStation::hits( unsigned int region ) const
{
   return range( m_hits[ region ] );
}

//=============================================================================
void Hlt1MuonStation::clearHits()
{
   for( Hlt1MuonHits& hits: m_hits ) {
      for( Hlt1MuonHit* hit: hits ) delete hit;
      hits.clear();
   }
}

//=============================================================================
void Hlt1MuonStation::setHits( const Hlt1MuonHits& hits )
{
   for( Hlt1MuonHit* hit: hits ) addHit( hit );
}

//=============================================================================
void Hlt1MuonStation::addHit( Hlt1MuonHit* hit )
{
   int index = int( ( hit->y() - m_ymin ) / m_dy );
   try {
      index +=  m_nRegionsY * xRegion( hit->x() );
   } catch ( const exception& ) {
      return;
   }
   m_hits[index].push_back( hit );
}

//=============================================================================
inline unsigned int Hlt1MuonStation::xRegion( const double x )
{
   unsigned int i = 0;
   for ( ; i < m_nRegionsX; ++i ) {
      // Use the first x region in the regions to test which one we need.
      const Hlt1MuonRegion& region = m_regions[ m_nRegionsY * i ];
      if ( x < region.xmax() ) break;
   }
   if ( i == m_nRegionsX ) throw std::exception();
   return i;
}
