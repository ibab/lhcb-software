#ifndef CHECKHLTDECISION_H 
#define CHECKHLTDECISION_H 1

// Include files
// from Gaudi


#include "Event/HltSummary.h"
#include "Event/L0DUReport.h"
//#include "Event/IL0Candidate.h"
#include "HltBase/HltBaseAlg.h"

/** @class CheckHLTDecision CheckHLTDecision.h
 *
 *  functionality:
 *        monitor the global Hlt results
 *        make histograms with Hlt diagnostics info
 *
 *  @author Arthur Maciel
 *  @date   2003-03-06
 */

class CheckHLTDecision : public HltBaseAlg {
public: 
  CheckHLTDecision( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CheckHLTDecision( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
  void monitorL0();
  void monitorAlleysinput();
  void monitorHLT();


};
#endif // CHECKHLTDECISION_H
