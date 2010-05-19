// $Id: PackedParticle.cpp,v 1.3 2010-05-19 09:04:08 jonrob Exp $

// local
#include "Event/PackedParticle.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void ParticlePacker::pack( const DataVector & parts,
                           PackedDataVector & pparts ) const
{
  pparts.data().reserve( parts.size() );
  if ( 0 == pparts.packingVersion()  )
  {
    for ( DataVector::const_iterator iD = parts.begin();
          iD != parts.end(); ++iD )
    {
      Data & part = **iD;
      pparts.data().push_back( PackedData() );
      PackedData & ppart = pparts.data().back();
      // fill ppart from part
      ppart.particleID = part.particleID().pid();
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pparts.packingVersion();
    throw GaudiException( mess.str(), "ParticlePacker", StatusCode::FAILURE );
  }
}

void ParticlePacker::unpack( const PackedDataVector & pparts,
                             DataVector       & parts ) const
{
  parts.reserve( pparts.data().size() );
  if ( 0 == pparts.packingVersion() )
  {
    for ( PackedDataVector::Vector::const_iterator iD = pparts.data().begin();
          iD != pparts.data().end(); ++iD )
    {
      const PackedData & ppart = *iD;
      // make and save new pid in container
      Data * part = new Data();
      parts.add( part );
      // Fill data from packed object
      part->setParticleID( LHCb::ParticleID(ppart.particleID) );
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pparts.packingVersion();
    throw GaudiException( mess.str(), "ParticlePacker", StatusCode::FAILURE );
  }
}

StatusCode ParticlePacker::check( const DataVector & dataA,
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
    ok &= (*iA)->particleID() == (*iB)->particleID();

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent.warning() << "Problem with Particle data packing :-" << endmsg
                       << "  Original Particle : " << **iA
                       << endmsg
                       << "  Unpacked Particle : " << **iB
                       << endmsg;
      sc = StatusCode::FAILURE;
    }
  }

  // Return final status
  return sc;
}
