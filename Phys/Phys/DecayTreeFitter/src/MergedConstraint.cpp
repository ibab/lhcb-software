#include "FitParams.h"
#include "ParticleBase.h"
#include "MergedConstraint.h"

namespace DecayTreeFitter
{

  ErrCode
  MergedConstraint::project(const FitParams& fitpar, 
			    Projection& p) const 
  {
    ErrCode status ;
    for(constraintlist::const_iterator it = m_list.begin() ;
	it != m_list.end() ; ++it) {
      status |= (*it)->project(fitpar,p) ;
      p.incrementOffset((*it)->dim()) ;
    }
 
    return status ;
  }
  
  void MergedConstraint::print(std::ostream& os) const
  {
    os << "Merged constraint: " << std::endl ;
    for(constraintlist::const_iterator it = m_list.begin() ;
	it != m_list.end() ; ++it) {
      os << "          " ;
      (*it)->print(os) ;
    }
  }
  
}

