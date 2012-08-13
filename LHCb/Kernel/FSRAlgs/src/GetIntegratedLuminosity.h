#ifndef GETINTEGRATEDLUMINOSITY_H 
#define GETINTEGRATEDLUMINOSITY_H 1

// Include files
// from Gaudi

#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/IDataManagerSvc.h"

  
// event model
#include "Event/HltLumiSummary.h"
#include "Event/LumiFSR.h"
#include "Event/LumiIntegral.h"
#include "Event/EventCountFSR.h"
#include "Event/TimeSpanFSR.h"

// local
#include "LumiIntegrator.h"

/** @class GetIntegratedLuminosity GetIntegratedLuminosity.h
 *
 *  Returns integrated luminosity
 *
 *  @author Yasmine Amhis
 *  @date   2010-05-26
 */
class GetIntegratedLuminosity : public GaudiTupleAlg  
                               {
public: 
  /// Standard constructor
  GetIntegratedLuminosity( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetIntegratedLuminosity( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
  // ==========================================================================
protected:
  
  virtual int check( );

  
  IDataProviderSvc* m_fileRecordSvc;
  std::string m_FileRecordName;                 // location of FileRecords
  std::string m_FSRName;                        // specific tag of summary data in FSR
  std::string m_defaultStatusStr; ///status to start with if nothing else is known, Set by DefaultStatus
  LHCb::EventCountFSR::StatusFlag m_defaultStatus; ///status to start with if nothing else is known, cast from DefaultStatus
  int         m_events_in_file;                 // events after OpenFileIncident
  bool m_countersDetails;
  int m_count_input;  // number of events seen
  int m_count_output;  // number of incidents seen
  //LHCb::EventCountFSR* m_eventFSR; // FSR for current file 
  
  
  std::string m_ToolName;                       // name of tool for normalization
  std::string m_EventCountFSRName;              // specific tag of event summary data in FSR
  
private:
  ILumiIntegrator *m_integratorTool;            // tool to integrate luminosity

};
#endif 
