// $Id: HltDecReportsMaker.h,v 1.2 2008-10-16 21:44:54 tskwarni Exp $
#ifndef HLTDECREPORTSMAKER_H 
#define HLTDECREPORTSMAKER_H 1

// Include files
// from Gaudi
#include "HltBase/HltBaseAlg.h"
#include "Event/HltDecReports.h"

#include "Kernel/IANNSvc.h"

/** @class HltDecReportsMaker HltDecReportsMaker.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2008-07-22
 *
 * Algorithm to traslate HltSummary to HltDecReports
 * 
 */
class HltDecReportsMaker : public HltBaseAlg {
public: 

  enum OutputInfoLevel { kNoInfoLevel=0, ///< don't save (not accepted for Decisions)
                         kPassInfoLevel=1, ///< save if successful
                         kErrorsInfoLevel=2, ///< save if errors
                         kPassOrErrorsInfoLevel=3, ///< save if errors or successful
                         kAlwaysInfoLevel=4 ///< always save if present in the configuration 
  };


  enum GlobalSelectionIDs { kHlt1GlobalID=1,
                            kHlt2GlobalID=2,
                            kL0GlobalID=3 
  };


  enum ErrorBitFlags { kDecisionNotConfigured=1
  };
  

  /// Standard constructor
  HltDecReportsMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltDecReportsMaker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  /// determine if and how selection should be saved (bit output see selectionPersistencyFlagBits enum)
  unsigned int infoLevelFlag( const std::string & selectionName );  

  /// location of input Hlt Summary
  StringProperty m_inputHltSummaryLocation;

  /// location of output Hlt Summary
  StringProperty m_outputHltDecReportsLocation;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;  

private:
  
  /// for setting per selection properties
  typedef std::map<std::string,int> SelectionSetting;
  typedef SimpleProperty< std::map<std::string,int> > SelectionSettingProp;
  
 /// Present output mode (0=normal 1=debug)
  unsigned int m_debugMode;

  /// debug event period (global, can't be change per selection)  0=never 1=always e.g. 100=every 100th event
  UnsignedIntegerProperty m_debugPeriod;

  /// event counter (reset at every debug event)
  unsigned int m_event;

  /// default output info level for Decision selections (can be overruled by per selection setting)
  UnsignedIntegerProperty m_infoLevelDecision;
  UnsignedIntegerProperty m_infoLevelDecisionDebug;

  /// default output info level for non-Decision selections (can be overruled by per selection setting)
  UnsignedIntegerProperty m_infoLevelNonDecision;
  UnsignedIntegerProperty m_infoLevelNonDecisionDebug;
  
  /// per selection info level
  SelectionSettingProp m_infoLevel;
  SelectionSettingProp m_infoLevelDebug;
 
  
};
#endif // HLTDECREPORTSMAKER_H
