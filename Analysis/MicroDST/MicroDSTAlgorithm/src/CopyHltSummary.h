// $Id: CopyHltSummary.h,v 1.1 2008-04-16 13:19:19 jpalac Exp $
#ifndef COPYHLTSUMMARY_H 
#define COPYHLTSUMMARY_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"


/** @class CopyHltSummary CopyHltSummary.h
 *  
 *  MicroDSTAlgorithm to clone an LHCb::HltSummary from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::HltSummary is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *  If no InputLocation is specified the header is taken from LHCb::HltSummaryLocation::Default
 *
 *  <b>Example</b>: Clone the LHCb::HltSummary from default location ("Hlt/Summary") 
 *  to "/Event/MyLocation/Hlt/Summary"
 *  @code
 *  // Add a sequencer
 *  ApplicationMgr.TopAlg += { "GaudiSequencer/MyStuffCopier" } ;
 *  MyStuffCopier.Members += {"CopyHltSummary"};
 *  CopyHltSummary.OutputPrefix = "MyLocation";
 *  @endcode
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2008-04-16
 */
class CopyHltSummary : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyHltSummary( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyHltSummary( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  typedef MicroDST::BasicCopy<LHCb::HltSummary> HltSummaryCopy;
};
#endif // COPYHLTSUMMARY_H
