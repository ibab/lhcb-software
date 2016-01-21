// $Id: PackedMCRichDigitSummary.cpp,v 1.5 2010-04-11 14:27:14 jonrob Exp $

// local
#include "Event/PackedMCRichDigitSummary.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void MCRichDigitSummaryPacker::pack( const DataVector & sums,
                                     PackedDataVector & psums ) const
{
  const char ver = psums.packingVersion();
  if ( 0 == ver || 1 == ver )
  {
    psums.data().reserve( sums.size() );
    for ( const auto * sum : sums )
    {
      // make new packed data
      psums.data().emplace_back( PackedData() );
      auto & psum = psums.data().back();
      // fill packed data
      psum.history     = sum->history().historyCode();
      psum.richSmartID = sum->richSmartID().key();
      if ( sum->mcParticle() )
      {
        psum.mcParticle = ( UNLIKELY( 0 == ver ) ?
                            m_pack.reference32( &psums,
                                                sum->mcParticle()->parent(),
                                                sum->mcParticle()->key() ) :
                            m_pack.reference64( &psums,
                                                sum->mcParticle()->parent(),
                                                sum->mcParticle()->key() ) );
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "MCRichDigitSummaryPacker", StatusCode::FAILURE );
  }
}

void MCRichDigitSummaryPacker::unpack( const PackedDataVector & psums,
                                       DataVector       & sums ) const
{
  const char ver = psums.packingVersion();
  if ( 0 == ver || 1 == ver )
  {
    sums.reserve( psums.data().size() );
    for ( const auto & psum : psums.data() )
    {
      // make and save new sum in container
      auto * sum = new Data();
      sums.add( sum );
      // Fill data from packed object
      sum->setHistory( LHCb::MCRichDigitHistoryCode(psum.history) );
      sum->setRichSmartID( LHCb::RichSmartID(psum.richSmartID) );
      if ( -1 != psum.mcParticle )
      {
        int hintID(0), key(0);
        if ( ( 0!=ver && m_pack.hintAndKey64(psum.mcParticle,&psums,&sums,hintID,key) ) ||
             ( 0==ver && m_pack.hintAndKey32(psum.mcParticle,&psums,&sums,hintID,key) ) )
        {
          SmartRef<LHCb::MCParticle> ref(&sums,hintID,key);
          sum->setMCParticle( ref );
        }
        else { parent().Error( "Corrupt MCRichDigitSummary MCParticle SmartRef detected." ).ignore(); }
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "MCRichDigitSummaryPacker", StatusCode::FAILURE );
  }
}

StatusCode MCRichDigitSummaryPacker::check( const DataVector & dataA,
                                            const DataVector & dataB ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // checker
  const DataPacking::DataChecks ch(parent());

  // Loop over data containers together and compare
  DataVector::const_iterator iA(dataA.begin()), iB(dataB.begin());
  for ( ; iA != dataA.end() && iB != dataB.end(); ++iA, ++iB )
  {
    // assume OK from the start
    bool ok = true;
    // Detector ID
    ok &= ch.compareInts( "RichSmartID", (*iA)->richSmartID(), (*iB)->richSmartID() );
    // History code
    ok &= ch.compareInts( "HistoryCode", (*iA)->history().historyCode(), (*iB)->history().historyCode() );
    // MCParticle reference
    ok &= ch.comparePointers( "MCParticle", (*iA)->mcParticle(), (*iB)->mcParticle() );

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent().warning() << "Problem with MCRichDigitSummary data packing :-" << endmsg
                         << "  Original Summary : " << **iA
                         << endmsg
                         << "  Unpacked Summary : " << **iB
                         << endmsg;
      sc = StatusCode::FAILURE;
    }
  }

  // Return final status
  return sc;
}
