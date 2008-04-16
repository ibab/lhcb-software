// $Id: CopyL0DUReport.h,v 1.1 2008-04-16 13:19:53 jpalac Exp $
#ifndef COPYL0DUREPORT_H 
#define COPYL0DUREPORT_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"


/** @class CopyL0DUReport CopyL0DUReport.h
 *  
 *  MicroDSTAlgorithm to clone an LHCb::L0DUReport from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::L0DUReport is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *  If no InputLocation is specified the header is taken from LHCb::L0DUReportLocation::Default
 *
 *  <b>Example</b>: Clone the LHCb::L0DUReport from default location ("Trig/L0/L0DUReport") 
 *  to "/Event/MyLocation/Trig/L0/L0DUReport"
 *  @code
 *  // Add a sequencer
 *  ApplicationMgr.TopAlg += { "GaudiSequencer/MyStuffCopier" } ;
 *  MyStuffCopier.Members += {"CopyL0DUReport"};
 *  CopyL0DUReport.OutputPrefix = "MyLocation";
 *  @endcode
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2008-04-16
 */
class CopyL0DUReport : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyL0DUReport( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyL0DUReport( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  typedef MicroDST::BasicCopy<LHCb::L0DUReport> L0DUReportCopy;
};
#endif // COPYL0DUREPORT_H
