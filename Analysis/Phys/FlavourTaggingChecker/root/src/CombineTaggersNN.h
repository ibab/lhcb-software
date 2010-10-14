#ifndef CombineTaggersNN_H
#define CombineTaggersNN_H 1

#include <vector>
#include <cmath>
#include "Particle.h"
#include "FlavourTag.h"
#include "Tagger.h"

#include "NNcomb.h"


/** @class CombineTaggersNN
 *
 *  Combine taggers decision using a NNet which takes as input the mistags
 *
 *  @author Marc Grabalosa, Marco Musy
 *  @date   2010-09-07
 */

class CombineTaggersNN {

 public:

  CombineTaggersNN( );
  ~CombineTaggersNN(); ///< Destructor

  FlavourTag* combineTaggers( Taggers& vtg );

 
 private:

  double m_ProbMin;
  double m_P0_NN, m_P1_NN, m_P2_NN;
  double m_omegascale, m_omegamaxbin;

  TString getPath(std::string);
  float pmu, pe, pk, pss, pvtx;
  FlavourTag* theTag;

};

#endif 
