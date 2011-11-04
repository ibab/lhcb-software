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
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SvcFactory.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ConversionSvc.h"

#include "Hit.h"

namespace Tests {

  IDataProviderSvc* dataSvc();

  class TmpConversionSvc : public ConversionSvc   {
  public:
    TmpConversionSvc(const std::string& nam, ISvcLocator* svcLoc)
      : ConversionSvc(nam, svcLoc, TEST_StorageType)   {
    }
    virtual ~TmpConversionSvc() {}
  };

  class IncidentAlgorithm : public Algorithm  {
  public:
    IncidentAlgorithm(const std::string& name, ISvcLocator *svcloc) 
      : Algorithm(name, svcloc) {
    }
    virtual ~IncidentAlgorithm() {
    }
    virtual StatusCode execute()  {
      MsgStream log(msgSvc(), name());
      log << MSG::ALWAYS << "Data-On-Demand trigger running for:"
	  << "/Event/2/21/211/2115/21x15/21xx15/21xxx15" << endmsg;
      StatusCode sc = dataSvc()->registerObject("/Event/2/21/211/2115/21x15/21xx15/21xxx15",new Hit());
      return sc;
    }
  };

}

using namespace Tests;

// Factory for instantiation of service objects
DECLARE_ALGORITHM_FACTORY(IncidentAlgorithm)
  DECLARE_SERVICE_FACTORY(TmpConversionSvc)
  DECLARE_SERVICE_FACTORY(DataSvc)
