// $Id: HltSelection2Decision.h,v 1.1 2008-09-04 12:20:01 graven Exp $
#ifndef HLTCOMMON_HLTSELECTION2DECISION_H 
#define HLTCOMMON_HLTSELECTION2DECISION_H 1

// Include files
// from Gaudi

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/IANNSvc.h"
#include "HltBase/IHltDataSvc.h"
#include "HltBase/HltSelection.h"

/** @class HltSelection2Decision HltSelection2Decision.h
 *  
 *  functionality:
 *      Takes one singe HltSelection, and creates a single DecReport,
 *      terminating a trigger line (i.e. this algorithm does not have
 *      any explicit output). As a result, this algorithm is only 
 *      expected to run in case the input selection has passed...
 *
 *  Options:
 *  @author Gerhard Raven
 *  @date   2008-08-25
 */
class HltSelection2Decision : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltSelection2Decision( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSelection2Decision( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  // hlt data provided service
  mutable IHltDataSvc* m_dataSvc;
  IHltDataSvc& dataSvc() const {
        if (m_dataSvc == 0)  m_dataSvc = svc<IHltDataSvc>("HltDataSvc");
        return *m_dataSvc;
  }
  // assigned names and numbers service...
  mutable IANNSvc *m_annSvc;
  IANNSvc&     annSvc() const {
        if (m_annSvc == 0)  m_annSvc = svc<IANNSvc>("HltANNSvc");
        return *m_annSvc;
  }

  Hlt::Selection* m_input;
  stringKey m_inputName;
  std::string m_decisionName;
  std::string m_location;
};
#endif // HLTSELECTIONENTRY_H
