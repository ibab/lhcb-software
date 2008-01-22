// $Id: HltSummaryWriter.h,v 1.3 2008-01-22 09:56:37 hernando Exp $
#ifndef HLTCOMMON_HLTSUMMARYWRITER_H 
#define HLTCOMMON_HLTSUMMARYWRITER_H 1

// Include files
// from Gaudi
#include "HltBase/HltBaseAlg.h"
#include "Event/HltSummary.h"

/** @class HltHadAlleyPreTrigger HltHadAlleyPreTrigger.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-28
 */
class HltSummaryWriter : public HltBaseAlg {
public: 
  /// Standard constructor
  HltSummaryWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSummaryWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

public:

  void writeSelection(int id);


protected:

  LHCb::HltSummary* m_summary;

  bool m_saveAll;

  StringArrayProperty m_AselectionNames;
  // StringArrayProperty m_monitorConditions;


  std::vector<std::string> m_selectionNames;
  std::vector<int> m_selectionIDs;

protected:

  std::string m_hltSummaryLocation;


};
#endif // HLTHADALLEYPRETRIGGER_H
