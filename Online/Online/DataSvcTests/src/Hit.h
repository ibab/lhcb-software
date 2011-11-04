//====================================================================
//	Hit.h
//--------------------------------------------------------------------
//
//	Package    : PersistencySvc/Test_1 (The LHCb PersistencySvc service)
//
//  Description: Definition of example data object
//
//	Author     : M.Frank
//  Created    : 13/1/99
//
//====================================================================
#ifndef DATASVC_HIT_H
#define DATASVC_HIT_H 1

// Framework includes
#include "GaudiKernel/DataObject.h"
// External declarations
static const CLID& CLID_Hit = 1;

/** @name The Hit class.
 
  Example data object
 
  @author Markus Frank
 */
class Hit : public DataObject   {
  /// data members
  unsigned int m_counts, m_value;
public:
  /// Standard Cosntructor
  Hit();
  /// Standard Destructor
  virtual ~Hit();
  /// Update data memebrs
  void set(unsigned int val, unsigned int cnt)  {
    m_counts = cnt;
    m_value = val;
  }
  /// Data Accessor
  unsigned int adcCount()  const    {
    return m_counts;
  }
  /// Data Accessor
  unsigned int adcAddress()   const {
    return m_value;
  }
  /// Retrieve Pointer to class defininition structure
  virtual const CLID& clID() const    {
    return CLID_Hit;
  }
  static long refCount();
};
#endif // PERSISTENCYSVC_HIT_H
