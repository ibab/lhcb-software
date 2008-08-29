// $Id: CopyHltSummary.h,v 1.2 2008-08-29 07:08:56 jpalac Exp $
#ifndef COPYHLTSUMMARY_H 
#define COPYHLTSUMMARY_H 1

// Include files
// from MicroDST
#include "MicroDST/ObjectClonerAlg.h"
// from LHCb
#include "Event/HltSummary.h"

/** @class CopyHltSummary CopyHltSummary.h
 *  
 *  MicroDSTAlgorithm to clone an LHCb::HltSummary from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::HltSummary is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *
 *  <b>Example</b>: Clone the LHCb::HltSummary from default location ("Hlt/Summary") 
 *  to "/Event/MyLocation/Hlt/Summary"
 *  @code
 *  // Add a sequencer
 *  ApplicationMgr.TopAlg += { "GaudiSequencer/MyStuffCopier" } ;
 *  MyStuffCopier.Members += {"MicroDST::ObjectClonerAlg<HltSummary>/CopyHltSummary"};
 *  CopyHltSummary.OutputPrefix = "MyLocation";
 *  CopyHltSummary.InputLocation = "Hlt/Summary";
 *  @endcode
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2008-04-16
 */
typedef MicroDST::ObjectClonerAlg<LHCb::HltSummary> CopyHltSummary;
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyHltSummary )
#endif // COPYHLTSUMMARY_H
