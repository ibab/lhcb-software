#ifndef     GIGA_IGiGaStepActionFACTORY_H
#define     GIGA_IGiGaStepActionFACTORY_H 1


// 
#include "GaudiKernel/IFactory.h"

#include "GiGa/IIDIGiGaStepActionFactory.h"


class ISvcLocator;
class IGiGaStepAction;


/** @class IGiGaStepActionFactory IGiGaStepActionFactory.h GiGa/IGiGaStepActionFactory.h
    
    The IGiGaStepActionFactory is the interface used to create contrete instances of GiGa Stepping Action 
    
    @author Vanya Belyaev
    @author 23/01/2001
*/

class IGiGaStepActionFactory : virtual public IFactory 
{
  ///
 public:
  ///
  /// Retrieve interface ID
  static  const InterfaceID&  interfaceID () { return IID_IGiGaStepActionFactory; }
  ///
  /// Create an instance of a concrete GiGaStepAction
  virtual       IGiGaStepAction* instantiate ( const std::string& ,  ISvcLocator* ) const = 0;
  ///
  /// Access to the GiGaStepAction type.
  virtual const std::string&  stepActionType ()    const  = 0;
  ///
};

///
///

#endif   // GIGA_IGiGaStepActionFACTORY_H

