// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT
#include "Mint/ReturnPhaseSpace.h"
using namespace MINT;

double ReturnPhaseSpace::RealVal(IDalitzEvent& evt){
  return evt.phaseSpace();
}
