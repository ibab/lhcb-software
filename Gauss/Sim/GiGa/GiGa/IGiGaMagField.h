// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/GiGa/IGiGaMagField.h,v 1.5 2001-07-15 20:53:56 ibelyaev Exp $ 
#ifndef     GIGA_IGIGAMagField_H
#define     GIGA_IGIGAMagField_H 1 
//
#include <string>
//
#include "GaudiKernel/IInterface.h"
// from G4 
#include "G4MagneticField.hh"
//
#include "GiGa/IIDIGiGaMagField.h" 
// 

/** @class IGiGaMagField     IGiGaMagField.h GiGa/IGiGaMagField.h

    definition of (pseudu)abstract (pseudo)interface to factory for Geant 4 Magnetic field class

    @author Vanya Belyaev
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

#endif      GIGA_IGIGAMagField_H
