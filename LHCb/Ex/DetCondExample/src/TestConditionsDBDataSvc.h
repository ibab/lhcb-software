//$Id: TestConditionsDBDataSvc.h,v 1.3 2001-11-29 11:04:32 andreav Exp $
#ifndef DETCONDEXAMPLE_TESTCONDITIONSDBDATASVC_H
#define DETCONDEXAMPLE_TESTCONDITIONSDBDATASVC_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IDetDataSvc;

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

  /// Create a valid DataObject by folder name (for default tag and key),
  /// then load it in the TDS using the implicit naming convention:
  /// if the DataObject exists already, update it instead (if necessary).
  StatusCode i_retrieveValidCondition  ( DataObject*&         refpObject,
					 const std::string&   folderName );
  
 private:

  /// IDetDataSvc interface of the detector data service
  IDetDataSvc*          m_detDataSvc;

};

#endif    // DETCONDEXAMPLE_TESTCONDITIONSDBDATASVC_H


