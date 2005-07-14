//$Id: DAQCondDBAlgorithm.h,v 1.5 2005-07-14 15:10:22 marcocle Exp $
#ifndef DETCONDEXAMPLE_DAQCONDDBALGORITHM_H
#define DETCONDEXAMPLE_DAQCONDDBALGORITHM_H 1

// Base class
#include "GaudiAlg/GaudiAlgorithm.h"

// Forward declarations
class ICondDBAccessSvc;
namespace pool
{
  class AttributeListSpecification;
}

///---------------------------------------------------------------------------
/** @class DAQCondDBAlgorithm DAQCondDBAlgorithm.h DetCondExample/DAQCondDBAlgorithm.h

    Simple algorithm emulating a DAQ task to populate the ConditionsDB.
    This can be useful to measure the I/O rate for writing in the database.

    @author Andrea Valassi 
    @date July 2002
*///--------------------------------------------------------------------------

class DAQCondDBAlgorithm : public GaudiAlgorithm {

 public:

  /// Constructor
  DAQCondDBAlgorithm ( const std::string& name, ISvcLocator* pSvcLocator ); 
  
  // Algorithm standard methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:

  /// DAQ CondDB relative folder name (from the job options)
  std::string m_daqFolderName;

  /// DAQ CondDB record size (from the job options)
  long m_daqRecordSize;

  /// DAQ CondDB record (built only once during the initialization)
  std::string m_daqRecord;

  /// Show event by event progress ( every n events if n>0 )
  long m_daqShowProgress;

  /// Number of events analysed so far
  long m_daqEventNumber;

  /// System time at the end of initialization 
  longlong m_nsInitialized;

  /// Total time (ns) spent in event processing
  longlong m_nsExec;

  /// Total time (ns) spent in writing to the DB
  longlong m_nsDBIO;

  /// Handle to the CondDBAccessSvc (retrieved during the initialization)
  ICondDBAccessSvc* m_dbAccSvc;

  /// Specification for the payload to sore
  pool::AttributeListSpecification* m_payloadSpec;

};

#endif    // DETCONDEXAMPLE_DAQCONDDBALGORITHM_H


