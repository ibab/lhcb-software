// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/GiGa/IGiGaStackActionFactory.h,v 1.4 2001-07-15 20:53:57 ibelyaev Exp $ 
#ifndef     GIGA_IGiGaStackActionFACTORY_H
#define     GIGA_IGiGaStackActionFACTORY_H 1
// GauidKernel
#include "GaudiKernel/IFactory.h"
// GiGa 
#include "GiGa/IIDIGiGaStackActionFactory.h"
// 
class ISvcLocator;
class IGiGaStackAction;


/** @class IGiGaStackActionFactory IGiGaStackActionFactory.h GiGa/IGiGaStackActionFactory.h
    
    The IGiGaStackActionFactory is the interface used to create contrete instances of GiGa Stack Action 
    
    @author Vanya Belyaev
    @author 23/01/2001
*/

class IGiGaStackActionFactory : virtual public IFactory 
{
  ///
 public:
  ///
  /// Retrieve interface ID
  static  const InterfaceID&  interfaceID () { return IID_IGiGaStackActionFactory; }
  ///
  /// Create an instance of a concrete GiGaStackAction
  virtual       IGiGaStackAction* instantiate ( const std::string& ,  ISvcLocator* ) const = 0;
  ///
  /// Access to the GiGaStackAction type.
  virtual const std::string&  stackActionType ()    const  = 0;
  ///
};
///

#endif   // GIGA_IGiGaStackActionFACTORY_H

