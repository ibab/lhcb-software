// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/GiGa/IGiGaTrackActionFactory.h,v 1.2 2001-03-18 14:56:13 ibelyaev Exp $ 
#ifndef     GIGA_IGiGaTrackActionFACTORY_H
#define     GIGA_IGiGaTrackActionFACTORY_H 1
// GaudiKernel
#include "GaudiKernel/IFactory.h"
// GiGa 
#include "GiGa/IIDIGiGaTrackActionFactory.h"
//
class ISvcLocator;
class IGiGaTrackAction;
//

/** @class IGiGaTrackActionFactory IGiGaTrackActionFactory.h GiGa/IGiGaTrackActionFactory.h
    
    The IGiGaTrackActionFactory is the interface used to create contrete instances of GiGa Track Action 
    
    @author Vanya Belyaev
    @author 23/01/2001
*/

class IGiGaTrackActionFactory : virtual public IFactory 
{
  ///
 public:
  ///
  /// Retrieve interface ID
  static  const InterfaceID&  interfaceID () { return IID_IGiGaTrackActionFactory; }
  ///
  /// Create an instance of a concrete GiGaTrackAction
  virtual       IGiGaTrackAction* instantiate ( const std::string& ,  ISvcLocator* ) const = 0;
  ///
  /// Access to the GiGaTrackAction type.
  virtual const std::string&  trackActionType ()    const  = 0;
  ///
};

///
///

#endif   // GIGA_IGiGaTrackActionFACTORY_H

