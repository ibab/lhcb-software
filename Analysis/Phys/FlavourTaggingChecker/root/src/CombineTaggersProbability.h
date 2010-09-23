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
  double m_omegamaxbin;
  double m_omegascale ;
  double m_ProbMin    ;
  FlavourTag* theTag;

};

#endif 
