// $Id: PackedVertex.cpp,v 1.3 2010-05-19 09:04:09 jonrob Exp $

// local
#include "Event/PackedVertex.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void VertexPacker::pack( const Data & vert,
                         PackedData & pvert,
                         PackedDataVector & pverts ) const
{
  if ( 0 == pverts.packingVersion() )
  {
    // technique
    pvert.technique = static_cast<int>(vert.technique());

    // outgoing particles
    pvert.firstOutgoingPart = pverts.outgoingParticles().size();
    for ( SmartRefVector<LHCb::Particle>::const_iterator iP = vert.outgoingParticles().begin();
          iP != vert.outgoingParticles().end(); ++iP )
    {
      const LHCb::Particle * P = *iP;
      if ( P )
      {
        pverts.outgoingParticles().push_back( m_pack.reference64( &pverts,
                                                                  P->parent(),
                                                                  P->key() ) );
      }
    }
    pvert.lastOutgoingPart = pverts.outgoingParticles().size();

  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pverts.packingVersion();
    throw GaudiException( mess.str(), "VertexPacker", StatusCode::FAILURE );
  }
}

void VertexPacker::pack( const DataVector & verts,
                         PackedDataVector & pverts ) const
{
  pverts.data().reserve( verts.size() );

  for ( DataVector::const_iterator iD = verts.begin();
        iD != verts.end(); ++iD )
  {
    const Data & vert = **iD;

    // new packed data object
    pverts.data().push_back( PackedData() );
    PackedData & pvert = pverts.data().back();

    // Key
    pvert.key = vert.key();

    // fill physics info from vert to pvert
    pack( vert, pvert, pverts );
  }

}

void VertexPacker::unpack( const PackedData       & pvert,
                           Data                   & vert,
                           const PackedDataVector & pverts,
                           DataVector             & verts ) const
{
  if ( 0 == pverts.packingVersion() )
  {
    // technique
    vert.setTechnique( static_cast<Vertex::CreationMethod>(pvert.technique) );

    // outgoing particles
    for ( unsigned int iiP = pvert.firstOutgoingPart; iiP < pvert.lastOutgoingPart; ++iiP )
    {
      const int & iP = pverts.outgoingParticles()[iiP];
      int hintID(0), key(0);
      m_pack.hintAndKey64( iP, &pverts, &verts, hintID, key );
      SmartRef<LHCb::Particle> ref(&verts,hintID,key);
      vert.addToOutgoingParticles( ref );
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pverts.packingVersion();
    throw GaudiException( mess.str(), "VertexPacker", StatusCode::FAILURE );
  }
}

void VertexPacker::unpack( const PackedDataVector & pverts,
                           DataVector           & verts ) const
{
  verts.reserve( pverts.data().size() );

  for ( PackedDataVector::Vector::const_iterator iD = pverts.data().begin();
        iD != pverts.data().end(); ++iD )
  {
    const PackedData & pvert = *iD;

    // make and save new pid in container
    Data * vert = new Data();
    verts.insert( vert, pvert.key );

    // Fill data from packed object
    unpack( pvert, *vert, pverts, verts );
  }

}

StatusCode VertexPacker::check( const DataVector & dataA,
                                const DataVector & dataB,
                                GaudiAlgorithm & parent ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // checker
  const DataPacking::DataChecks ch(parent);

  // Loop over data containers together and compare
  DataVector::const_iterator iA(dataA.begin()), iB(dataB.begin());
  for ( ; iA != dataA.end() && iB != dataB.end(); ++iA, ++iB )
  {
    // assume OK from the start
    bool ok = true;

    // checks here

    // technique
    ok &= (*iA)->technique() == (*iB)->technique();

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent.warning() << "Problem with Vertex data packing :-" << endmsg
                       << "  Original Vertex : " << **iA
                       << endmsg
                       << "  Unpacked Vertex : " << **iB
                       << endmsg;
      sc = StatusCode::FAILURE;
    }
  }

  // Return final status
  return sc;
}
