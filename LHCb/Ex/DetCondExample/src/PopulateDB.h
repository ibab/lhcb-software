//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/PopulateDB.h,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $
#ifndef DETCONDEXAMPLE_POPULATEDB_H
#define DETCONDEXAMPLE_POPULATEDB_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IConditionsDBCnvSvc;
class IConditionsDBGate;

///---------------------------------------------------------------------------
/** @class PopulateDB PopulateDB.h DetCondExample/PopulateDB.h

    Simple algorithm to populate the ConditionsDB.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class PopulateDB : public Algorithm {

 public:

  /// Constructor
  PopulateDB ( const std::string& name, ISvcLocator* pSvcLocator ); 
  
  // Algorithm standard methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:

  /// Store sample data
  StatusCode i_condDBStoreSampleData   ( );

  /// Dump sample data
  StatusCode i_condDBDumpSampleData    ( );

 private:

  /// Handle to the ConditionsDBCnvSvc (encode data to be written)
  IConditionsDBCnvSvc* m_conditionsDBCnvSvc;

  /// Handle to the ConditionsDBGate (write the data in the CondDB)
  IConditionsDBGate* m_conditionsDBGate;

};

#endif    // DETCONDEXAMPLE_POPULATEDB_H


