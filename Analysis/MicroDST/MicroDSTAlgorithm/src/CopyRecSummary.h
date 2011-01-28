#ifndef COPYRECSUMMARY_H 
#define COPYRECSUMMARY_H 1

// MicroDST
#include "MicroDST/ObjectClonerAlg.h"

// Include files
// from LHCb
#include "Event/RecSummary.h"


/** @class CopyRecSummary CopyRecSummary.h
 *  
 *  MicroDSTAlgorithm to clone an LHCb::RecSummary from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::RecSummary is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *  If no InputLocation is specified the header is taken from LHCb::RecSummaryLocation::Default
 *
 *  <b>Example</b>: Clone the LHCb::RecSummary from default location ("Rec/Header") 
 *  to "/Event/MyLocation/Rec/Header"
 *  @code
 *  mySeq = GaudiSequencer("SomeSequence")
 *  copyRecSummary = CopyRecSummary()
 *  copyRecSummary.OutputPrefix = "MyLocation"
 *  mySeq.Members += [copyRecSummary]
 *  @endcode
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2007-10-15
 */
//=============================================================================
template <> struct Location<LHCb::RecSummary> 
{
  const static std::string Default;
};
const std::string Location<LHCb::RecSummary>::Default = LHCb::RecSummaryLocation::Default;
//=============================================================================
typedef MicroDST::ObjectClonerAlg<LHCb::RecSummary> CopyRecSummary;
// Declaration of the Algorithm Factory
DECLARE_NAMED_ALGORITHM_FACTORY( CopyRecSummary, CopyRecSummary )
#endif // COPYRECSUMMARY_H
