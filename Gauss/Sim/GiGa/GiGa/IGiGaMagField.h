// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2001/07/23 13:11:45  ibelyaev
// the package restructurisation(II)
// 
// ===========================================================================
#ifndef     GIGA_IGIGAMagField_H
#define     GIGA_IGIGAMagField_H 1 
// ===========================================================================
// STD & STL 
#include <string>
// Gaudi Kernel
#include "GaudiKernel/IInterface.h"
// GiGa 
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

class IGiGaMagField: virtual public G4MagneticField , 
                     virtual public IInterface 
{
public:
  /// Retrieve the unique interface ID
  static const InterfaceID&  interfaceID() { return IID_IGiGaMagField ; }
  /// identification
  virtual const std::string& name       () const = 0 ;
  /// initialization 
  virtual StatusCode         initialize ()       = 0 ;
  /// finalization 
  virtual StatusCode         finalize   ()       = 0 ;
  /// destructor 
  virtual ~IGiGaMagField(){};
};

// ===========================================================================
#endif   ///< GIGA_IGIGAMagField_H
// ===========================================================================
