//====================================================================
//	PersistencySvcTest test class functionality
//--------------------------------------------------------------------
//
//	Package    : PersistencySvcTest ( The LHCb Offline System)
//
//  Description: Test the PersistencySvc functionality
//
//	Author     : M.Frank
//  History    :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who     
// +---------+----------------------------------------------+---------
// | 11/2/99 | Initial version                              | MF
// +---------+----------------------------------------------+---------
//
//====================================================================
#ifndef PERSISTENCYSVC_PERSISTENCYSVCTEST_H
#define PERSISTENCYSVC_PERSISTENCYSVCTEST_H

// Interface/Framework include files
#include "GaudiKernel/IPersistencySvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IService.h"
// Base class
#include "DataSvcTests/DataSvcTest.h"

class ICnvFactory;

/** Definition of a small class to test the persistency service.
  This test is based on the data service test.

  @author M.Frank
  @date   11/2/99
    
*/
class PersistencySvcTest    : public DataSvcTest    {
public:
  /// Pointer to conversion service interface
  IConversionSvc*   m_pICV;
  /// Pointer to the instantiated service interface
  IService*         m_pISvcCnv;
public:
  /// Standtad constructor
  PersistencySvcTest(const std::string& nam, bool dbg);
  /// standard destructor
  virtual ~PersistencySvcTest();
  /// Shutdown the test...
  virtual void shutdown();
  /// Action method
  virtual void run();
  /// Add converter to a conversion service
  StatusCode addConverter(const ICnvFactory& fac);
  /// Set Data loader object
  StatusCode testDataLoader(IConversionSvc* loader);
};
#endif // PERSISTENCYSVC_PERSISTENCYSVCTEST_H
