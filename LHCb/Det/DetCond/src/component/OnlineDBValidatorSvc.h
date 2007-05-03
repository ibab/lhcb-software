// $Id: OnlineDBValidatorSvc.h,v 1.1 2007-05-03 17:28:28 marcocle Exp $
#ifndef COMPONENT_ONLINEDBVALIDATORSVC_H 
#define COMPONENT_ONLINEDBVALIDATORSVC_H 1

// Include files
#include "GaudiKernel/Service.h"

class Condition;

/** @class OnlineDBValidatorSvc
 *  
 *  Small service intended to check if the Online database has been correctly replicated.
 *  Since the replication of the online CondDB and the transfer of RAW data files are
 *  independent, it is possible that the file is processed with a non up-to-date CondDB.
 *  To synchronize the processing with the replication, a special condition (Marker) is
 *  added to the CondDB at regular intervals during the run, plus once at the end of run.
 *  If the IOV for the marker condition valid for the current event is finite (upper limit
 *  different from the maximum value), it means that the local copy of the CondDB is not
 *  enough recent because the last verifiable update is older than current event (i.e.
 *  the time corresponding to the lower limit of the marker condition).
 *  This service registers itself to the UpdateManagetrSvc as a user of the marker condition
 *  (which path is configurable with the option "MarkerPath") and check if the upper limit
 *  of its IOV if equal to the maximum, in which case prints an error message and returns
 *  a failure (from the callback function OnlineDBValidatorSvc::i_CheckMarker), which triggers
 *  a stop of the application.
 *
 *  @author Marco Clemencic
 *  @date   2007-05-03
 */
class OnlineDBValidatorSvc: public Service {
public: 
  /// Standard constructor
  OnlineDBValidatorSvc( const std::string& name, ISvcLocator* svcloc ); 

  virtual ~OnlineDBValidatorSvc( ); ///< Destructor

  /// Initilize
  virtual StatusCode initialize();

private:

  /// Function called by the UpdateManagerSvc
  StatusCode i_CheckMarker();

  /// Property OnlineDBValidatorSvc.MarkerPath: Path of the marker condition in the transient store.
  std::string m_markerPath;

  /// Pointer to the marker condition.
  Condition *m_markerCondition;

};
#endif // COMPONENT_ONLINEDBVALIDATORSVC_H
