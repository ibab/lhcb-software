// $Id: EventAccounting.h,v 1.1 2009-11-11 09:25:39 rlambert Exp $
#ifndef EVENTACCOUNTING_H 
#define EVENTACCOUNTING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// event model
#include "Event/EventCountFSR.h"


/** @class EventAccounting EventAccounting.h
 *  
 *
 *  @author Rob Lambert
 *  @date   2009-11-11
 *
 *  EventAccounting fills an EventCountFSR.
 *         This alg should be run by default in Brunel and DaVinci
 *         The EventCountFSR holds information on how many events should be in the file
 *         and how many were read from the input files used.
 *         A flag is also set in case the number stored is known to be wrong.
 *   -<c>"OverrideStatus"</c>: Override the status by the DefaultStatus in the FSR at the end of the job
 *   -<c>"DefaultStatus"</c>:  The status to start with if nothing else is known
 *
 */
class EventAccounting : public GaudiAlgorithm 
  , public virtual IIncidentListener {
public: 
  /// Standard constructor
  EventAccounting( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~EventAccounting( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
  // ==========================================================================
  // IIncindentListener interface
  // ==========================================================================
  virtual void handle ( const Incident& ) ;
  // ==========================================================================

  
protected:
  /// Reference to file records data service
  IDataProviderSvc* m_fileRecordSvc;

  //std::string m_DataName;             // input location of summary data
  std::string m_FSRName;              // output location of summary data in FSR

  LHCb::EventCountFSR* m_eventFSR;    // FSR for current file

  Counter     m_count_files;        // a map of string to int for filenames
  int         m_count_events;       // number of events seen
  int         m_count_output;       // number of incidents seen

  
  bool m_overrideStatus; ///override status at end of job with default. Set by OverrideStatus.
  std::string m_defaultStatusStr; ///status to start with if nothing else is known, Set by DefaultStatus
  LHCb::EventCountFSR::StatusFlag m_defaultStatus; ///status to start with if nothing else is known, cast from DefaultStatus
  
private:
  mutable IIncidentSvc* m_incSvc ;                      // the incident service 

};
#endif // EVENTACCOUNTING_H
