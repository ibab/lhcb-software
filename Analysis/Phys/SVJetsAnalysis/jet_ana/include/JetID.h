
#include "ROOTOscar.h"
bool JetID(Particle *p){
return p->n90 >= 4 && p->mtf <= 0.8;
}