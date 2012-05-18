
#include "JetIDCut.h"
#define JetIDCut_h
#include "ROOTOscar.h"
bool JetIDCut(Particle *p){
return p->n90 >= 4 && p->mtf <= 0.8 && p->momentum.Pt() > 10000 && p->N_HasPVInfo > 3;
}
