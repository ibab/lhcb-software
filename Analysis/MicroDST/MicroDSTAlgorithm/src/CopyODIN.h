// $Id: CopyODIN.h,v 1.5 2008-08-29 07:08:56 jpalac Exp $
#ifndef COPYODIN_H 
#define COPYODIN_H 1

// Include files
// MicroDST
#include "MicroDST/ObjectClonerAlg.h"
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
 *  If no InputLocation is specified the header is taken from LHCb::ODIN::Location::Default
 *
 *  <b>Example</b>: Clone the LHCb::ODIN from default location ("DAQ/ODIN") 
 *  to "/Event/MyLocation/DAQ/ODIN"
 *  @code
 *  // Add a sequencer
 *  ApplicationMgr.TopAlg += { "GaudiSequencer/MyStuffCopier" } ;
 *  MyStuffCopier.Members += {"MicroDST::ObjectClonerAlg<LHCb::ODIN>/CopyODIN"};
 *  CopyODIN.OutputPrefix = "MyLocation";
 *  @endcode
 *
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-15
 */

typedef MicroDST::ObjectClonerAlg<LHCb::ODIN> CopyODIN;
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyODIN )

#endif // COPYODIN_H
