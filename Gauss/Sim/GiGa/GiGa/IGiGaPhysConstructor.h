// $Id: IGiGaPhysConstructor.h,v 1.3 2003-04-06 18:49:46 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef        GIGA_IGiGaPhysConstructor_H
#define        GIGA_IGiGaPhysConstructor_H 1 
// ============================================================================
// GiGa 
#include "GiGa/IGiGaPhysicsConstructor.h"
#include "GiGa/IIDIGiGaPhysConstructor.h"
// Geant4 
#include "G4VPhysicsConstructor.hh"

/** @class IGiGaPhysConstructor GiGa/IGiGaPhysConstructor.h
 *
 *  @author Witek Pokorski Witold.Pokorski@cern.ch
 */

class IGiGaPhysConstructor: 
  public virtual   G4VPhysicsConstructor ,
  public virtual IGiGaPhysicsConstructor  
{
public:
  
  /// unique interface identification
  static const InterfaceID& interfaceID() { return IID_IGiGaPhysConstructor; }
  
protected:
  
  /// virtual destructor 
  virtual ~IGiGaPhysConstructor(){};
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif   ///<   GIGA_IGiGaPhysConstructor_H
// ============================================================================
