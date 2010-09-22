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
  psums.data().reserve( sums.size() );
  if ( 0 == psums.packingVersion() )
  {
    for ( DataVector::const_iterator iD = sums.begin();
          iD != sums.end(); ++iD )
    {
      const Data & sum = **iD;
      // make new packed data
      psums.data().push_back( PackedData() );
      PackedData & psum = psums.data().back();
      // fill packed data
      psum.history     = sum.history().historyCode();
      psum.richSmartID = sum.richSmartID().key();
      if ( NULL != sum.mcParticle() )
      {
        psum.mcParticle = m_pack.reference( &psums,
                                            sum.mcParticle()->parent(),
                                            sum.mcParticle()->key() );
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)psums.packingVersion();
    throw GaudiException( mess.str(), "MCRichDigitSummaryPacker", StatusCode::FAILURE );
  }
}

void MCRichDigitSummaryPacker::unpack( const PackedDataVector & psums,
                                       DataVector       & sums ) const
{
  sums.reserve( psums.data().size() );
  if ( 0 == psums.packingVersion() )
  {
    for ( PackedDataVector::Vector::const_iterator iD = psums.data().begin();
          iD != psums.data().end(); ++iD )
    {
      const PackedData & psum = *iD;
      // make and save new sum in container
      Data * sum  = new Data();
      sums.add( sum );
      // Fill data from packed object
      sum->setHistory( LHCb::MCRichDigitHistoryCode(psum.history) );
      sum->setRichSmartID( LHCb::RichSmartID(psum.richSmartID) );
      if ( -1 != psum.mcParticle )
      {
        int hintID(0), key(0);
        m_pack.hintAndKey( psum.mcParticle, &psums, &sums, hintID, key );
        SmartRef<LHCb::MCParticle> ref(&sums,hintID,key);
        sum->setMCParticle( ref );
      }
    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)psums.packingVersion();
    throw GaudiException( mess.str(), "MCRichDigitSummaryPacker", StatusCode::FAILURE );
  }
}

StatusCode MCRichDigitSummaryPacker::check( const DataVector & dataA,
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
    // Detector ID
    ok &= (*iA)->richSmartID() == (*iB)->richSmartID();
    // History code
    ok &= (*iA)->history().historyCode() == (*iB)->history().historyCode();
    // MCParticle reference
    ok &= (*iA)->mcParticle() == (*iB)->mcParticle();

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent.warning() << "Problem with MCRichDigitSummary data packing :-" << endmsg
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
