//$Id: TestConditionsDBDataSvc.h,v 1.2 2001-11-27 18:31:30 andreav Exp $
#ifndef DETCONDEXAMPLE_TESTCONDITIONSDBDATASVC_H
#define DETCONDEXAMPLE_TESTCONDITIONSDBDATASVC_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IDetDataSvc;
class IConditionsDBDataSvc;

///---------------------------------------------------------------------------
/** @class TestConditionsDBDataSvc TestConditionsDBDataSvc.h DetCondExample/TestConditionsDBDataSvc.h

    Example of an algorithm testing the IConditionsDBDataSvc interface.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class TestConditionsDBDataSvc : public Algorithm {

 public:

  /// Constructor
  TestConditionsDBDataSvc ( const std::string& name, 
			    ISvcLocator* pSvcLocator ); 
  
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

#endif    // DETCONDEXAMPLE_TESTCONDITIONSDBDATASVC_H


