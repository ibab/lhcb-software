#ifndef GAUDIROOTCNV_ROOTUTILS_H
#define GAUDIROOTCNV_ROOTUTILS_H

// Framework include files
//#include "GaudiKernel/DataObject.h"

// Forward declarations
class DataObject;

// Functions imported from GaudiKernel
namespace GaudiRoot {
  void popCurrentDataObject();
  void pushCurrentDataObject(DataObject** pobjAddr);
}

namespace {
  /// Release an interface pointer
  template<class T> inline void releasePtr(T*& p)  {
    if ( 0 != p )    {
      p->release();
      p = 0;
    }
  }
  /// Delete ponter and set variable to NULL
  template<class T> inline void deletePtr(T*& p)  {
    if ( 0 != p )    {
      delete p;
      p = 0;
    }
  }

  /** @struct  DataObjectPush RootUtils.h src/RootUtils.h
   *
   * Helper structure to push data object onto execution stack.
   * This way the execution context for SmartRef objects is set
   * and Gaudi references can easily be initialized.
   *
   * @author  M.Frank
   * @version 1.0
   */
  struct DataObjectPush {
    /// Reference to DataObject
    DataObject* m_p;
    /// Initializing constructor
    DataObjectPush(DataObject* p):m_p(p){GaudiRoot::pushCurrentDataObject(&m_p);}
    /// Stnadard destructor. Note: NOT virtual, hence no inheritance!
    ~DataObjectPush() { GaudiRoot::popCurrentDataObject();  }
  };
}

#endif // GAUDIROOTCNV_ROOTUTILS_H
