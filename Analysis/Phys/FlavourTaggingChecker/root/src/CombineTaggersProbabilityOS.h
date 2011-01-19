#ifndef CombineTaggersProbabilityOS_H
#define CombineTaggersProbabilityOS_H 1

#include <vector>
#include <cmath>
#include "Particle.h"
#include "FlavourTag.h"
#include "Tagger.h"


/** @class CombineTaggersProbabilityOS
 *
 *  Tool to combine taggers probability treating them as independent
 *
 *  @author Marc Grabalosa
 *  @date   2011-01-18
 */
class CombineTaggersProbabilityOS {

 public:

  CombineTaggersProbabilityOS( );
  ~CombineTaggersProbabilityOS(){ delete theTag; } ///< Destructor

  FlavourTag* combineTaggers( Taggers& vtg );

 
 private:
  double m_omegamaxbin_OS, m_omegascale_OS, m_ProbMin_OS;
  double m_P0_Cal_OS, m_P1_Cal_OS, m_Eta_Cal_OS;
  FlavourTag* theTag;

};

#endif 
