// $Id: IGiGaEventAction.h,v 1.6 2002-05-07 12:21:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef     GIGA_IGIGAEventACTION_H
#define     GIGA_IGIGAEventACTION_H 1 
// ============================================================================
// base classes from GiGa
#include "GiGa/IGiGaInterface.h"
// unique IID 
#include "GiGa/IIDIGiGaEventAction.h"
// G4 
#include "G4UserEventAction.hh"

/** @interface IGiGaEventAction IGiGaEventAction.h "GiGa/IGiGaEventAction.h"
 *
 *  Definition of "pseudo-abstract" pseudo-interface for GiGa Event Action
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    17/03/2001
 */

class IGiGaEventAction: 
  virtual public G4UserEventAction ,  
  virtual public IGiGaInterface  
{
public:
  
  /// Retrieve the unique interface ID
  static const InterfaceID& interfaceID ()  { return IID_IGiGaEventAction ; }
  
protected:
  
  ///  virtual (protected) destructor   
  virtual ~IGiGaEventAction(){};
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<  GIGA_IGIGAEventACTION_H
// ============================================================================
