//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/DumpDetectorStore.h,v 1.3 2006-08-31 13:53:44 marcocle Exp $
#ifndef DETCONDEXAMPLE_DUMPDETECTORSTORE_H
#define DETCONDEXAMPLE_DUMPDETECTORSTORE_H 1

// Base class
#include "GaudiAlg/GaudiAlgorithm.h"

///---------------------------------------------------------------------------
/** @class DumpDetectorStore DumpDetectorStore.h DetCondExample/DumpDetectorStore.h

    Simple algorithm to dump the detector data store.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class DumpDetectorStore : public GaudiAlgorithm {

 public:

  /// Constructor
  DumpDetectorStore ( const std::string& name, ISvcLocator* pSvcLocator ); 
  
  /// Only finalize is implemented (GaudiAlgorithm::initialize() is enough).
  StatusCode execute();
  StatusCode finalize();

private:
  bool m_dumpConds;
  
};

#endif    // DETCONDEXAMPLE_DUMPDETECTORSTORE_H



