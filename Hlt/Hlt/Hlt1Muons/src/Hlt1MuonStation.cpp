// $Id: $
// Include files 
#include <exception>

// STL
#include <vector>

// boost
#include <boost/foreach.hpp>

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
Hlt1MuonStation::Hlt1MuonStation( DeMuonDetector* det, const int station,
                                  const vector< double >& regions )
   : m_muonDet( det ), m_station( station )
{
   m_z = det->getStationZ( station );

   m_nRegionsX = regions.size() - 1;
   m_nRegionsY = 7;

   m_hits.resize( nRegions() );
   m_regions.reserve( nRegions() );

   m_ymax = det->getOuterY( station );
   m_ymin = - m_ymax;
   m_dy = 2 * m_ymax / m_nRegionsY;

   for( unsigned int i = 1; i < regions.size(); ++i ) {
      for( unsigned int j = 0; j < m_nRegionsY; ++j ) {
         const unsigned int id = ( i - 1 ) * m_nRegionsY + j;
         double ymin = m_ymin + j * m_dy;
         m_regions.push_back( Hlt1MuonRegion( id, regions[ i - 1], regions[ i ],
                                              ymin, ymin + m_dy ) );
      }
   }
}

//=============================================================================
Hlt1MuonStation::~Hlt1MuonStation()
{

} 

//=============================================================================
Hlt1MuonHitRange Hlt1MuonStation::hits( const double xmin,
                                        const unsigned int region ) const
{
   Hlt1MuonHits::const_iterator it = m_hits[ region ].begin(),
      end = m_hits[ region ].end();
   for ( ; it != end; ++it ) {
      const Hlt1MuonHit* hit = *it;
      double x = hit->x() + hit->dx() / 2.;
      if ( x > xmin ) break;
   }
   return Hlt1MuonHitRange( it, end );
}

//=============================================================================
Hlt1MuonHitRange Hlt1MuonStation::hits( const unsigned int region ) const
{
   return range( m_hits[ region ] );
}

//=============================================================================
void Hlt1MuonStation::clearHits()
{
   BOOST_FOREACH( Hlt1MuonHits& hits, m_hits ) hits.clear();
}

//=============================================================================
void Hlt1MuonStation::setHits( const Hlt1MuonHits& hits )
{
   BOOST_FOREACH( Hlt1MuonHit* hit, hits ) {
      addHit( hit );
   }
}

//=============================================================================
void Hlt1MuonStation::addHit( Hlt1MuonHit* hit )
{
   double x = hit->x();
   unsigned int xr = 0;
   try {
      xr = xRegion( x );
   } catch ( const exception& ) {
      return;
   }
   double y = hit->y();
   int yr = int( ( y - m_ymin ) / m_dy );
   int index = m_nRegionsY * xr + yr;
   m_hits[ index ].push_back( hit );
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
   if ( i == m_nRegionsX ) {
      throw std::exception();
   }
   return i;
}
