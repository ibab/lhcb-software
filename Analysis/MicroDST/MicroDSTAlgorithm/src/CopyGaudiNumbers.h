#ifndef COPYGAUDINUMBERS_H 
#define COPYGAUDINUMBERS_H 1

// MicroDST
#include "MicroDST/ObjectClonerAlg.h"

// Include files
// from LHCb
#include "Kernel/Counters.h"


/** @class CopyGaudiNumbers CopyGaudiNumbers.h
 *  
 *  MicroDSTAlgorithm to clone a Gaudi::Numbers from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The Gaudi::Numbers is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *
 *  <b>Example</b>: Clone the Gaudi::Numbers from location ("Counters/CounterAlg") 
 *  to "/Event/MyLocation/Counters/CounterAlg"
 *  @code
 *  mySeq = GaudiSequencer("SomeSequence")
 *  copyRecSummary = CopyGaudiNumbers()
 *  copyRecSummary.OutputPrefix = "MyLocation"
 *  mySeq.Members += [copyRecSummary]
 *  @endcode
 *
 *  @author Juan PALACIOS palacios@physik.uzh.ch
 *  @date   2011-02-14
 */
//=============================================================================
template <> struct Location<Gaudi::Numbers> 
{
  const static std::string Default;
};
const std::string Location<Gaudi::Numbers>::Default = "";
//=============================================================================
typedef MicroDST::ObjectClonerAlg<Gaudi::Numbers> CopyGaudiNumbers;
// Declaration of the Algorithm Factory
DECLARE_NAMED_ALGORITHM_FACTORY( CopyGaudiNumbers, CopyGaudiNumbers )
#endif // COPYGAUDINUMBERS_H
