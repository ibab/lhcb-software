//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/DumpDetectorStore.h,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $
#ifndef DETCONDEXAMPLE_DUMPDETECTORSTORE_H
#define DETCONDEXAMPLE_DUMPDETECTORSTORE_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

///---------------------------------------------------------------------------
/** @class DumpDetectorStore DumpDetectorStore.h DetCondExample/DumpDetectorStore.h

    Simple algorithm to dump the detector data store.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class DumpDetectorStore : public Algorithm {

 public:

  /// Constructor
  DumpDetectorStore ( const std::string& name, ISvcLocator* pSvcLocator ); 
  
  // Algorithm standard methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
};

#endif    // DETCONDEXAMPLE_DUMPDETECTORSTORE_H



