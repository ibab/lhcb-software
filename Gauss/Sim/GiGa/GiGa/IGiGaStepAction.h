// $Id: IGiGaStepAction.h,v 1.9 2004-02-20 18:58:18 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2002/05/07 12:21:31  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef     GIGA_IGIGAStepACTION_H
#define     GIGA_IGIGAStepACTION_H 1 
// ============================================================================
// GiGa 
#include "GiGa/IGiGaInterface.h"
// Geant4 
#include "G4UserSteppingAction.hh"

/** @class IGiGaStepAction IGiGaStepAction.h GiGa/IGiGaStepAction.h
 *
 *  Definition of "pseudo-abstract" pseudo-interface 
 *                          for GiGa Stepping Action
 *
 *  @author  Vanya Belyaev
 *  @date    21/02/2001
 */

class IGiGaStepAction: 
  virtual public G4UserSteppingAction ,
  virtual public IGiGaInterface 
{
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID () ;
  
protected:
  
  ///  virtual destructor   
  virtual ~IGiGaStepAction() ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<  GIGA_IGIGAStepACTION_H
// ============================================================================
