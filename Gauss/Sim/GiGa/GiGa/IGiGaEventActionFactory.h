// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/GiGa/IGiGaEventActionFactory.h,v 1.3 2001-07-15 20:53:56 ibelyaev Exp $ 
#ifndef     GIGA_IGiGaEventActionFACTORY_H
#define     GIGA_IGiGaEventActionFACTORY_H 1


// 
#include "GaudiKernel/IFactory.h"

#include "GiGa/IIDIGiGaEventActionFactory.h"


class ISvcLocator;
class IGiGaEventAction;


/** @class IGiGaEventActionFactory IGiGaEventActionFactory.h GiGa/IGiGaEventActionFactory.h
    
    The IGiGaEventActionFactory is the interface used to create contrete instances of GiGa event Action 
    
    @author Vanya Belyaev
    @author 17/03/2001
*/

class IGiGaEventActionFactory : virtual public IFactory 
{
  ///
 public:
  ///
  /// Retrieve interface ID
  static  const InterfaceID&  interfaceID () { return IID_IGiGaEventActionFactory; }
  ///
  /// Create an instance of a concrete GiGaeventAction
  virtual       IGiGaEventAction* instantiate ( const std::string& ,  ISvcLocator* ) const = 0;
  ///
  /// Access to the GiGaeventAction type.
  virtual const std::string&  eventActionType ()    const  = 0;
  ///
};

///
///

#endif   // GIGA_IGiGaEventActionFACTORY_H

