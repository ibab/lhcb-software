//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/TestConditionsDBDataSvc.h,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $
#ifndef DETCONDEXAMPLE_TESTCONDITIONSDBDATASVC_H
#define DETCONDEXAMPLE_TESTCONDITIONSDBDATASVC_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IConditionDataSvc;
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

  /// IConditionDataSvc interface of the detector data service
  IConditionDataSvc*    m_conditionDataSvc;

  /// IConditionsDBDataSvc interface of the detector data service
  IConditionsDBDataSvc* m_conditionsDBDataSvc;

};

#endif    // DETCONDEXAMPLE_TESTCONDITIONSDBDATASVC_H


