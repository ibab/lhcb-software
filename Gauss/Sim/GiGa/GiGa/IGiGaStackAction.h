// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/GiGa/IGiGaStackAction.h,v 1.3 2001-03-18 14:56:13 ibelyaev Exp $ 
#ifndef     GIGA_IGIGASTACKACTION_H
#define     GIGA_IGIGASTACKACTION_H 1 
// STL 
#include <string> 
// base classes from GaudiKernel  
#include "GaudiKernel/IInterface.h"
// base class from Geant4 
#include "G4UserStackingAction.hh"
// unique IID 
#include "GiGa/IIDIGiGaStackAction.h"



/** @class IGiGaStackAction IGiGaStackAction.h GiGa/IGiGaStackAction.h

    Definition of "pseudo-abstract" pseudo-interface for GiGa Stacking Action

    @author  Vanya Belyaev
    @date    21/02/2001
*/

class IGiGaStackAction: virtual public G4UserStackingAction ,  /* "the main" class from Geant4      */ 
                        virtual public IInterface              /* to allow easy Gaudi instantiation */
{
  ///
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID ()  { return IID_IGiGaStackAction ; }
  ///
  /// identification 
  virtual const std::string& name        () const = 0 ;
  /// initialization method 
  virtual StatusCode         initialize  ()       = 0 ;
  /// finalization method 
  virtual StatusCode         finalize    ()       = 0 ;
  ///
  ///  virtual destructor   
  virtual ~IGiGaStackAction(){};
  ///
};


#endif  //  GIGA_IGIGASTACKACTION_H
