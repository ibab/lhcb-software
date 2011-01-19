#ifndef CombineTaggersProbability_H
#define CombineTaggersProbability_H 1

#include <vector>
#include <cmath>
#include "Particle.h"
#include "FlavourTag.h"
#include "Tagger.h"


/** @class CombineTaggersProbability
 *
 *  Tool to combine taggers probability treating them as independent
 *
 *  @author Marco Musy
 *  @date   2010-07-15
 */
class CombineTaggersProbability {

 public:

  CombineTaggersProbability( );
  ~CombineTaggersProbability(){ delete theTag; } ///< Destructor

  FlavourTag* combineTaggers( Taggers& vtg );

 
 private:
  double m_omegamaxbin, m_omegascale, m_ProbMin;
  double m_P0_Cal_All, m_P1_Cal_All, m_Eta_Cal_All;
  FlavourTag* theTag;

};

#endif 
