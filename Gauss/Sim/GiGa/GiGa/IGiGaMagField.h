// $Id: IGiGaMagField.h,v 1.8 2002-05-07 12:21:30 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// ===========================================================================
#ifndef     GIGA_IGIGAMagField_H
#define     GIGA_IGIGAMagField_H 1 
// ===========================================================================
// GiGa 
#include "GiGa/IGiGaInterface.h" 
#include "GiGa/IIDIGiGaMagField.h" 
// from G4 
#include "G4MagneticField.hh"
 

/** @interface IGiGaMagField IGiGaMagField.h "GiGa/IGiGaMagField.h"
 *
 *  definition of (pseudo) abstract (pseudo) interface 
 *  for Geant 4 Magnetic field class
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

class IGiGaMagField: 
  virtual public G4MagneticField , 
  virtual public IAlgTool 
{
public:
  
  /// Retrieve the unique interface ID
  static const InterfaceID&  interfaceID() { return IID_IGiGaMagField ; }
  
protected:
  
  /// destructor 
  virtual ~IGiGaMagField(){};
};
// ===========================================================================

// ===========================================================================
// The END 
// ===========================================================================
#endif   ///< GIGA_IGIGAMagField_H
// ===========================================================================
