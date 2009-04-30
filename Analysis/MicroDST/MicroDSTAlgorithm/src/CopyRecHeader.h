// $Id: CopyRecHeader.h,v 1.8 2009-04-30 15:06:09 jpalac Exp $
#ifndef COPYRECHEADER_H 
#define COPYRECHEADER_H 1

// MicroDST
#include "MicroDST/ObjectClonerAlg.h"

// Include files
// from LHCb
#include "Event/RecHeader.h"


/** @class CopyRecHeader CopyRecHeader.h
 *  
 *  MicroDSTAlgorithm to clone an LHCb::RecHeader from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::RecHeader is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *  If no InputLocation is specified the header is taken from LHCb::RecHeaderLocation::Default
 *
 *  <b>Example</b>: Clone the LHCb::RecHeader from default location ("Rec/Header") 
 *  to "/Event/MyLocation/Rec/Header"
 *  @code
 *  // Add a sequencer
 *  ApplicationMgr.TopAlg += { "GaudiSequencer/MyStuffCopier" } ;
 *  MyStuffCopier.Members += {""MicroDST::ObjectClonerAlg<LHCb::RecHeader>/CopyRecHeader"};
 *  CopyRecHeader.OutputPrefix = "MyLocation";
 *  @endcode
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2007-10-15
 */
//=============================================================================
template <> struct Location<LHCb::RecHeader> 
{
  const static std::string Default;
};
const std::string Location<LHCb::RecHeader>::Default = LHCb::RecHeaderLocation::Default;
//=============================================================================
typedef MicroDST::ObjectClonerAlg<LHCb::RecHeader> CopyRecHeader;
// Declaration of the Algorithm Factory
DECLARE_NAMED_ALGORITHM_FACTORY( CopyRecHeader, CopyRecHeader )
#endif // COPYRECHEADER_H
