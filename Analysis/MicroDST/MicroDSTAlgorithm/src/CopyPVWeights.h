// $Id: CopyPVWeights.h,v 1.9 2009-04-30 15:43:48 jpalac Exp $
#ifndef COPYPVWEIGHTS_H 
#define COPYPVWEIGHTS_H 1

// MicroDST
#include "MicroDST/KeyedContainerClonerAlg.h"
#include <MicroDST/ICloneWeightsVector.h>
#include "MicroDST/BindType2ClonerDef.h"
// Include files
// from LHCb
#include "Event/WeightsVector.h"


/** @class CopyPVWeights CopyPVWeights.h
 *  
 *  MicroDSTAlgorithm to clone an LHCb::WeightsVector::Container from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::RecHeader is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *  If no InputLocation is specified the header is taken from LHCb::RecHeaderLocation::Default
 *
 *  <b>Example</b>: Clone the LHCb::WeightsVector::Container from default location ("Rec/Vertex/Weights") 
 *  to "/Event/MyLocation/Rec/Vertex/Weights"
 *  @code
 *  mySeq = GaudiSequencer("SomeSequence")
 *  copyPVWeights = CopyPVWeights()
 *  copyPVWeights.OutputPrefix = "MyLocation"
 *  mySeq.Members += [copyPVWeights]
 *  @endcode
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2007-10-15
 */
//=============================================================================
template <> struct BindType2Cloner<LHCb::WeightsVector>
{  
  typedef LHCb::WeightsVector type;
  typedef ICloneWeightsVector Cloner; 
};
//=============================================================================
template<> struct Defaults<LHCb::WeightsVector>
{  
  const static std::string clonerType; 
};
//=============================================================================
const std::string Defaults<LHCb::WeightsVector>::clonerType = "WeightsVectorCloner";
template <> struct Location<LHCb::WeightsVector> 
{
  const static std::string Default;
};
const std::string Location<LHCb::WeightsVector>::Default = LHCb::WeightsVectorLocation::Default;
//=============================================================================
typedef MicroDST::KeyedContainerClonerAlg<LHCb::WeightsVector> CopyPVWeights;
// Declaration of the Algorithm Factory
DECLARE_NAMED_ALGORITHM_FACTORY( CopyPVWeights, CopyPVWeights )
#endif // COPYPVWEIGHTS_H
