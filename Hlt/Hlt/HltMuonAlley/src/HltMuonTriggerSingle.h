// $Id: HltMuonTriggerSingle.h,v 1.1.1.1 2006-09-25 16:29:11 asatta Exp $
#ifndef HLTMUONTRIGGERSINGLE_H 
#define HLTMUONTRIGGERSINGLE_H 1

// Include files
// from Gaudi
#include "HltCommon/HltAlgorithm.h"
#include "HltCommon/HltFunctions.h"

/** @class HltMuonAlleyTrigger HltMuonAlleyTrigger.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2006-09-07
 */
class HltMuonTriggerSingle : public HltAlgorithm {
public: 
  /// Standard constructor
  HltMuonTriggerSingle( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltMuonTriggerSingle( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  double m_ptMin;

  HltHisto m_histoPt;
  //  HltHisto m_histoIP;
  //  HltHisto m_histoPt1;

protected:

  int m_ptKey;
  //  int m_ipKey;
  Estd::function<LHCb::Track>* _ptFun;
  //  Estd::function<LHCb::Track>* _ptFun;
  Estd::filter<LHCb::Track>*   _ptCut;
  //  Hlt::trackvertex_bifunction* _ipFun;
protected:

  Hlt::SortTrackByPt _sortByPT;

};
#endif // HLTMUONALLEYTRIGGERSINGLE_H
