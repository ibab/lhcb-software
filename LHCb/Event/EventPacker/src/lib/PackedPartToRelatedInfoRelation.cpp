
#include "Event/PackedPartToRelatedInfoRelation.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void RelatedInfoRelationsPacker::pack( const DataVector & rels,
                                       PackedDataVector & prels ) const
{
  // loop over the relations
  for ( const auto& rel : rels.relations() )
  {
    if ( !rel.from() ) continue;

    // make a new entry for this relation
    prels.relations().emplace_back( LHCb::PackedRelatedInfoMap() );
    auto & prel = prels.relations().back();

    // reference to the particle
    prel.reference = m_pack.reference64( &prels,
                                         rel.from()->parent(),
                                         rel.from()->key() );

    // First entry in the info vector
    prel.first = prels.info().size();

    // Loop over the related info map and fill into info vector
    const auto & rMap = rel.to();
    prels.info().reserve( prels.info().size() + rMap.size() );
    for ( const auto& data : rMap )
    {
      prels.info().emplace_back( std::make_pair(data.first,data.second) );
    }

    // Last entry in the info vector
    prel.last = prels.info().size();

  }

}

void RelatedInfoRelationsPacker::unpack( const PackedDataVector & prels,
                                         DataVector             & rels,
                                         const std::string & location ) const
{
  // If location is empty, unpacked everything
  if ( location.empty() )
  {
    for ( const auto& rel : prels.relations() ) { unpack( rel, prels, rels ); }
  }
  else
  {
    // loop over containers
    for ( const auto& cont : prels.containers() )
    {
      // Reconstruct container name for this entry
      const int indx = cont.reference >> 32;
      const auto & containerName = prels.linkMgr()->link(indx)->path();
      // if name matches, unpack
      if ( containerName == location )
      {
        // Loop over the relations saved at this container location and unpack
        for ( auto kk = cont.first; cont.last > kk; ++kk )
        { unpack( prels.relations()[kk], prels, rels ); }
      }
    }
  }
}

void
RelatedInfoRelationsPacker::unpack( const LHCb::PackedRelatedInfoMap & pmap,
                                    const PackedDataVector           & prels,
                                    DataVector                       & rels ) const
{
  // reconstruct the particle SmartRef and its container
  int srcLink(0), srcKey(0);
  m_pack.indexAndKey64( pmap.reference, srcLink, srcKey );

  // If new container, update cached info
  if ( srcLink != m_prevSrcLink )
  {
    m_prevSrcLink = srcLink;
    const auto & srcName = prels.linkMgr()->link(srcLink)->path();
    m_srcContainer = parent().get<LHCb::Particles>( srcName );
  }

  // Get the source object
  auto * from = m_srcContainer->object( srcKey );

  // Recreate the RelatedInfoMap
  TO to;
  to.reserve( pmap.last - pmap.first );
  for ( auto jj = pmap.first; pmap.last > jj; ++jj )
  {
    to.insert( prels.info()[jj] );
  }

  // Save the relation
  rels.relate( from, to );
}

StatusCode 
RelatedInfoRelationsPacker::check( const DataVector & dataA,
                                   const DataVector & dataB ) const
{
  // assume OK from the start
  bool ok = true;

  // checker
  const DataPacking::DataChecks ch(parent());

  // Number of relations
  ok &= ch.compareInts( "#Relations", dataA.size(), dataB.size() );

  // Loop over the relations (only if same size...)
  if ( ok ) 
  {
    auto iA(dataA.relations().begin()), iB(dataB.relations().begin());
    for ( ; iA != dataA.relations().end() && iB != dataB.relations().end(); ++iA, ++iB )
    {
      // Check the relations
      ok &= ch.comparePointers( "Particle", (*iA).from(), (*iB).from() );
      ok &= (*iA).to() == (*iB).to(); 
    }
  }

  // force printout for tests
  //ok = false;
  // If comparison not OK, print full information
  if ( !ok )
  {
    const std::string loc = ( dataA.registry() ?
                              dataA.registry()->identifier() : "Not in TES" );
    parent().warning() << "Problem with RelatedInfo data packing :-" << endmsg
                       << " Location '" << loc << "'" << endmsg
                       << " Size : Original=" << dataA.size() << " Unpacked=" << dataB.size() 
                       << endmsg;
    // if same size, print contents of relations
    if ( dataA.size() == dataB.size() )
    {
      auto iA(dataA.relations().begin()), iB(dataB.relations().begin());
      for ( ; iA != dataA.relations().end() && iB != dataB.relations().end(); ++iA, ++iB )
      {
        parent().warning() << " Original : From=" << (*iA).from() << " To=" << (*iA).to() << endmsg;
        parent().warning() << " Unpacked : From=" << (*iB).from() << " To=" << (*iB).to() << endmsg;
      }
    }
  }

  // finally return
  return ( ok ? StatusCode::SUCCESS : StatusCode::FAILURE );
}
