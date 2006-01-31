#ifndef MUONALGS_MUONCHAMBERRESPONSE_H
#define MUONALGS_MUONCHAMBERRESPONSE_H 1

#include "GaudiKernel/RndmGenerators.h"
 
class MuonChamberResponse  {
public:
//constructor
  MuonChamberResponse(Rndm::Numbers* flat,Rndm::Numbers* poisson,double mean );
  MuonChamberResponse(){};
  ~MuonChamberResponse(){};
  int extractNoise();
private:
  Rndm::Numbers* p_flat;
  Rndm::Numbers* p_poisson;
  double m_chamberNoiseRate;
 };

#endif
