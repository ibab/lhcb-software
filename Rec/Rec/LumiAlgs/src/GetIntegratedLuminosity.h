
#ifndef GETINTEGRATEDLUMINOSITY_H 
#define GETINTEGRATEDLUMINOSITY_H 1

// Include files
// from Gaudi

#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
/// Reference to run records data service
  
// event model
#include "Event/HltLumiSummary.h"
#include "Event/LumiFSR.h"
#include "Event/LumiIntegral.h"

// local
#include "LumiIntegrator.h"

/** @class GetIntegratedLuminosity GetIntegratedLuminosity.h
 *   
 *
 *  @author Yasmine Amhis
 *  @date   2010-05-26
 */
class GetIntegratedLuminosity : public GaudiTupleAlg , 
                                virtual public IIncidentListener {
public: 
  /// Standard constructor
  GetIntegratedLuminosity( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetIntegratedLuminosity( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  // ==========================================================================
  // IIncindentListener interface
  // ==========================================================================
  virtual void handle ( const Incident& ) {m_nHandled++; m_eventFSR->setOutput(m_nHandled);}
  // ==========================================================================
protected:
  int m_nHandled; ///<total events
  
  bool m_countersDetails;
  int m_count_input;  // number of events seen
  int m_count_output;  // number of incidents seen
 
  LHCb::EventCountFSR* m_eventFSR; // FSR for current file 
  
  
  std::string m_ToolName;                       // name of tool for normalization
  std::string m_EventCountFSRName;              // specific tag of event summary data in FSR
  
private:
  ILumiIntegrator *m_integratorTool;            // tool to integrate luminosity
  mutable IIncidentSvc* m_incSvc ;                      // the incident service 
};
#endif 
