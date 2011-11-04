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
// Framework include files
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ConversionSvc.h"

class TmpConversionSvc : public ConversionSvc   {
public:
  TmpConversionSvc(const std::string& nam, ISvcLocator* svcLoc)
  : ConversionSvc(nam, svcLoc, TEST_StorageType)   {
  }
  virtual ~TmpConversionSvc() {}
};

// Factory for instantiation of service objects
DECLARE_SERVICE_FACTORY(TmpConversionSvc)
DECLARE_SERVICE_FACTORY(DataSvc)
