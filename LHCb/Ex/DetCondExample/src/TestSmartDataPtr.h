//$Id: TestSmartDataPtr.h,v 1.3 2001-11-29 11:00:48 andreav Exp $
#ifndef DETCONDEXAMPLE_TESTSMARTDATAPTR_H
#define DETCONDEXAMPLE_TESTSMARTDATAPTR_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IDetDataSvc;

///---------------------------------------------------------------------------
/** @class TestSmartDataPtr TestSmartDataPtr.h DetCondExample/TestSmartDataPtr.h

    Example of an algorithm testing SmartDataPtr's to ConditionData

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class TestSmartDataPtr : public Algorithm {

 public:

  /// Constructor
  TestSmartDataPtr ( const std::string& name, ISvcLocator* pSvcLocator ); 
  
  // Algorithm standard methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:

  /// IDetDataSvc interface of the detector data service
  IDetDataSvc*          m_detDataSvc;

};

#endif    // DETCONDEXAMPLE_TESTSMARTDATAPTR_H



