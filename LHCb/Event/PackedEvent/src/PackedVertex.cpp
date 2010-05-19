// $Id: PackedVertex.cpp,v 1.3 2010-05-19 09:04:09 jonrob Exp $

// local
#include "Event/PackedVertex.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void VertexPacker::pack( const DataVector & verts,
                         PackedDataVector & pverts ) const
{
  pverts.data().reserve( verts.size() );
  if ( 0 == pverts.packingVersion() )
  {
    for ( DataVector::const_iterator iD = verts.begin();
          iD != verts.end(); ++iD )
    {
      Data & vert = **iD;
      pverts.data().push_back( PackedData() );
      PackedData & pvert = pverts.data().back();
      // fill pvert from vert
      pvert.technique = static_cast<int>(vert.technique());
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
  if ( 0 == pverts.packingVersion() )
  {
    for ( PackedDataVector::Vector::const_iterator iD = pverts.data().begin();
          iD != pverts.data().end(); ++iD )
    {
      const PackedData & pvert = *iD;
      // make and save new pid in container
      Data * vert = new Data();
      verts.add( vert );
      // Fill data from packed object
      vert->setTechnique( static_cast<Vertex::CreationMethod>(pvert.technique) );
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pverts.packingVersion();
    throw GaudiException( mess.str(), "VertexPacker", StatusCode::FAILURE );
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
