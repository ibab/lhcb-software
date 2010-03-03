//	====================================================================
//  DstCollection.Write.h
//	--------------------------------------------------------------------
//
//	Package   : GaudiExamples/DstCollection
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef EVTCOLLECTION_WRITE_H
#define EVTCOLLECTION_WRITE_H

// Framework include files
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Algorithm.h"  // Required for inheritance

// Forward declarations
class INtupleSvc;

/** User example objects: DstCollectionWrite

    Description:
    A small class creating an event collection.

    Author:  M.Frank
    Version: 1.0
*/
class DstCollectionWrite : public Algorithm {
  NTuple::Item<IOpaqueAddress*>   m_evtAddrColl;
  NTuple::Item<int>               m_numColl;
  NTuple::Item<float>             m_ptHat;
  INTupleSvc*                     m_evtTupleSvc;
public:
  /// Constructor: A constructor of this form must be provided.
  DstCollectionWrite(const std::string& name, ISvcLocator* pSvcLocator); 
  /// Standard Destructor
  virtual ~DstCollectionWrite();
  /// Initialize
  virtual StatusCode initialize();
  /// Finalize
  virtual StatusCode finalize() {
    return StatusCode::SUCCESS;
  }
  /// Event callback
  virtual StatusCode execute();
};

#endif // EVTCOLLECTION_WRITE_H
