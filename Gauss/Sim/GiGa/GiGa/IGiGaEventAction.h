// $Id: IGiGaEventAction.h,v 1.7 2004-02-20 18:58:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2002/05/07 12:21:30  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef     GIGA_IGIGAEventACTION_H
#define     GIGA_IGIGAEventACTION_H 1 
// ============================================================================
// base classes from GiGa
#include "GiGa/IGiGaInterface.h"
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
  static const InterfaceID& interfaceID () ;
  
protected:
  
  ///  virtual (protected) destructor   
  virtual ~IGiGaEventAction();
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<  GIGA_IGIGAEventACTION_H
// ============================================================================
