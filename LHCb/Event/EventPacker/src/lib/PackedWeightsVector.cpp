
// local
#include "Event/PackedWeightsVector.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void WeightsVectorPacker::pack( const DataVector & weightsV,
                                PackedDataVector & pweightsV ) const
{
  const auto pVer = pweightsV.packingVersion();
  if ( 1 == pVer || 0 == pVer )
  {
    pweightsV.data().reserve( weightsV.size() );
    for ( const Data * weights : weightsV )
    {
      // new packed data
      pweightsV.data().push_back( PackedData() );
      auto & pweights = pweightsV.data().back();

      // Save the PV key
      pweights.pvKey = weights->key();

      // fill packed data
      pweights.firstWeight = pweightsV.weights().size();
      pweightsV.weights().reserve( pweightsV.weights().size() + weights->weights().size() );
      for ( const auto& W : weights->weights() )
      {
        pweightsV.weights().emplace_back( LHCb::PackedWeight(W.first,m_pack.fraction(W.second)) );
      }
      pweights.lastWeight = pweightsV.weights().size();

    }

  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pVer;
    throw GaudiException( mess.str(), "WeightsVectorPacker", StatusCode::FAILURE );
  }
}

void WeightsVectorPacker::unpack( const PackedDataVector & pweightsV,
                                  DataVector       & weightsV ) const
{
  const auto pVer = pweightsV.packingVersion();
  if ( 1 == pVer || 0 == pVer )
  {
    weightsV.reserve( pweightsV.data().size() );
    for ( const PackedData & pweights : pweightsV.data() )
    {
      // make and save new unpacked data
      auto * weights  = new Data();
      if ( 0 == pVer ) { weightsV.insert( weights ); }
      else             { weightsV.insert( weights, pweights.pvKey ); }

      // fill the unpacked weights vector
      auto & wWeights =
        *(const_cast<LHCb::WeightsVector::WeightDataVector*>(&weights->weights()));
      wWeights.reserve( pweights.lastWeight - pweights.firstWeight );
      for ( auto iW = pweights.firstWeight; iW < pweights.lastWeight; ++iW )
      {
        const auto & pweight = pweightsV.weights()[iW];
        wWeights.emplace_back( LHCb::WeightsVector::WeightData( pweight.key,
                                                                (float)m_pack.fraction(pweight.weight) ) );
      }

    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pVer;
    throw GaudiException( mess.str(), "WeightsVectorPacker", StatusCode::FAILURE );
  }
}

StatusCode WeightsVectorPacker::check( const DataVector & dataA,
                                       const DataVector & dataB ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // checker
  const DataPacking::DataChecks ch(parent());

  // Loop over data containers together and compare
  auto iA(dataA.begin()), iB(dataB.begin());
  for ( ; iA != dataA.end() && iB != dataB.end(); ++iA, ++iB )
  {
    // assume OK from the start
    bool ok = true;

    // loop over weights and test
    const bool sizeOK = ch.compareInts( "#Weights",
                                        (*iA)->weights().size(), (*iB)->weights().size() );
    ok &= sizeOK;
    if ( sizeOK )
    {
      auto iWA((*iA)->weights().begin()), iWB((*iB)->weights().begin());
      for ( ; iWA != (*iA)->weights().end() && iWB != (*iB)->weights().end(); ++iWA, ++iWB )
      {
        ok &= (*iWA).first == (*iWB).first;
        ok &= ch.compareDoubles( "Weight", (*iWA).second, (*iWB).second );
      }
    }

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent().warning() << "Problem with WeightsVector data packing :-" << endmsg
                         << "  Original Weight : " << *iA
                         << endmsg
                         << "  Unpacked Weight : " << *iB
                         << endmsg;
      sc = StatusCode::FAILURE;
    }
  }

  // Return final status
  return sc;
}
