// $Id: IGiGaRunAction.h,v 1.4 2002-05-07 12:21:31 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGA_IGIGARUNACTION_H 
#define GIGA_IGIGARUNACTION_H 1
// ============================================================================
// GiGa 
#include "GiGa/IGiGaInterface.h"
#include "GiGa/IIDIGiGaRunAction.h"
// G4 
#include "G4UserRunAction.hh"

/** @class IGiGaRunAction IGiGaRunAction.h GiGa/IGiGaRunAction.h
 *  
 *  definition of presudo-abstract interface for 
 *  GiGa Run Action
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   26/07/2001
 */

class IGiGaRunAction: 
  virtual public G4UserRunAction ,  
  virtual public IGiGaInterface
{
  
public:
  
  /** retrieve the uniques interface ID
   *  @return uniqie interface ID 
   */
  static const InterfaceID& interfaceID ()  { return IID_IGiGaRunAction ; }
  
protected:
  
  ///  virtual destructor   
  virtual ~IGiGaRunAction(){};
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif ///< GIGA_IGIGARUNACTION_H
// ============================================================================
