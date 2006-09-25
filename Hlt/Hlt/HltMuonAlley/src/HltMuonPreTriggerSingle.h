// $Id: HltMuonPreTriggerSingle.h,v 1.1.1.1 2006-09-25 16:29:11 asatta Exp $
#ifndef HLTMUONPRETRIGGERSINGLE_H 
#define HLTMUONPRETRIGGERSINGLE_H 1

// Include files
// from Gaudi
#include "HltCommon/HltAlgorithm.h"
#include "HltCommon/HltFunctions.h"

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
