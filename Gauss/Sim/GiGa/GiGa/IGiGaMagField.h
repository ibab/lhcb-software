/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef     GIGA_IGIGAMagField_H
#define     GIGA_IGIGAMagField_H 1 
/// ===========================================================================
//
#include <string>
//
#include "GaudiKernel/IInterface.h"
// from G4 
#include "G4MagneticField.hh"
//
#include "GiGa/IIDIGiGaMagField.h" 
// 

/** @class IGiGaMagField IGiGaMagField.h GiGa/IGiGaMagField.h
 *
 *  definition of (pseudo) abstract (pseudo) interface 
 *  for Geant 4 Magnetic field class
 *
 *  @author Vanya Belyaev
 */

class IGiGaMagField: virtual public G4MagneticField , 
                     virtual public IInterface 
{
  ///
public:
  /// Retrieve interface ID
  static const InterfaceID&  interfaceID() { return IID_IGiGaMagField ; }
  ///
  virtual const std::string& name       () const = 0 ;
  virtual StatusCode         initialize ()       = 0 ;
  virtual StatusCode         finalize   ()       = 0 ;
  ///
  virtual ~IGiGaMagField(){};
  ///
};
///

/// ===========================================================================
#endif   ///< GIGA_IGIGAMagField_H
/// ===========================================================================
