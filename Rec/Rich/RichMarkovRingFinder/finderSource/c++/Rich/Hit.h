

#ifndef LESTER_HIT_H
#define LESTER_HIT_H

#include "Small2Vector.h"
#include "GenericRingFinder/GenericHit.h"

namespace Lester 
{
  
  class Hit : public Small2Vector 
  {
  public:
    Hit ( const double x, const double y   ) : Small2Vector(x,y) { }
    Hit ( const GenRingF::GenericHit & hit ) : Small2Vector(hit.x(), hit.y()) { }
  };
  
}

#endif
