// LHAFortran.h is a part of the PYTHIA event generator.
// Copyright (C) 2011 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Header file for Fortran Les Houches Accord user process information.
// LHAupFortran: derived class with the HEPRUP and HEPEUP Fortran info.
// You are expected to supply the fillHepRup and fillHepEup methods.

#ifndef Powheg_vbf_wp_wp_H
#define Powheg_vbf_wp_wp_H

#include "Pythia.h"
#include "PythiaStdlib.h"
#include "lhauppowheg.h"

namespace Powheg {
  
  // Class derived from LHAup. 
  class LHAupPowheg_vbf_wp_wp : public LHAupPowheg {
    
  public:
    
    // Constructor.
    LHAupPowheg_vbf_wp_wp();

    // Destructor.
    ~LHAupPowheg_vbf_wp_wp();
    
    // Routine for doing the job of setting initialization info.  
    bool setInit();
    
    // Routine for doing the job of setting info on next event.  
    bool setEvent(int idProcIn);
    
    // Routine for returning the library name.
    string libraryName();
    
  private:
    
    // User-written routine that does the intialization and fills heprup.
    bool fillHepRup();
    
    // User-written routine that does the event generation and fills hepeup.
    bool fillHepEup();
  };
  
} // End namespace Powheg.

#endif // Powheg_vbf_wp_wp_H
