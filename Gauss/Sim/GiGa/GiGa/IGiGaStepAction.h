#ifndef     GIGA_IGIGAStepACTION_H
#define     GIGA_IGIGAStepACTION_H 1 


#include <string> 

/// base classes from GaudiKernel  
#include "GaudiKernel/IInterface.h"

/// base class from Geant4 
#include "G4UserSteppingAction.hh"

/// unique IID 
#include "GiGa/IIDIGiGaStepAction.h"


/** @class IGiGaStepAction IGiGaStepAction.h GiGa/IGiGaStepAction.h

    Definition of "pseudo-abstract" pseudo-interface for GiGa Stepping Action

    @author  Vanya Belyaev
    @date    21/02/2001
*/

class IGiGaStepAction: virtual public G4UserSteppingAction ,  /* "the main" class from Geant4      */ 
		       virtual public IInterface              /* to allow easy Gaudi instantiation */
{
  ///
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID ()  { return IID_IGiGaStepAction ; }
  ///
  /// identification 
  virtual const std::string& name        () const = 0 ;
  /// initialization method 
  virtual StatusCode         initialize  ()       = 0 ;
  /// finalization method 
  virtual StatusCode         finalize    ()       = 0 ;
  ///
  ///  virtual destructor   
  virtual ~IGiGaStepAction(){};
  ///
};


#endif  //  GIGA_IGIGAStepACTION_H
