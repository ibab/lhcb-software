// $Id: CopyODIN.h,v 1.9 2009-04-30 15:43:48 jpalac Exp $
#ifndef COPYODIN_H 
#define COPYODIN_H 1

// Include files
// MicroDST
#include "MicroDST/ObjectClonerAlg.h"
#include "MicroDST/BindType2Cloner.h"
// from LHCb
#include "Event/ODIN.h"

/** @class CopyODIN CopyODIN.h
 *  
 *  MicroDSTAlgorithm to clone an LHCb::ODIN from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::ODIN is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *  If no InputLocation is specified the header is taken from LHCb::ODINLocation::Default
 *
 *  <b>Example</b>: Clone the LHCb::ODIN from default location ("DAQ/ODIN") 
 *  to "/Event/MyLocation/DAQ/ODIN"
 *  @code
 *  mySeq = GaudiSequencer("SomeSequence")
 *  copyODIN = CopyODIN()
 *  copyODIN.OutputPrefix = "MyLocation"
 *  mySeq.Members += [copyODIN]
 *  @endcode
 *
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-15
 */
//=============================================================================
template <> struct Location<LHCb::ODIN> {
  const static std::string Default;
};  
const std::string Location<LHCb::ODIN>::Default = LHCb::ODINLocation::Default;
//=============================================================================
typedef MicroDST::ObjectClonerAlg<LHCb::ODIN> CopyODIN;
// Declaration of the Algorithm Factory
DECLARE_NAMED_ALGORITHM_FACTORY( CopyODIN, CopyODIN )

#endif // COPYODIN_H
