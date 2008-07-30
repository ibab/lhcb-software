// $Id: HltSummaryWriter.h,v 1.6 2008-07-30 13:37:33 graven Exp $
#ifndef HLTCOMMON_HLTSUMMARYWRITER_H 
#define HLTCOMMON_HLTSUMMARYWRITER_H 1

// Include files
// from Gaudi
#include "HltBase/HltBaseAlg.h"
#include "Event/HltSummary.h"
#include "Event/RecVertex.h"

/** @class HltSummaryWriter.h
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

private:

  void writeSelection(LHCb::HltSummary&, const stringKey& id);

  bool m_saveAll;

  StringArrayProperty m_AselectionNames;

  std::vector<stringKey> m_selectionIDs;

  std::string m_hltSummaryLocation;

};
#endif // HLTHADALLEYPRETRIGGER_H
