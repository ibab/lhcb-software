// $Id: IGiGaPhysList.h,v 1.7 2002-05-07 12:21:31 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef        GIGA_IGiGaPhysList_H
#define        GIGA_IGiGaPhysList_H 1 
// ============================================================================
// GiGa 
#include "GiGa/IGiGaInterface.h"
#include "GiGa/IIDIGiGaPhysList.h"
// Geant4 
#include "G4VUserPhysicsList.hh"

/** @class IGiGaPhysList IGiGaPhysList.h GiGa/IGiGaPhysList.h
 *
 *  definition of (pseudo)abstract (pseudo)interface 
 *    to Geant 4 Physics List class
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

class IGiGaPhysList: 
  virtual public G4VUserPhysicsList ,
  virtual public IGiGaInterface  
{
public:
  
  /// uniqie interafce identification
  static const InterfaceID& interfaceID() { return IID_IGiGaPhysList; }
  
protected:
  
  /// virtual destructor 
  virtual ~IGiGaPhysList(){};
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif   ///<   GIGA_IGiGaPhysList_H
// ============================================================================
