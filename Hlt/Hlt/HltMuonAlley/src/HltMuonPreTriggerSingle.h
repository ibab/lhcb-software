// $Id: HltMuonPreTriggerSingle.h,v 1.2 2006-09-27 14:18:04 cattanem Exp $
#ifndef HLTMUONPRETRIGGERSINGLE_H 
#define HLTMUONPRETRIGGERSINGLE_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

/** @class HltMuonAlleyPreTrigger HltMuonAlleyPreTrigger.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2006-09-07
 */
class HltMuonPreTriggerSingle : public HltAlgorithm {
public: 
  /// Standard constructor
  HltMuonPreTriggerSingle( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltMuonPreTriggerSingle( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  double m_ptMin;

  HltHisto m_histoPt;
  HltHisto m_histoPt1;

protected:

  int m_ptKey;
  Estd::function<LHCb::Track>* _ptFun;
  Estd::filter<LHCb::Track>*   _ptCut;

protected:

  Hlt::SortTrackByPt _sortByPT;

};
#endif // HLTMUONALLEYPRETRIGGERSINGLE_H
