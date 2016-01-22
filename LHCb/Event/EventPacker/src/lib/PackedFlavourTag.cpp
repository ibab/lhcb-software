
// local
#include "Event/PackedFlavourTag.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void FlavourTagPacker::pack( const Data & ft,
                             PackedData & pft,
                             PackedDataVector & pfts ) const
{

  // Decision
  pft.decision = ft.decision();
  pft.omega    = m_pack.fraction( ft.omega() );
 
  // OS decision
  pft.decisionOS = ft.decisionOS();
  pft.omegaOS    = m_pack.fraction( ft.omegaOS() );

  // tagging particle
  if ( ft.taggedB() )
  {
    pft.taggedB = m_pack.reference64( &pfts,
                                      ft.taggedB()->parent(),
                                      ft.taggedB()->key() );
  }

  // Taggers
  pft.firstTagger = pfts.taggers().size();
  pfts.taggers().reserve( pfts.taggers().size() + ft.taggers().size() );
  for ( const auto & T : ft.taggers() )
  {
    // make a new packed tagger object
    pfts.taggers().emplace_back( PackedTagger() );
    auto & ptagger = pfts.taggers().back();

    // save data members
    ptagger.type     = T.type();
    ptagger.decision = T.decision();
    ptagger.omega    = m_pack.fraction( T.omega() );

    // tagging particles
    ptagger.firstTagP = pfts.taggeringPs().size();
    pfts.taggeringPs().reserve( pfts.taggeringPs().size() + T.taggerParts().size() );
    for ( const auto& TP : T.taggerParts() )
    {
      if ( TP.target() )
      {
        pfts.taggeringPs().push_back( m_pack.reference64( &pfts,
                                                          TP->parent(),
                                                          TP->key() ) );
      }
    }
    ptagger.lastTagP = pfts.taggeringPs().size();
    
  }
  pft.lastTagger = pfts.taggers().size();

}

void FlavourTagPacker::pack( const DataVector & fts,
                             PackedDataVector & pfts ) const
{
  pfts.data().reserve( fts.size() );

  for ( const auto * ft : fts )
  {
    // Make a new packed data object and save
    pfts.data().emplace_back( PackedData() );
    auto & pft = pfts.data().back();

    // fill ppart key from part
    pft.key = ft->key();

    // Pack all the physics information
    pack( *ft, pft, pfts );
  }

}

void FlavourTagPacker::unpack( const PackedData       & pft,
                               Data                   & ft,
                               const PackedDataVector & pfts,
                               DataVector             & fts ) const
{
  
  // Decision
  ft.setDecision( pft.decision );
  ft.setOmega   ( m_pack.fraction(pft.omega) );

  // OS Decision
  ft.setDecisionOS( pft.decisionOS );
  ft.setOmegaOS   ( m_pack.fraction(pft.omegaOS) );

  // Tagging B
  if ( -1 != pft.taggedB )
  {
    int hintID(0), key(0);
    if ( m_pack.hintAndKey64( pft.taggedB, &pfts, &fts, hintID, key ) )
    {
      SmartRef<LHCb::Particle> ref(&fts,hintID,key);
      ft.setTaggedB( ref );
    }
    else { parent().Error("Corrupt FlavourTag Particle SmartRef found").ignore(); }
  }

  // Taggers
  std::vector<LHCb::Tagger>& taggers = 
    *(const_cast<std::vector<LHCb::Tagger>*>(&ft.taggers()));
  taggers.reserve( pft.lastTagger - pft.firstTagger );
  for ( auto iT = pft.firstTagger; iT < pft.lastTagger; ++iT )
  {
    // Reference to packed tagger
    const auto & ptagger = pfts.taggers()[iT];

    // Make a new tagger
    taggers.emplace_back( LHCb::Tagger() );
    auto & tagger = taggers.back();

    // set the tagger members
    tagger.setType    ( ptagger.type                   );
    tagger.setDecision( ptagger.decision               );
    tagger.setOmega   ( m_pack.fraction(ptagger.omega) );

    // tagging particles
    for ( auto iP = ptagger.firstTagP; iP < ptagger.lastTagP; ++iP )
    {
      int hintID(0), key(0);
      if ( m_pack.hintAndKey64( pfts.taggeringPs()[iP],
                                &pfts, &fts, hintID, key ) )
      {
        SmartRef<LHCb::Particle> ref(&fts,hintID,key);
        tagger.addToTaggerParts( ref );
      }
      else { parent().Error("Corrupt FlavourTag Tagging Particle SmartRef found").ignore(); }
    }
  }

}

