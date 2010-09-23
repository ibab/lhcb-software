#ifndef CombineTaggersNN_H
#define CombineTaggersNN_H 1

#include <vector>
#include <cmath>
#include "Particle.h"
#include "FlavourTag.h"
#include "Tagger.h"
#include "TMVA/Reader.h"
#include "TMVA/Factory.h"


/** @class CombineTaggersNN
 *
 *  Combine taggers decision using a NNet which takes as input the mistags
 *
 *  @author Marc Grabalosa
 *  @date   2010-07-15
 */
class CombineTaggersNN {

 public:

  CombineTaggersNN( );
  ~CombineTaggersNN(); ///< Destructor

  FlavourTag* combineTaggers( Taggers& vtg );

 
 private:

  double m_ProbMin, m_ProbMin_NN;
  double m_P0_NN, m_P1_NN;
  double m_omegascale, m_omegamaxbin;

  TString getPath(std::string);
  TString m_TmvaMethod;
  TString m_WeightsFile;
  TMVA::Reader* m_reader_comb;
  float pmu, pe, pk, pss, pvtx;
  FlavourTag* theTag;

};

#endif 
