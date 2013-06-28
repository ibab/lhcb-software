// $Id: $
// STL
#include <vector>
#include <algorithm>

// boost
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

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
#include "Hlt1MuonStation.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt1MuonHitManager
//
// 2010-12-07 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Hlt1MuonHitManager );

using boost::assign::list_of;
using boost::bind;
using std::make_pair;
using std::vector;
using std::sort;
using std::pair;
using std::less;

//=============================================================================
Hlt1MuonHitManager::Hlt1MuonHitManager(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
   : GaudiTool ( type, name , parent ), m_muonDet( 0 ),
     m_muonCoords( 0 ), m_loaded( false )
{
   declareInterface<Hlt1MuonHitManager>(this);

   // declare properties
   declareProperty( "MuonCoordLocation", m_coordLocation = LHCb::MuonCoordLocation::MuonCoords );
   declareProperty( "MakeClusters", m_makeClusters = false );

   // X region boundaries
   m_regions.resize( 5 );
   // Define the regions, the last ones are increased a little to make sure
   // everything is caught.
   std::vector< double > tmp0=list_of( -3840 )( -1920 )( -960 )( -480 )( 480 )( 960 )( 1920 )( 3940 );
   std::vector< double > tmp1=list_of( -4800 )( -1200 )( -600 )( 600 )( 1200 )( 4900 );
   std::vector< double > tmp2=list_of( -5180 )( -1292 )( -644 )( 644 )( 1292 )( 5280 );
   std::vector< double > tmp3=list_of( -5556 )( -1392 )( -696 )( 696 )( 1392 )( 5656 );
   std::vector< double > tmp4=list_of( -5952 )( -1488 )( -744 )( 744 )( 1488 )( 6052 );

   m_regions[ 0 ] = tmp0;
   m_regions[ 1 ] = tmp1;
   m_regions[ 2 ] = tmp2;
   m_regions[ 3 ] = tmp3;
   m_regions[ 4 ] = tmp4;

   m_nHits.resize( 5, 0 );

   m_prepared.resize( 5, false );
}

//=============================================================================
Hlt1MuonHitManager::~Hlt1MuonHitManager()
{
   BOOST_FOREACH( Hlt1MuonStation* station, m_stations ) {
      delete station;
   }
} 

//=============================================================================
StatusCode Hlt1MuonHitManager::initialize()
{
   StatusCode sc = GaudiTool::initialize();
   if ( sc.isFailure() ) return sc;

   incSvc()->addListener( this, IncidentType::BeginEvent );

   m_muonDet = getDet< DeMuonDetector >( "/dd/Structure/LHCb/DownstreamRegion/Muon" );

   unsigned int stations = m_muonDet->stations();

   for ( unsigned int i = 0; i < stations; ++i ) {
      Hlt1MuonStation* station = new Hlt1MuonStation( m_muonDet, i, m_regions[ i ] );
      m_stations.push_back( station );
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
      if ( m_prepared[ station ] ) m_stations[ station ]->clearHits();
   }
   m_muonCoords = 0;
   m_prepared.assign( 5, false );
   m_nHits.assign( 5, 0 );
   m_loaded = false;
   m_coords.clear();
}

//=============================================================================
Hlt1MuonHitRange Hlt1MuonHitManager::hits( const double xmin, const unsigned int station,
                                           const unsigned int region )
{
   if ( !m_prepared[ station ] ) {
      prepareHits( station );
   }
   return m_stations[ station ]->hits( xmin, region );
}

//=============================================================================
unsigned int Hlt1MuonHitManager::nRegions( const unsigned int station ) const
{
   return  m_stations[ station ]->nRegions();
}

//=============================================================================
const Hlt1MuonStation& Hlt1MuonHitManager::station( const unsigned int id ) const
{
   const Hlt1MuonStation* station = m_stations[ id ];
   return *station;
}

//=============================================================================
const Hlt1MuonRegion& Hlt1MuonHitManager::region( const unsigned int station,
                                                  const unsigned int region ) const
{
   return m_stations[ station ]->region( region );
}

//=============================================================================
void Hlt1MuonHitManager::prepareHits( const unsigned int station )
{
   if ( !m_loaded ) loadCoords();

   m_hits.clear();
   BOOST_FOREACH( const coords_t::value_type& entry, m_coords.equal_range( station ) ) {
      const LHCb::MuonCoord* coord = entry.second;
      double x = 0., dx = 0., y = 0., dy = 0., z = 0., dz = 0.;
      StatusCode sc = m_muonDet->Tile2XYZ( coord->key(), x, dx, y, dy, z, dz );
      if ( sc.isFailure() ) {
         Warning( "Impossible MuonTileID" );
         continue;
      }
      Hlt1MuonHit* hit = new Hlt1MuonHit( coord->key(), x, dx, y, dy, z, dz );
      m_hits.push_back( hit );
   }

   // Sort the hits
   std::sort( m_hits.begin(), m_hits.end(),
              bind( less< double >(), bind< double >( &Hlt1MuonHit::x, _1 ),
                    bind< double >( &Hlt1MuonHit::x, _2 ) ) );

   // Put the hits in the station
   m_stations[ station ]->setHits( m_hits );
   m_prepared[ station ] = true;
}

//=============================================================================
void Hlt1MuonHitManager::loadCoords()
{
   LHCb::MuonCoords* coords = get< LHCb::MuonCoords >( m_coordLocation );
   if ( coords == 0 ) {
      Exception( "Cannot retrieve MuonCoords ", StatusCode::FAILURE );
   } else {
      m_muonCoords = coords;
   }

   for ( LHCb::MuonCoords::const_iterator it = m_muonCoords->begin(), end = m_muonCoords->end();
         it != end; ++it ) {
      const LHCb::MuonCoord* coord = *it;
      const LHCb::MuonTileID& tile = coord->key();
      unsigned int station = tile.station();
      m_coords.insert( make_pair( station, coord ) );
      m_nHits[ station ] += 1;
   }

   std::vector< unsigned int >::const_iterator it = max_element
      ( m_nHits.begin(), m_nHits.end() );
   m_hits.reserve( *it );
   m_loaded = true;
}
