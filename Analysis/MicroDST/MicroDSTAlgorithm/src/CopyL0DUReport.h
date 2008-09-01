// $Id: CopyL0DUReport.h,v 1.4 2008-09-01 17:28:14 jpalac Exp $
#ifndef COPYL0DUREPORT_H 
#define COPYL0DUREPORT_H 1

// Include files
// from MicroDST
#include "MicroDST/ObjectClonerAlg.h"
// from LHCb
#include "Event/L0DUReport.h"
/** @class CopyL0DUReport CopyL0DUReport.h
 *  
 *  MicroDSTAlgorithm to clone an LHCb::L0DUReport from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::L0DUReport is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *
 *  <b>Example</b>: Clone the LHCb::L0DUReport from default location ("Trig/L0/L0DUReport") 
 *  to "/Event/MyLocation/Trig/L0/L0DUReport"
 *  @code
 *  // Add a sequencer
 *  ApplicationMgr.TopAlg += { "GaudiSequencer/MyStuffCopier" } ;
 *  MyStuffCopier.Members += {"MicroDST::ObjectClonerAlg<LHCb::L0DUReport>/CopyL0DUReport"};
 *  CopyL0DUReport.InputLocation = "Trig/L0/L0DUReport";
 *  CopyL0DUReport.OutputPrefix = "MyLocation";
 *  @endcode
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2008-04-16
 */
//=============================================================================
template <> struct BindType2Cloner<LHCb::L0DUReport> 
{
  typedef LHCb::L0DUReport type;
  typedef MicroDST::BasicCopy<LHCb::L0DUReport> cloner;
};
//=============================================================================
template <> struct Location<LHCb::L0DUReport> 
{
  const static std::string Default;
};
const std::string Location<LHCb::L0DUReport>::Default = LHCb::L0DUReportLocation::Default;
//=============================================================================
typedef MicroDST::ObjectClonerAlg<LHCb::L0DUReport> CopyL0DUReport;
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyL0DUReport )
#endif // COPYL0DUREPORT_H
