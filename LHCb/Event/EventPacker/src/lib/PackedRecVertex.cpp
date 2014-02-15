// $Id: PackedRecVertex.cpp,v 1.3 2010-05-19 09:04:09 jonrob Exp $

// local
#include "Event/PackedRecVertex.h"

// Checks
#include "Event/PackedEventChecks.h"

using namespace LHCb;

void RecVertexPacker::pack( const Data & vert,
                            PackedData & pvert,
                            const DataVector & /* verts */,
                            PackedDataVector & pverts ) const
{
  pvert.technique  = vert.technique();
  pvert.chi2       = m_pack.fltPacked( vert.chi2() );
  pvert.nDoF       = vert.nDoF();
  pvert.x          = m_pack.position( vert.position().x() );
  pvert.y          = m_pack.position( vert.position().y() );
  pvert.z          = m_pack.position( vert.position().z() );

  // convariance Matrix
  const double err0 = safe_sqrt( vert.covMatrix()(0,0) );
  const double err1 = safe_sqrt( vert.covMatrix()(1,1) );
  const double err2 = safe_sqrt( vert.covMatrix()(2,2) );
  pvert.cov00 = m_pack.position( err0 );
  pvert.cov11 = m_pack.position( err1 );
  pvert.cov22 = m_pack.position( err2 );
  pvert.cov10 = m_pack.fraction( vert.covMatrix()(1,0), err1*err0 );
  pvert.cov20 = m_pack.fraction( vert.covMatrix()(2,0), err2*err0 );
  pvert.cov21 = m_pack.fraction( vert.covMatrix()(2,1), err2*err1 );

  //== Store the Tracks and weights
  pvert.firstTrack = pverts.refs().size();
  std::vector<float>::const_iterator iW = vert.weights().begin();
  for ( SmartRefVector<LHCb::Track>::const_iterator itT = vert.tracks().begin();
        vert.tracks().end() != itT; ++itT, ++iW )
  {
    pverts.addRef( m_pack.reference( &pverts, (*itT)->parent(), (*itT)->key() ) );
    pverts.addWeight( m_pack.fraction(*iW) );
  }
  pvert.lastTrack = pverts.refs().size();

  //== Handles the ExtraInfo
  pvert.firstInfo = pverts.extras().size();
  for ( GaudiUtils::VectorMap<int,double>::iterator itE = vert.extraInfo().begin();
        vert.extraInfo().end() != itE; ++itE )
  {
    pverts.addExtra( (*itE).first, m_pack.fltPacked( (*itE).second ) );
  }
  pvert.lastInfo = pverts.extras().size();

}

void RecVertexPacker::pack( const DataVector & verts,
                            PackedDataVector & pverts ) const
{
  pverts.vertices().reserve(verts.size());

  for ( DataVector::const_iterator iD = verts.begin();
        iD != verts.end(); ++iD )
  {
    const Data & vert = **iD;

    // new packed data object
    pverts.vertices().push_back( PackedData() );
    PackedData & pvert = pverts.vertices().back();

    // Key
    pvert.key = vert.key();

    // fill physics info from vert to pvert
    pack( vert, pvert, verts, pverts );
  }

}

void RecVertexPacker::unpack( const PackedData       & pvert,
                              Data                   & vert,
                              const PackedDataVector & pverts,
                              DataVector             & verts ) const
{
  vert.setTechnique( (LHCb::RecVertex::RecVertexType) pvert.technique );
  vert.setChi2AndDoF( m_pack.fltPacked( pvert.chi2), pvert.nDoF );
  vert.setPosition( Gaudi::XYZPoint( m_pack.position( pvert.x ),
                                     m_pack.position( pvert.y ),
                                     m_pack.position( pvert.z ) ) );

  // convariance Matrix
  const double err0 = m_pack.position( pvert.cov00 );
  const double err1 = m_pack.position( pvert.cov11 );
  const double err2 = m_pack.position( pvert.cov22 );
  Gaudi::SymMatrix3x3 & cov = *(const_cast<Gaudi::SymMatrix3x3*>(&vert.covMatrix()));
  cov(0,0) = err0 * err0;
  cov(1,0) = err1 * err0 * m_pack.fraction( pvert.cov10 );
  cov(1,1) = err1 * err1;
  cov(2,0) = err2 * err0 * m_pack.fraction( pvert.cov20 );
  cov(2,1) = err2 * err1 * m_pack.fraction( pvert.cov21 );
  cov(2,2) = err2 * err2;

  //== Store the Tracks and weights
  int hintID(0), tKey(0);
  for ( unsigned short int kk = pvert.firstTrack; pvert.lastTrack > kk; ++kk )
  {
    // Get the track
    const int trk = *(pverts.refs().begin()+kk);
    m_pack.hintAndKey( trk, &pverts, &verts, hintID, tKey );
    SmartRef<LHCb::Track> ref( &verts, hintID, tKey );
    // If available, get the weight
    const float weight = (float)( (int)pverts.version()>1 ?
                                  m_pack.fraction(pverts.weights()[kk]) : 1.0 );
    // save with weight
    vert.addToTracks( ref, weight );
  }

  //== Handles the ExtraInfo
  for ( unsigned short int kEx = pvert.firstInfo; pvert.lastInfo > kEx; ++kEx )
  {
    const std::pair<int,int>& info = *(pverts.extras().begin()+kEx);
    vert.addInfo( info.first, m_pack.fltPacked( info.second ) );
  }
}

void RecVertexPacker::unpack( const PackedDataVector & pverts,
                              DataVector             & verts ) const
{
  verts.reserve( pverts.vertices().size() );

  for ( PackedDataVector::Vector::const_iterator iD = pverts.vertices().begin();
        iD != pverts.vertices().end(); ++iD )
  {
    const PackedData & pvert = *iD;

    // make and save new pid in container
    Data * vert = new Data();
    verts.insert( vert, pvert.key );

    // Fill data from packed object
    unpack( pvert, *vert, pverts, verts );
  }

}
