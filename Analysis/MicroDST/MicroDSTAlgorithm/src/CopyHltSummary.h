// $Id: CopyHltSummary.h,v 1.3 2008-09-01 17:12:56 jpalac Exp $
#ifndef COPYHLTSUMMARY_H 
#define COPYHLTSUMMARY_H 1

// Include files
// from MicroDST
#include "MicroDST/ObjectClonerAlg.h"
// from LHCb
#include "Event/HltSummary.h"
#include "Defaults.h"
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
template <> struct BindType2Cloner<LHCb::HltSummary> 
{
  typedef LHCb::HltSummary type;
  typedef MicroDST::BasicCopy<LHCb::HltSummary> cloner;
};
//=============================================================================
template <> struct Location<LHCb::HltSummary> 
{
  const static std::string Default;
};
const std::string Location<LHCb::HltSummary>::Default = LHCb::HltSummaryLocation::Default;
//=============================================================================
typedef MicroDST::ObjectClonerAlg<LHCb::HltSummary> CopyHltSummary;
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyHltSummary )
#endif // COPYHLTSUMMARY_H
