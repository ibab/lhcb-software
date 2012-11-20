/*
  powheg.h is part of the Powheg++ interface library.
  Copyright (C) 2011 Philip Ilten
  This code links with POWHEG-BOX routines and is based on LHAupFortran.h 
  from the PYTHIA source code. PYTHIA is licensed under the GNU GPL version 2.
  This code is also licensed under the GNU GPL version 2.
*/

#ifndef Powheg_Powheg_H
#define Powheg_Powheg_H

// STL.
#include <fstream>

// Pythia.
#include "Pythia.h"
#include "PythiaStdlib.h"

// POWHEG-BOX matrix elements.
#include "dijet.h"
#include "lhauppowheg.h"
#include "gg_h.h"
#include "hvq.h"
#include "st_sch.h"
#include "st_tch.h"
#include "vbf_h.h"
#include "vbf_wp_wp.h"
#include "w.h"
#include "wbb.h"
#include "wj.h"
#include "ww.h"
#include "wz.h"
#include "z.h"
#include "zj.h"
#include "zz.h"       

namespace Powheg {
  class Powheg {
    
  public:
    // Constructor and destructor.
    Powheg();
    ~Powheg();

    // Read a configuration string.
    void readString(string setting);
    
    // Read a configuration file.
    bool readFile(string name);
    
    // Initialize the POWHEG configuration file.
    void init();

    // Remove the POWHEG-BOX run files.
    void cleanRun();

    // Pointer to the matrix element to use.
    LHAupPowheg *matrixElement;

  protected:

    // Vector of settings to pass to POWHEG.
    vector<string> settings;

    // Name of the process requested.
    string process;

    // Name of the PDF set file to use.
    string pdf;

    // Pointers to matrix elements available from POWHEG libraries.
    vector<LHAupPowheg*> matrixElements;
  };
}

#endif // Powheg_Powheg_H
