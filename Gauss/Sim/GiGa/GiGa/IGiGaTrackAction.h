// $Id: IGiGaTrackAction.h,v 1.8 2002-05-07 12:21:31 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef     GIGA_IGIGATrackACTION_H
#define     GIGA_IGIGATrackACTION_H 1 
// ============================================================================
// GiGa
#include "GiGa/IGiGaInterface.h"
#include "GiGa/IIDIGiGaTrackAction.h"
// Geant4 
#include "G4UserTrackingAction.hh"

/** @class IGiGaTrackAction IGiGaTrackAction.h GiGa/IGiGaTrackAction.h
 *
 *  Definition of "pseudo-abstract" pseudo-interface 
 *                          for GiGa Tracking Action
 *
 *  @author  Vanya Belyaev
 *   @date    21/02/2001
 */

class IGiGaTrackAction: 
  virtual public G4UserTrackingAction , 
  virtual public IGiGaInterface 
{

public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID ()  { return IID_IGiGaTrackAction ; }
  
protected:
  
  ///  virtual destructor   
  virtual ~IGiGaTrackAction(){};
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<  GIGA_IGIGATrackACTION_H
// ============================================================================
