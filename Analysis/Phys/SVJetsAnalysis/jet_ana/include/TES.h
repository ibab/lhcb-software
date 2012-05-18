#ifndef TES_h
#define TES_h

#include "ROOTOscar.h"
//#include "selvars.h"

class TES{

public:

TES(){debug = false;}
~TES(){}
bool debug;
   ParticlesVector stdjets, stdmuons;
   ParticlesVector fatjets;

   void ClearVariables();
   void release_particle_vec(std::vector<Particle*>&);
   void ClearVariables_noselection();

   void release_particle(Particle *);
   void DefVars();

};

#endif //TES_HPP_
