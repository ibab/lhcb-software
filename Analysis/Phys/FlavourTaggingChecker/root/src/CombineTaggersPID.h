#ifndef CombineTaggersPID_H
#define CombineTaggersPID_H 1

#include <vector>
#include <cmath>
#include "Particle.h"
#include "FlavourTag.h"
#include "Tagger.h"


/** @class CombineTaggersPID
 *
 *  Combine taggers decision based on the assigned PID
 *
 *  @author Marco Musy, Marc Grabalosa
 *  @date   2010-06-29
 */
class CombineTaggersPID {

 public:

  CombineTaggersPID( );
  ~CombineTaggersPID(){ delete theTag; } ///< Destructor

  FlavourTag* combineTaggers( Taggers& vtg );

 
 private:
  std::vector<int> m_pid_cats_bs;
  std::vector<int> m_pid_cats_bu;
  std::vector<int> m_index;
  bool m_isBs;
  double m_cat5,m_cat4,m_cat3,m_cat2,m_cat1;

  FlavourTag* theTag;

};

#endif 
