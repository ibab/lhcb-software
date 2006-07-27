// $Id: IGiGaSensDet.h,v 1.7 2006-07-27 09:34:18 gcorti Exp $ 
#ifndef GIGA_IGIGASENSDET_H
#define GIGA_IGIGASENSDET_H 1 

// Include files
// from GiGa
#include "GiGa/IGiGaInterface.h"

// base class from Geant4 
#include "G4VSensitiveDetector.hh" 

/** @class IGiGaSensDet IGiGaSensDet.h GiGa/IGiGaSensDet.h
 *
 *  Definition of pseudo-abstract pseudo-interface for GiGa Sensitive Detector 
 *
 *  @author  Vanya Belyaev 
 *  @author  G. Corti
 *  @date    2001-01-23
 *  @date    2006-07-14 (revised)
 */

class IGiGaSensDet: public virtual G4VSensitiveDetector,
                    public virtual IGiGaInterface       
{
public:
  
  /** Retrieve the unique interface ID (static)
   *  @see IInterface
   */
  static const InterfaceID& interfaceID();

  /** Method for being a member of a GiGaSensDetSequence
   *  Implemented by base class, does not need reimplementation!
   */ 
  virtual bool processStep( G4Step* step, G4TouchableHistory* history ) = 0;  
  
protected:
    
  virtual ~IGiGaSensDet(); ///< virtual destructor 
  IGiGaSensDet() ;         ///< default constructor  
  
};

#endif  // GIGA_IGIGASENSDET_H
