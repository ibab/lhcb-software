//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/ClearDetectorStore.h,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $
#ifndef DETCONDEXAMPLE_CLEARDETECTORSTORE_H
#define DETCONDEXAMPLE_CLEARDETECTORSTORE_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IDataManagerSvc;

///---------------------------------------------------------------------------
/** @class ClearDetectorStore ClearDetectorStore.h DetCondExample/ClearDetectorStore.h

    Simple algorithm to clear the detector data store.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class ClearDetectorStore : public Algorithm {

 public:

  /// Constructor
  ClearDetectorStore ( const std::string& name, ISvcLocator* pSvcLocator ); 
  
  // Algorithm standard methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:

  /// IDataManagerSvc interface of the detector data service
  IDataManagerSvc* m_detDataMgrSvc;

};

#endif    // DETCONDEXAMPLE_CLEARDETECTORSTORE_H


