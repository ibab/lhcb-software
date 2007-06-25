// $Id: HltSummaryWriter.h,v 1.1 2007-06-25 20:50:26 hernando Exp $
#ifndef HLTSUMMARYWRITER_H 
#define HLTSUMMARYWRITER_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/HltSummary.h"
#include "HltBase/HltTypes.h"

/** @class HltHadAlleyPreTrigger HltHadAlleyPreTrigger.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-28
 */
class HltSummaryWriter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltSummaryWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSummaryWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  void writeSelection(int id);

  void printInfo(const LHCb::HltSummary& sum, int id);

protected:

  LHCb::HltSummary* m_summary;

  StringArrayProperty m_selectionNames;
  // StringArrayProperty m_monitorConditions;

  bool m_saveAll;

  std::vector<int> m_selectionIDs;

protected:

  std::string m_dataSummaryLocation;

  IDataProviderSvc* m_hltSvc;
  Hlt::Configuration* m_conf;
  LHCb::HltSummary* m_datasummary;  

};
#endif // HLTHADALLEYPRETRIGGER_H
