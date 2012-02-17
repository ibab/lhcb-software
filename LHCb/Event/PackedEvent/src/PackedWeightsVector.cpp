// $Id: PackedWeightsVector.cpp,v 1.9 2010-04-11 14:27:15 jonrob Exp $

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
  pweightsV.data().reserve( weightsV.size() );
  if ( 1 == pweightsV.packingVersion() ||
       0 == pweightsV.packingVersion() )
  {
    for ( DataVector::const_iterator iD = weightsV.begin();
          iD != weightsV.end(); ++iD )
    {
      // original data
      const Data & weights = **iD;

      // new packed data
      pweightsV.data().push_back( PackedData() );
      PackedData & pweights = pweightsV.data().back();

      // Save the PV key
      pweights.pvKey = weights.key();

      // fill packed data
      pweights.firstWeight = pweightsV.weights().size();
      pweightsV.weights().reserve( pweightsV.weights().size() + weights.weights().size() );
      for ( LHCb::WeightsVector::WeightDataVector::const_iterator iW = weights.weights().begin();
            iW != weights.weights().end(); ++iW )
      {
        pweightsV.weights().push_back( LHCb::PackedWeight(iW->first,m_pack.fraction(iW->second)) );
      }
      pweights.lastWeight = pweightsV.weights().size();

    }

  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pweightsV.packingVersion();
    throw GaudiException( mess.str(), "WeightsVectorPacker", StatusCode::FAILURE );
  }
}

void WeightsVectorPacker::unpack( const PackedDataVector & pweightsV,
                                  DataVector       & weightsV ) const
{
  weightsV.reserve( pweightsV.data().size() );
  if ( 1 == pweightsV.packingVersion() ||
       0 == pweightsV.packingVersion() )
  {
   for ( PackedDataVector::WeightsVector::const_iterator iD = pweightsV.data().begin();
          iD != pweightsV.data().end(); ++iD )
    {
      // packed vector of weights
      const PackedData & pweights = *iD;

      // make and save new unpacked data
      Data * weights  = new Data();
      if ( 0 == pweightsV.packingVersion() )
      {
        weightsV.insert( weights );
      }
      else
      { 
        weightsV.insert( weights, pweights.pvKey );
      }

      // fill the unpacked weights vector
      LHCb::WeightsVector::WeightDataVector & wWeights = 
        *(const_cast<LHCb::WeightsVector::WeightDataVector*>(&weights->weights()));
      wWeights.reserve( pweights.lastWeight - pweights.firstWeight );
      for ( unsigned short int iW = pweights.firstWeight; iW < pweights.lastWeight; ++iW )
      {
        const PackedWeight & pweight = pweightsV.weights()[iW];
        wWeights.push_back( LHCb::WeightsVector::WeightData( pweight.key,
                                                             (float)m_pack.fraction(pweight.weight) ) );
      }

    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pweightsV.packingVersion();
    throw GaudiException( mess.str(), "WeightsVectorPacker", StatusCode::FAILURE );
  }
}

StatusCode WeightsVectorPacker::check( const DataVector & dataA,
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

    // loop over weights and test
    const bool sizeOK = (*iA)->weights().size() == (*iB)->weights().size();
    ok &= sizeOK;
    if ( sizeOK ) 
    {
      LHCb::WeightsVector::WeightDataVector::const_iterator iWA((*iA)->weights().begin());
      LHCb::WeightsVector::WeightDataVector::const_iterator iWB((*iB)->weights().begin());
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
      parent.warning() << "Problem with WeightsVector data packing :-" << endmsg
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
