
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
  pft.category = ft.category();
  pft.omega    = m_pack.fraction( ft.omega() );
 
  // OS decision
  pft.decisionOS = ft.decisionOS();
  pft.categoryOS = ft.categoryOS();
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
  for ( std::vector<LHCb::Tagger>::const_iterator iT = ft.taggers().begin();
        iT != ft.taggers().end(); ++iT )
  {
    // make a new packed tagger object
    pfts.taggers().push_back( PackedTagger() );
    PackedTagger & ptagger = pfts.taggers().back();

    // save data members
    ptagger.type     = (*iT).type();
    ptagger.decision = (*iT).decision();
    ptagger.omega    = m_pack.fraction( (*iT).omega() );

    // tagging particles
    ptagger.firstTagP = pfts.taggeringPs().size();
    pfts.taggeringPs().reserve( pfts.taggeringPs().size() + (*iT).taggerParts().size() );
    for ( SmartRefVector<LHCb::Particle>::const_iterator iTP = (*iT).taggerParts().begin();
          iTP !=(*iT).taggerParts().end(); ++iTP )
    {
      if ( *iTP )
      {
        pfts.taggeringPs().push_back( m_pack.reference64( &pfts,
                                                          (*iTP)->parent(),
                                                          (*iTP)->key() ) );
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

  for ( DataVector::const_iterator iD = fts.begin();
        iD != fts.end(); ++iD )
  {
    const Data & ft = **iD;

    // Make a new packed data object and save
    pfts.data().push_back( PackedData() );
    PackedData & pft = pfts.data().back();

    // fill ppart key from part
    pft.key = ft.key();

    // Pack all the physics information
    pack( ft, pft, pfts );
  }

}

void FlavourTagPacker::unpack( const PackedData       & pft,
                               Data                   & ft,
                               const PackedDataVector & pfts,
                               DataVector             & fts ) const
{
  
  // Decision
  ft.setDecision( pft.decision );
  ft.setCategory( pft.category );
  ft.setOmega   ( m_pack.fraction(pft.omega) );

  // OS Decision
  ft.setDecisionOS( pft.decisionOS );
  ft.setCategoryOS( pft.categoryOS );
  ft.setOmegaOS   ( m_pack.fraction(pft.omegaOS) );

  // Tagging B
  if ( -1 != pft.taggedB )
  {
    int hintID(0), key(0);
    m_pack.hintAndKey64( pft.taggedB, &pfts, &fts, hintID, key );
    SmartRef<LHCb::Particle> ref(&fts,hintID,key);
    ft.setTaggedB( ref );
  }

  // Taggers
  std::vector<LHCb::Tagger> taggers;
  taggers.reserve( pft.lastTagger - pft.firstTagger );
  for ( unsigned int iT = pft.firstTagger; iT < pft.lastTagger; ++iT )
  {

    // Reference to packed tagger
    const LHCb::PackedTagger & ptagger = pfts.taggers()[iT];

    // Make a new tagger
    taggers.push_back( LHCb::Tagger() );
    LHCb::Tagger & tagger = taggers.back();

    // set the tagger members
    tagger.setType    ( ptagger.type                   );
    tagger.setDecision( ptagger.decision               );
    tagger.setOmega   ( m_pack.fraction(ptagger.omega) );

    // tagging particles
    for ( unsigned int iP = ptagger.firstTagP; iP < ptagger.lastTagP; ++iP )
    {
      int hintID(0), key(0);
      m_pack.hintAndKey64( pfts.taggeringPs()[iP],
                           &pfts, &fts, hintID, key );
      SmartRef<LHCb::Particle> ref(&fts,hintID,key);
      tagger.addToTaggerParts( ref );
    }

  }
  ft.setTaggers( taggers );

}

void FlavourTagPacker::unpack( const PackedDataVector & pfts,
                               DataVector             & fts ) const
{
  fts.reserve( pfts.data().size() );

  for ( PackedDataVector::Vector::const_iterator iD = pfts.data().begin();
        iD != pfts.data().end(); ++iD )
  {
    const PackedData & pft = *iD;

    // make and save new pid in container
    Data * ft = new Data();
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
  DataVector::const_iterator iA(dataA.begin()), iB(dataB.begin());
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
  ok &= dataA.key() == dataB.key();

  // decision
  ok &= ch.compareInts( "Decision", dataA.decision(), dataB.decision() );
  // category
  ok &= ch.compareInts( "Category", dataA.category(), dataB.category() );
  // omega
  ok &= ch.compareFloats( "Omega", dataA.omega(), dataB.omega(), 1e-4 );

  // decisionOS
  ok &= ch.compareInts( "DecisionOS", dataA.decisionOS(), dataB.decisionOS() );
  // categoryOS
  ok &= ch.compareInts( "CategoryOS", dataA.categoryOS(), dataB.categoryOS() );
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
    std::vector<LHCb::Tagger>::const_iterator iA = dataA.taggers().begin();
    std::vector<LHCb::Tagger>::const_iterator iB = dataB.taggers().begin();
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
        SmartRefVector<LHCb::Particle>::const_iterator iPA = iA->taggerParts().begin();
        SmartRefVector<LHCb::Particle>::const_iterator iPB = iB->taggerParts().begin();
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
