// $Id: IGiGaStackAction.h,v 1.9 2004-02-20 18:58:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name:
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2002/05/07 12:21:31  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef     GIGA_IGIGASTACKACTION_H
#define     GIGA_IGIGASTACKACTION_H 1 
// ============================================================================
// GiGa
#include "GiGa/IGiGaInterface.h"
// Geant4 
#include "G4UserStackingAction.hh"

/** @class IGiGaStackAction IGiGaStackAction.h GiGa/IGiGaStackAction.h
 *
 *  Definition of "pseudo-abstract" pseudo-interface 
 *                           for GiGa Stacking Action
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    21/02/2001
 */

class IGiGaStackAction: 
  virtual public G4UserStackingAction ,
  virtual public IGiGaInterface   
{
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID () ;
  
protected:
  
  ///  virtual destructor   
  virtual ~IGiGaStackAction() ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<  GIGA_IGIGASTACKACTION_H
// ============================================================================