void FlavourTagPacker::unpack( const PackedDataVector & pfts,
                               DataVector             & fts ) const
{
  fts.reserve( pfts.data().size() );

  for ( const auto & pft : pfts.data() )
  {
    // make and save new pid in container
    auto * ft = new Data();
    fts.insert( ft, pft.key );

    // Fill data from packed object
    unpack( pft, *ft, pfts, fts );
  }
}

StatusCode FlavourTagPacker::check( const DataVector & dataA,
                                    const DataVector & dataB ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // Loop over data containers together and compare
  auto iA(dataA.begin()), iB(dataB.begin());
  for ( ; iA != dataA.end() && iB != dataB.end(); ++iA, ++iB )
  {
    sc = sc && check( **iA, **iB );
  }

  // Return final status
  return sc;
}

StatusCode FlavourTagPacker::check( const Data & dataA,
                                    const Data & dataB ) const
{ 
  // assume OK from the start
  bool ok = true;

  // checker
  const DataPacking::DataChecks ch(parent());

  // Checks

  // key
  ok &= ch.compareInts( "Key", dataA.key(), dataB.key() );

  // decision
  ok &= ch.compareInts( "Decision", dataA.decision(), dataB.decision() );
  // omega
  ok &= ch.compareFloats( "Omega", dataA.omega(), dataB.omega(), 1e-4 );

  // decisionOS
  ok &= ch.compareInts( "DecisionOS", dataA.decisionOS(), dataB.decisionOS() );
  // omegaOS
  ok &= ch.compareFloats( "OmegaOS", dataA.omegaOS(), dataB.omegaOS(), 1e-4 );

  // tagging B
  ok &= ch.comparePointers( "TaggedB", dataA.taggedB(), dataB.taggedB() );

  // Taggers
  const bool sizeOK = ch.compareInts( "#Taggers", 
                                      dataA.taggers().size(), 
                                      dataB.taggers().size() );
  ok &= sizeOK;
  if ( sizeOK )
  {
    auto iA(dataA.taggers().begin()), iB(dataB.taggers().begin());
    for ( ; iA != dataA.taggers().end() && iB != dataB.taggers().end(); ++iA, ++iB )
    {
      ok &= ch.compareInts( "TaggerType",     iA->type(),     iB->type()     );
      ok &= ch.compareInts( "TaggerDecision", iA->decision(), iB->decision() );
      ok &= ch.compareFloats( "TaggerOmega",  iA->omega(),    iB->omega()    );
      const bool pSizeOK = ch.compareInts( "TaggerPSize", 
                                           iA->taggerParts().size(),
                                           iB->taggerParts().size() );
      ok &= pSizeOK;
      if ( pSizeOK )
      {
        auto iPA(iA->taggerParts().begin()), iPB(iB->taggerParts().begin());
        for ( ; iPA != iA->taggerParts().end() && iPB != iB->taggerParts().end(); ++iPA, ++iPB )
        {
          ok &= ch.comparePointers( "TaggerParts", &**iPA, &**iPB );
        }
      }
    }
  }

  // force printout for tests
  //ok = false;
  // If comparison not OK, print full information
  if ( !ok )
  {
    const std::string loc = ( dataA.parent() && dataA.parent()->registry() ?
                              dataA.parent()->registry()->identifier() : "Not in TES" );
    parent().warning() << "Problem with FlavourTag data packing :-" << endmsg
                       << "  Original FlavourTag key=" << dataA.key() 
                       << " in '" << loc << "'" << endmsg
                       << dataA << endmsg
                       << "  Unpacked FlavourTag" << endmsg
                       << dataB << endmsg;
  }

  return ( ok ? StatusCode::SUCCESS : StatusCode::FAILURE );
}
