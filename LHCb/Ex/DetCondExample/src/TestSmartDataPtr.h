//$Id: TestSmartDataPtr.h,v 1.2 2001-11-27 18:31:02 andreav Exp $
#ifndef DETCONDEXAMPLE_TESTSMARTDATAPTR_H
#define DETCONDEXAMPLE_TESTSMARTDATAPTR_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IDetDataSvc;
class IConditionsDBDataSvc;

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

  /// IConditionsDBDataSvc interface of the detector data service
  IConditionsDBDataSvc* m_conditionsDBDataSvc;

};

#endif    // DETCONDEXAMPLE_TESTSMARTDATAPTR_H


