// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/GiGa/IGiGaSensDet.h,v 1.2 2001-03-18 16:04:27 ibelyaev Exp $ 
#ifndef    GIGA_IGiGaSensDet_H
#define    GIGA_IGiGaSensDet_H 1 
// from STL
#include <string> 
// base classes from GaudiKernel  
#include "GaudiKernel/IInterface.h"
// base class from Geant4 
#include "G4VSensitiveDetector.hh" 
// unique IID 
#include "GiGa/IIDIGiGaSensDet.h"
// 

/** @class IGiGaSensDet IGiGaSensDet.h GiGa/IGiGaSensDet.h

    Definition of pseudo-abstract pseudo-interface for GiGa Sensitive Detector 

    @author  Vanya Belyaev
    @date    23/01/2001
*/


class IGiGaSensDet: virtual public G4VSensitiveDetector,  /*  "the main" class from Geant4                   */ 
               virtual public IInterface             /*  to allow easy Gaudi instantiation              */
{
  ///
 public:
  ///
  /// Retrieve interface ID
  static const InterfaceID& interfaceID ()  { return IID_IGiGaSensDet ; }
  ///

  /// identification 
  virtual const std::string& name        () const = 0 ;
  /// initialization method 
  virtual StatusCode         initialize  ()       = 0 ;
  /// finalization method 
  virtual StatusCode         finalize    ()       = 0 ;
  ///
  ///  virtual destructor   
  virtual ~IGiGaSensDet(){};
  ///
 protected:
  ///
  IGiGaSensDet(): G4VSensitiveDetector( "NotYetDefined" ){} ; // no default constructor 
  ///
};

///
///
///




#endif  // GIGA_IGiGaSensDet_H













