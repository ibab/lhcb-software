#ifndef RICHMARKOV_SEED_CLHEP_STATIC_ENGINE_H
#define RICHMARKOV_SEED_CLHEP_STATIC_ENGINE_H

namespace RichMarkov {
  void seedCLHEPStaticEngine(); // FwdDec
}

#include "urandom/jamesRandomSeed.h"
#include "CLHEP/Random/Random.h"
#include "CLHEP/Random/JamesRandom.h"
#include <typeinfo>
#include <string>

namespace RichMarkov {

  void seedCLHEPStaticEngine() {
    try {
      HepJamesRandom & engine
        = dynamic_cast<HepJamesRandom &>(*HepRandom::getTheEngine());
      // If we got this far, the static engine WAS a jamesRandom, so:
      engine.setSeed(jamesRandomSeed());
    }
    catch (std::bad_cast) {
      // Oh dear! The CLHEP standard changed, and the static engine is no
      // longer a JamesRandom :(
      throw FinderExternalException(std::string("Oh dear! The CLHEP standard changed, and the static engine is no longer a JamesRandom :("));
    };
  };

}

#endif
