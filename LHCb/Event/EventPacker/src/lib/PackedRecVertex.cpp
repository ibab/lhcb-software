
// local
#include "Event/PackedRecVertex.h"
#include "Event/PackerUtils.h"

// Checks
#include "Event/PackedEventChecks.h"

using namespace LHCb;

void RecVertexPacker::pack( const Data & vert,
                            PackedData & pvert,
                            const DataVector & /* verts */,
                            PackedDataVector & pverts ) const
{
  const auto ver = pverts.packingVersion();
  if ( 1 == ver || 0 == ver )
  {

    pvert.technique  = vert.technique();
    pvert.chi2       = m_pack.fltPacked( vert.chi2() );
    pvert.nDoF       = vert.nDoF();
    pvert.x          = m_pack.position( vert.position().x() );
    pvert.y          = m_pack.position( vert.position().y() );
    pvert.z          = m_pack.position( vert.position().z() );
    
    // convariance Matrix
    const auto err0 = safe_sqrt( vert.covMatrix()(0,0) );
    const auto err1 = safe_sqrt( vert.covMatrix()(1,1) );
    const auto err2 = safe_sqrt( vert.covMatrix()(2,2) );
    pvert.cov00 = m_pack.position( err0 );
    pvert.cov11 = m_pack.position( err1 );
    pvert.cov22 = m_pack.position( err2 );
    pvert.cov10 = m_pack.fraction( vert.covMatrix()(1,0), err1*err0 );
    pvert.cov20 = m_pack.fraction( vert.covMatrix()(2,0), err2*err0 );
    pvert.cov21 = m_pack.fraction( vert.covMatrix()(2,1), err2*err1 );
    
    //== Store the Tracks and weights
    pvert.firstTrack = pverts.refs().size();
    {
      auto iW = vert.weights().begin();
      for ( auto itT = vert.tracks().begin();
            vert.tracks().end() != itT; ++itT, ++iW )
      {
        if ( *itT )
        {
          pverts.refs().push_back( UNLIKELY( 0==ver ) ?
                                   m_pack.reference32( &pverts, (*itT)->parent(), (*itT)->key() ) :
                                   m_pack.reference64( &pverts, (*itT)->parent(), (*itT)->key() ) );
          pverts.weights().push_back( m_pack.fraction(*iW) );
        }
        else
        {
          parent().Warning( "Null Track SmartRef in '" + 
                            LHCb::Packer::Utils::location(vert.parent()) + "'" ).ignore();
        }
      }
    }
    pvert.lastTrack = pverts.refs().size();
    
    //== Handles the ExtraInfo
    pvert.firstInfo = pverts.extras().size();
    for ( const auto& E : vert.extraInfo() )
    {
      pverts.addExtra( E.first, m_pack.fltPacked(E.second) );
    }
    pvert.lastInfo = pverts.extras().size();

  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "RecVertexPacker", StatusCode::FAILURE );
  }

}

void RecVertexPacker::pack( const DataVector & verts,
                            PackedDataVector & pverts ) const
{
  pverts.vertices().reserve(verts.size());

  for ( const auto * vert : verts )
  {
    // new packed data object
    pverts.vertices().emplace_back( PackedData() );
    auto & pvert = pverts.vertices().back();

    // Key
    pvert.key = vert->key();

    // fill physics info from vert to pvert
    pack( *vert, pvert, verts, pverts );
  }

}

void RecVertexPacker::unpack( const PackedData       & pvert,
                              Data                   & vert,
                              const PackedDataVector & pverts,
                              DataVector             & verts ) const
{
  const char ver = pverts.packingVersion();
  if ( 1 == ver || 0 == ver )
  {

    vert.setTechnique( (LHCb::RecVertex::RecVertexType) pvert.technique );
    vert.setChi2AndDoF( m_pack.fltPacked( pvert.chi2), pvert.nDoF );
    vert.setPosition( Gaudi::XYZPoint( m_pack.position( pvert.x ),
                                       m_pack.position( pvert.y ),
                                       m_pack.position( pvert.z ) ) );
    
    // convariance Matrix
    const auto err0 = m_pack.position( pvert.cov00 );
    const auto err1 = m_pack.position( pvert.cov11 );
    const auto err2 = m_pack.position( pvert.cov22 );
    auto & cov = *(const_cast<Gaudi::SymMatrix3x3*>(&vert.covMatrix()));
    cov(0,0) = err0 * err0;
    cov(1,0) = err1 * err0 * m_pack.fraction( pvert.cov10 );
    cov(1,1) = err1 * err1;
    cov(2,0) = err2 * err0 * m_pack.fraction( pvert.cov20 );
    cov(2,1) = err2 * err1 * m_pack.fraction( pvert.cov21 );
    cov(2,2) = err2 * err2;
    
    //== Store the Tracks and weights
    int hintID(0), tKey(0);
    for ( auto kk = pvert.firstTrack; pvert.lastTrack > kk; ++kk )
    {
      // Get the track
      const auto trk = *(pverts.refs().begin()+kk);
      if ( ( 0!=ver && m_pack.hintAndKey64( trk, &pverts, &verts, hintID, tKey ) ) ||
           ( 0==ver && m_pack.hintAndKey32( trk, &pverts, &verts, hintID, tKey ) ) )
      {
        SmartRef<LHCb::Track> ref( &verts, hintID, tKey );
        // If available, get the weight
        const auto weight = (float)( (int)pverts.version()>1 ?
                                     m_pack.fraction(pverts.weights()[kk]) : 1.0 );
        // save with weight
        vert.addToTracks( ref, weight );
      }
      else { parent().Error( "Corrupt RecVertex Track SmartRef detected." ).ignore(); }
    }
    
    //== Handles the ExtraInfo
    for ( auto kEx = pvert.firstInfo; pvert.lastInfo > kEx; ++kEx )
    {
      const auto& info = *(pverts.extras().begin()+kEx);
      vert.addInfo( info.first, m_pack.fltPacked( info.second ) );
    }

  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "RecVertexPacker", StatusCode::FAILURE );
  }

}

void RecVertexPacker::unpack( const PackedDataVector & pverts,
                              DataVector             & verts ) const
{
  verts.reserve( pverts.vertices().size() );

  for ( const auto & pvert : pverts.vertices() )
  {
    // make and save new pid in container
    auto * vert = new Data();
    verts.insert( vert, pvert.key );

    // Fill data from packed object
    unpack( pvert, *vert, pverts, verts );
  }

}
