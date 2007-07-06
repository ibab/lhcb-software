// $Id: HltSummaryWriter.h,v 1.2 2007-07-06 20:39:47 hernando Exp $
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

  template <class T>
  void printInfo(const std::string& title, const T& con) {
    typedef typename T::const_iterator iter;
    for (iter it = con.begin(); it != con.end(); ++it)
      printInfo(title,*(*it));  
  }
  
  void printInfo(const std::string& title, const LHCb::Track& track);
  
  void printInfo(const std::string& title, const LHCb::RecVertex& ver);

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
