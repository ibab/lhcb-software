//$Id: BootDetectorStore.h,v 1.1 2001-11-29 13:16:04 andreav Exp $
#ifndef DETCONDEXAMPLE_BOOTDETECTORSTORE_H
#define DETCONDEXAMPLE_BOOTDETECTORSTORE_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

///---------------------------------------------------------------------------
/** @class BootDetectorStore BootDetectorStore.h DetCondExample/BootDetectorStore.h

    Simple algorithm to boot the detector data store.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class BootDetectorStore : public Algorithm {

 public:

  /// Constructor
  BootDetectorStore ( const std::string& name, ISvcLocator* pSvcLocator ); 
  
  // Algorithm standard methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
};

#endif    // DETCONDEXAMPLE_BOOTDETECTORSTORE_H



