// $Id: L0DecReportsMaker.h,v 1.1 2010-06-23 22:50:27 tskwarni Exp $
#ifndef L0DECREPORTSMAKER_H 
#define L0DECREPORTSMAKER_H 1

// Include files
#include <string>
// from Gaudi
#include "GaudiKernel/VectorMap.h"
#include "Event/HltObjectSummary.h"
#include "GaudiAlg/GaudiAlgorithm.h"


namespace LHCb {
  class L0MuonCandidate;  
  class L0CaloCandidate;  
}


/** @class L0DecReportsMaker L0DecReportsMaker.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2010-06-23
 *
 *  Algorithm to translate L0DO report into HltDecReport
 *
 */
class L0DecReportsMaker : public GaudiAlgorithm {

public:

  

  /// Standard constructor
  L0DecReportsMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0DecReportsMaker( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  // ----------------------- data members 

  /// location of input L0 DU Report
  StringProperty m_inputL0DUReportLocation;

  /// location of output Hlt Summary
  StringProperty m_outputHltDecReportsLocation;

};

#endif // L0SELREPORTSMAKER_H
