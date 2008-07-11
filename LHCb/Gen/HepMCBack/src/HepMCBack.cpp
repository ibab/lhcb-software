// $Id: HepMCBack.cpp,v 1.1.1.1 2008-07-11 13:02:55 robbep Exp $
//----------------------------------------------------------------------------
// File for backward readibility of HepMC 1.26 when using HepMC 2.0
//----------------------------------------------------------------------------
// Modifs for HepMC 1.26
#include "TClass.h"
#include "TClassStreamer.h"
#include "Cintex/Cintex.h"

void GenParticle_Streamer(TBuffer &R__b, void *obj);
void GenVertex_Streamer(TBuffer &R__b, void *obj);

namespace {

  struct HepMCBack {

    HepMCBack() {
      
      ROOT::Cintex::Cintex::Enable();

      TClass *clazz = TClass::GetClass("HepMC::GenVertex");
      if (clazz) {
        clazz->AdoptStreamer(new TClassStreamer(GenVertex_Streamer));
      }
      else {
        std::cerr << "Warning: Cannot load dictionary for HepMC::GenVertex, "
                  << "you will not be able to read old HepMC 1.26 files."
                  << std::endl;
      }

      clazz = TClass::GetClass("HepMC::GenParticle");
      if (clazz) {
        clazz->AdoptStreamer(new TClassStreamer(GenParticle_Streamer));
      }
      else {
        std::cerr << "Warning: Cannot load dictionary for HepMC::GenParticle, "
                  << "you will not be able to read old HepMC 1.26 files."
                  << std::endl;
      }

    }
  };
  
}

static HepMCBack _HepMCBack;

    
