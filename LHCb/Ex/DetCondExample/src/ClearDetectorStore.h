//$Id: ClearDetectorStore.h,v 1.2 2001-11-30 09:35:48 andreav Exp $
#ifndef DETCONDEXAMPLE_CLEARDETECTORSTORE_H
#define DETCONDEXAMPLE_CLEARDETECTORSTORE_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

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
  
};

#endif    // DETCONDEXAMPLE_CLEARDETECTORSTORE_H


