/*
  lhauppowheg.h is part of the Powheg++ interface library.
  Copyright (C) 2011 Philip Ilten
  This code links with POWHEG-BOX routines and is based on LHAupFortran.h 
  from the PYTHIA source code. PYTHIA is licensed under the GNU GPL version 2.
  This code is also licensed under the GNU GPL version 2.
*/

#ifndef Powheg_LHAupPowheg_H
#define Powheg_LHAupPowheg_H

#include <cstring>
#include "Pythia.h"
#include "PythiaStdlib.h"

namespace Powheg {
  
  // Class derived from LHAup. 
  class LHAupPowheg : public Pythia8::LHAup {
    
  public:
    
    // Constructor.
    LHAupPowheg() {};

    // Destructor.
    ~LHAupPowheg() {};
    
    // Routine for doing the job of setting initialization info.  
    virtual bool setInit() {return false;}
    
    // Routine for doing the job of setting info on next event.  
    virtual bool setEvent(int idProcIn) {return false;}
    
    // Routine for returning the library name.
    virtual string libraryName() {return "none";}
    
  protected:
    
    // Save beam energies to calculate x values.
    double eBeamA, eBeamB;
    
    // User-written routine that does the intialization and fills heprup.
    virtual bool fillHepRup() {return false;}
    
    // User-written routine that does the event generation and fills hepeup.
    virtual bool fillHepEup() {return false;}
  };
  
} // End namespace Powheg.

#endif // Powheg_LHAupPowheg_H
