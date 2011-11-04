//====================================================================
//	Hit.cpp
//--------------------------------------------------------------------
//
//	Package    : DataSvc/Test_1 (The LHCb PersistencySvc service)
//
//  Description: Implementation of example data object
//
//	Author     : M.Frank
//  Created    : 13/1/99
//
//====================================================================
#define DATASVC_HIT_CPP 1
// C/C++ include files
#include <iostream>

// Framework include files
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"

// Test includes
#include "Factories.h"
#include "Hit.h"

static long hitCount = 0;

/// Standard Cosntructor
Hit::Hit()
: DataObject(), m_counts(0), m_value(0)
{
  hitCount++;
}

/// Standard Destructor
Hit::~Hit()  {
  hitCount--;
  /*
  if ( 0 != directory() )   {
    std::cout << " ~Hit:" << directory()->name() << " (" << this << ") .. ";
  }
  else  {
    std::cout << " ~Hit:" << this << " .. ";
  }
*/
}

long Hit::refCount() {
  return hitCount;
}

/** @name The TmpHitConverter class.
  Dummy converter to just pop a hit from the heap 
  @author Markus Frank
 */
class TmpHitConverter : public Converter     {
private:
  /// Steer printout
  bool m_print;
  long m_create;
public:
  /// Standard Constructor
  TmpHitConverter(ISvcLocator* loc = 0, bool prt = true)
    : Converter(TEST_StorageType,CLID_Hit,loc), m_print(prt)
  {
    m_create = 0;
  }
  /// Standard Destructor
  virtual ~TmpHitConverter()   {
    if ( m_print )    {
      std::cout << "... ~HitConverter deleted: HitCount=" << Hit::refCount() << std::endl;
    }
  }

  virtual long repSvcType() const   {
    return i_repSvcType();
  }
  StatusCode finalize()   {
    std::cout << "FINALIZE: HitConverter created " << m_create << " Objects in total" << std::endl;
    return StatusCode::SUCCESS;
  }
  /// Create the transient representation of an object.
  StatusCode createObj(IOpaqueAddress* pAddress, DataObject*& refpObject)    {
    refpObject = new Hit();
    m_create++;
    if ( m_print )    {
      std::string name = "???";
      IRegistry* pDir = pAddress->registry();
      if ( pDir != 0 )    {
        name = pDir->identifier();
      }
      std::cout << "... HitConverter Created new Hit: Info=" << name << std::endl;
    }
    return refpObject == 0 ? StatusCode::FAILURE : StatusCode::SUCCESS;
  }
  static const CLID& classID()   {
    return CLID_Hit;
  }
  static unsigned char storageType()    {
    return TEST_StorageType;
  }
};

/// TmpHit factory instantiation
MAKE_CONVERTER_FACTORY(TmpHitConverter)
