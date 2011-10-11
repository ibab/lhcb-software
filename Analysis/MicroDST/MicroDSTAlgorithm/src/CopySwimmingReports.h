// $Id: CopySwimmingReports.h,v 1.3 2009-04-30 15:43:48 jpalac Exp $
#ifndef COPYSWIMMINGREPORTS_H 
#define COPYSWIMMINGREPORTS_H 1

// Include files
// MicroDST
#include "MicroDST/KeyedContainerClonerAlg.h"
#include "MicroDST/BindType2Cloner.h"
#include <MicroDST/ICloneSwimmingReport.h>

// from LHCb
#include "Event/SwimmingReport.h"

/** @class CopySwimmingReports CopySwimmingReports.h
 *  
 *  MicroDSTAlgorithm to clone an LHCb::SwimmingReports from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::SwimmingReports is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *
 *  <b>Example</b>: Clone the LHCb::SwimmingReports from default location ("Phys/Swimming/Reports") 
 *  to "/Event/MyLocation/Phys/Swimming/Reports"
 *  @code
 *  mySeq = GaudiSequencer("SomeSequence")
 *  copySwimming = CopySwimmingReports()
 *  mySeq.Members += [copySwimming]
 *  copySwimming.OutputPrefix = "MyLocation"
 *  @endcode
 *
 *
 *  @author Roel Aaij
 *  @date   2011-10-09
 */
//=============================================================================
template <> struct BindType2Cloner<LHCb::SwimmingReport>
{
  typedef LHCb::SwimmingReport Type;
  typedef ICloneSwimmingReport Cloner;
};
//=============================================================================
template<> struct Defaults<LHCb::SwimmingReport>
{
  const static std::string clonerType;
};
const std::string Defaults<LHCb::SwimmingReport>::clonerType = "SwimmingReportCloner";
//=============================================================================
template <> struct Location<LHCb::SwimmingReport>
{
  const static std::string Default;
};  
const std::string Location<LHCb::SwimmingReport>::Default = LHCb::SwimmingReportsLocation::Default;
//=============================================================================
typedef MicroDST::KeyedContainerClonerAlg<LHCb::SwimmingReport> CopySwimmingReports;
// Declaration of the Algorithm Factory
DECLARE_NAMED_ALGORITHM_FACTORY( CopySwimmingReports, CopySwimmingReports )

#endif // COPYSWIMMINGREPORTS_H
